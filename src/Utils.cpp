/*
 * Utils.cpp
 *
 *  Created on: Jul 21, 2016
 *      Author: lenovo
 */
#include "../headers/Utils.h"
#include "../libraries/Random.h"
#include <string>

Utils::Utils() {
	// TODO Auto-generated constructor stub

}

Utils::~Utils() {
	// TODO Auto-generated destructor stub
}

std::string Utils::GenerateRandomNumber(int length){
	Random* random = new Random();
	std::string requestid = random->randomStrGen(length);
	delete random;
	return requestid;
}
