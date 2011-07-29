/*
 * BadConversion.h - Class for a standard exception for converting object data members to strings.
 * 
 * Author: Patrick O'Leary
 * Created: February 20, 2008
 * Copyright 2008. All rights reserved.
 */
#ifndef BADCONVERSION_H_
#define BADCONVERSION_H_

#include <stdexcept>
#include <string>

class BadConversion : public std::runtime_error {
public:
	BadConversion(const std::string & str);
};

#endif /*BADCONVERSION_H_*/
