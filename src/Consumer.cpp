/*
 * Consumer.cpp
 *
 *  Created on: Jul 6, 2016
 *      Author: nilav
 */

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <librdkafka/rdkafkacpp.h>
#include "../headers/Consumer.h"
#include "../headers/Producer.h"
#include <sys/types.h>
#include <unistd.h>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <fstream>

using namespace boost::property_tree;
using namespace std;

/* Consumer mode
 * Create consumer using accumulated global configuration.*/

int use_ccb = 0;
static int eof_cnt = 0;
static int partition_cnt = 0;
static int verbosity = 1;
static long msg_cnt = 0;
static int64_t msg_bytes = 0;
static bool run = true;
static bool exit_eof = false;

static void sigterm(int sig) {
	run = false;
}

void ExampleConsumeCb::msg_consume(RdKafka::Message* message, void* opaque,
		RdKafka::Conf *conf, RdKafka::Conf *tconf, string errstr, int partition,
		string topic) {
	switch (message->err()) {
	case RdKafka::ERR__TIMED_OUT:
		break;

	case RdKafka::ERR_NO_ERROR:
		//Real message
		msg_cnt++;
		msg_bytes += message->len();

		if (verbosity >= 3)
			std::cerr << "Read msg at offset " << message->offset()
					<< std::endl;
		RdKafka::MessageTimestamp ts;
		ts = message->timestamp();

		if (verbosity >= 2
				&& ts.type
						!= RdKafka::MessageTimestamp::MSG_TIMESTAMP_NOT_AVAILABLE) {
			std::string tsname = "?";
			if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_CREATE_TIME)
				tsname = "create time";
			else if (ts.type
					== RdKafka::MessageTimestamp::MSG_TIMESTAMP_LOG_APPEND_TIME)
				tsname = "log append time";
			std::cout << "Timestamp: " << tsname << " " << ts.timestamp
					<< std::endl;
		}

		if (verbosity >= 2 && message->key()) {
			std::cout << "Key: " << *message->key() << std::endl;
		}

		if (verbosity >= 1) {
//          std::string  value = static_cast<const char *>(message->payload());
			printf("%.*s\n", static_cast<int>(message->len()),
					static_cast<const char *>(message->payload()));
			/*Producer* producer = new Producer();
			 producer->produce(topic,conf,tconf,errstr,partition,"SUCCESS");*/

		}
		break;

	case RdKafka::ERR__PARTITION_EOF:
		//Last message
		if (exit_eof && ++eof_cnt == partition_cnt) {
			std::cerr << "%% EOF reached for all " << partition_cnt
					<< " partition(s)" << std::endl;
			run = false;
		}
		break;

	case RdKafka::ERR__UNKNOWN_TOPIC:
	case RdKafka::ERR__UNKNOWN_PARTITION:
		std::cerr << "Consume failed: " << message->errstr() << std::endl;
		run = false;
		break;

	default:
		//Errors
		std::cerr << "Consume failed: " << message->errstr() << std::endl;
		run = false;
	}
}

