/*
 * Stringify.h - Class for converting data to strings.
 * 
 * Author: Patrick O'Leary
 * Created: November 12, 2007
 * Copyright 2007. All rights reserved.
 */
#ifndef STRINGIFY_H_
#define STRINGIFY_H_

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include <UTILITY/BadConversion.h>

class Stringify {
public:
	Stringify(void);
	~Stringify(void);
	static float toFloat(const char* str) {
		return atof(str);
	}
	;
	static int toInt(const char* str) {
		return atoi(str);
	}
	;
	static std::string toString(double x) {
		std::ostringstream o;
		if (!(o << x))
			throw BadConversion("toString(double)");
		return o.str();
	}
	;
	static std::string toString(float x) {
		std::ostringstream o;
		if (!(o << x))
			throw BadConversion("toString(float)");
		return o.str();
	}
	;
	static std::string toString(int x) {
		std::ostringstream o;
		if (!(o << x))
			throw BadConversion("toString(int)");
		return o.str();
	}
	;
};

#endif /*STRINGIFY_H_*/
