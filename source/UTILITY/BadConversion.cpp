/*
 * BadConversion.cpp - Methods for BadConversion class.
 * 
 * Author: Patrick O'Leary
 * Created: February 20, 2008
 * Copyright 2008. All rights reserved.
 */
#include <UTILITY/BadConversion.h>

/*
 * BadConversion
 */
BadConversion::BadConversion(const std::string & str) :
	std::runtime_error(str) {
} // end BadConversion()