string Consumer::consume(string kafkaip, string consume_topic, string requestid) {
	string results = "";
	//readEnv(env_p);    // get the configuration attributes

	//std::string brokers = this->getKafkaIp();
	std::string brokers = kafkaip;
	std::string errstr;
	std::string topic_str = consume_topic;
	std::string mode;
	std::string debug;
	int32_t partition = 0;
	int64_t start_offset = RdKafka::Topic::OFFSET_STORED;
	bool do_conf_dump = false;

	//* Create configuration objects

	RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
	RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

	//* Set configuration properties

	conf->set("metadata.broker.list", brokers, errstr);
	conf->set("auto.commit.enable", "true", errstr);
	conf->set("auto.commit.interval.ms", "1000", errstr);
	conf->set("auto.offset.reset", "largest", errstr);
	conf->set("group.id", "15667", errstr);
	tconf->set("offsets.storage", "kafka", errstr);

	if (!debug.empty()) {
		if (conf->set("debug", debug, errstr) != RdKafka::Conf::CONF_OK) {
			std::cerr << errstr << std::endl;
			exit(1);
		}
	}

	if (do_conf_dump) {
		int pass;

		for (pass = 0; pass < 2; pass++) {
			std::list<std::string> *dump;
			if (pass == 0) {
				dump = conf->dump();
				std::cout << "# Global config" << std::endl;
			} else {
				dump = tconf->dump();
				std::cout << "# Topic config" << std::endl;
			}

			for (std::list<std::string>::iterator it = dump->begin();
					it != dump->end();) {
				std::cout << *it << " = ";
				it++;
				std::cout << *it << std::endl;
				it++;
			}
			std::cout << std::endl;
		}
		exit(0);
	}

	signal(SIGINT, sigterm);
	signal(SIGTERM, sigterm);

	RdKafka::Consumer *consumer = RdKafka::Consumer::create(conf, errstr);
	if (!consumer) {
		std::cerr << "Failed to create consumer: " << errstr << std::endl;
		exit(1);
	}

	std::cout << "% Created consumer " << consumer->name() << std::endl;

	//* Create topic handle.

	RdKafka::Topic *topic = RdKafka::Topic::create(consumer, topic_str, tconf,errstr);

	if (!topic) {
		std::cerr << "Failed to create topic: " << errstr << std::endl;
		exit(1);
	}

	//* Start consumer for topic+partition at start offset

	RdKafka::ErrorCode resp = consumer->start(topic, partition, start_offset);
	if (resp != RdKafka::ERR_NO_ERROR) {
		std::cerr << "Failed to start consumer: " << RdKafka::err2str(resp)
				<< std::endl;
		exit(1);
	}

	//* Consume messages

	while (run) {
		sleep(1);
		RdKafka::Message *msg = consumer->consume(topic, partition, 1000);

		/*try-catch to produce error message for invalid json*/
		if(msg->len() > 0){
			// jsonParse(value);

				char* value = static_cast<char *>(msg->payload());
				std::stringstream ss;
				ss<< value;
				//std::istringstream is(value);
		try {
			ptree pt;
			cout << "message consumed :: " << value << endl;
			read_json(ss, pt);
			cout << "reached here" << endl;
			string recieved_requestid = pt.get < string > ("requestid"); //string containing request id
			cout << "requestID :: " << recieved_requestid << endl;
			results = pt.get < string > ("result"); //string containing request query

			if(recieved_requestid == requestid){
				cout << "requestid found :: " << recieved_requestid;
				break;
			}
			pt.clear();
		} catch (std::exception const& ex) {
			cout << "parse exception :: " << ex.what();
			cout << endl;
		}

		}

		/*   ExampleConsumeCb exampleConsumerCb;
		 *exampleConsumerCb.msg_consume(msg, NULL,conf,tconf,errstr,0, topic2);
		 */
		delete msg;

		consumer->poll(10);
	}

	//* Stop consumer
	cout << "consumer stopped" << endl;

	consumer->stop(topic, partition);
	consumer->poll(1000);

/*
	delete topic;
	delete consumer;
*/

	/* Wait for RdKafka to decommission.
	 * This is not strictly needed (when check outq_len() above), but
	 * allows RdKafka to clean up all its resources before the application
	 * exits so that memory profilers such as valgrind wont complain about
	 * memory leaks.
	 */
	//RdKafka::wait_destroyed(5000);
	return results;
}

/*void Consumer::readEnv(const char * x) {

		if (x==NULL) {
	 cout<<"config file is empty";
	 }

	 Document document;
	 document.Parse(x);

	 Value& k = document["kafkaip"];
	 Value& to = document["topic"];

	 this->kafkaIp = k.GetString();
	 this->topic = to.GetString();

	//TODO uncomment after env variables are set
	this->kafkaIp = "172.16.10.14";
	this->topic = "jcoretodmz";

}*/
