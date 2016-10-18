/*
 * Configuration.cpp

 *
 *  Created on: Jul 22, 2016
 *      Author: lenovo
 */

#include "../headers/Configuration.h"
#include <rapidjson/document.h>
#include <librdkafka/rdkafkacpp.h>
#include <iostream>
#include <unistd.h>
#include <string>
using namespace std;
using namespace rapidjson;

string Configuration::message_broker ="";
string Configuration::dtojtopic ="";
string Configuration::jtodtopic ="";

Configuration::Configuration() {
	// TODO Auto-generated constructor stub
}

Configuration::~Configuration() {
	// TODO Auto-generated destructor stub
}

void Configuration::readEnv(const char * x) {

	//Parsing the json and getting the individual value

	Document document;
	document.Parse(x);

	Value& k = document["kafkaip"];
	Value& dmztojc = document["dtojtopic"];
	Value& jctodmz = document["jtodtopic"];

	Configuration::message_broker = k.GetString();
	Configuration::dtojtopic = dmztojc.GetString();
	Configuration::jtodtopic = jctodmz.GetString();
}

