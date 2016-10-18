/*
 * Configuration.h
 *
 *  Created on: Jul 22, 2016
 *      Author: lenovo
 */
#include <librdkafka/rdkafkacpp.h>
#include <string>
#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

class Configuration {
public:
	Configuration();
	virtual ~Configuration();
	static std::string message_broker;
	static std::string dtojtopic ;
	static std::string jtodtopic;
	void readEnv(const char * x);
};

#endif /* CONFIGURATION_H_ */
