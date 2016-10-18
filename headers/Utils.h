/*
 * Utils.h
 *
 *  Created on: Jul 21, 2016
 *      Author: lenovo
 */
#include <string>
#ifndef UTILS_H_
#define UTILS_H_

class Utils {
public:
	Utils();
	virtual ~Utils();
	std::string GenerateRandomNumber(int length);
};

#endif /* UTILS_H_ */
