/*
 * Producer.cpp
 *
 *  Created on: Jul 18, 2016
 *      Author: lenovo
 */

#include "../headers/Producer.h"
#include "librdkafka/rdkafkacpp.h"
#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;

Producer::Producer() {
	// TODO Auto-generated constructor stub

}

Producer::~Producer() {
	// TODO Auto-generated destructor stub
}

void Producer::produce(string topic_str, RdKafka::Conf *conf,
		RdKafka::Conf *tconf, string errstr, int32_t partition,
		string message) {

	/*
	 * Create producer using accumulated global configuration.
	 */
	RdKafka::Producer *producer = RdKafka::Producer::create(conf, errstr);
	if (!producer) {
		cerr << "Failed to create producer: " << errstr << endl;
		exit(1);
	}

	/*
	 * Create topic handle.
	 */
	RdKafka::Topic *topic = RdKafka::Topic::create(producer, topic_str, tconf,
			errstr);
	if (!topic) {
		cerr << "Failed to create topic: " << errstr << endl;
		exit(1);
	}

	RdKafka::ErrorCode resp = producer->produce(topic, partition,
			RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
			const_cast<char *>(message.c_str()), message.size(),
			NULL, NULL);

	if (resp != RdKafka::ERR_NO_ERROR)
		cerr << "% Produce failed: " << RdKafka::err2str(resp) << endl;
	else
		cout << message << endl;
	producer->poll(1000);
}
