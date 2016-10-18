/*
 * Consumer.h
 *
 *  Created on: Jul 6, 2016
 *      Author: nilav
 */
#include<iostream>
#include<librdkafka/rdkafkacpp.h>
#ifndef CONSUMER_H_
#define CONSUMER_H_

using namespace std;
class Consumer {
/*
protected:
	string getKafkaIp() {
		return kafkaIp;
	}

	string getTopic() {
		return topic;
	}
*/

public:
	string consume(string kafkaip, string topic1, string topic2);

/*
private:
	string kafkaIp;
	string topic;
	void readEnv(const char * x);
*/

};

class ExampleConsumeCb: public Consumer{
 public:
  void msg_consume(RdKafka::Message* message, void* opaque,  RdKafka::Conf *conf, RdKafka::Conf *tconf, string errstr, int partition, string topic);
};
/*

class ExampleEventCb{
 public:
  void event_cb (RdKafka::Event &event);
};
*/

#endif /* CONSUMER_H_ */
