/*
 * Producer.h
 *
 *  Created on: Jul 18, 2016
 *      Author: lenovo
 */

#ifndef PRODUCER_H_
#define PRODUCER_H_
#include <stdlib.h>
#include <string>
#include "librdkafka/rdkafkacpp.h"

class Producer {
public:
	Producer();
	virtual ~Producer();
	void produce(std::string topic_str, RdKafka::Conf *conf, RdKafka::Conf *tconf, std::string errstr, int32_t partition, std::string message);
};

#endif /* PRODUCER_H_ */
