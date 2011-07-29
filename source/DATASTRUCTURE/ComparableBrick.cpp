/*
 * ComparableBrick.cpp - Methods for class ComparableBrick.
 * 
 * Author: Patrick O'Leary
 * Created: February 20, 2008
 * Copyright 2008. All rights reserved.
 */
#include <sstream>

#include <DATASTRUCTURE/Brick.h>
#include <DATASTRUCTURE/ComparableBrick.h>
#include <UTILITY/BadConversion.h>

/* Allocate space for static variable */
bool ComparableBrick::greater;

/*
 * ComparableBrick - Constructor
 * 
 * parameter _which - int
 * parameter _value - float
 */
ComparableBrick::ComparableBrick(int _which, float _value, Brick * _brick) :
	which(_which), value(_value), brick(_brick) {
} // end ComparableBrick()

/*
 * ComparableBrick - Copy constructor (SHALLOW)
 * 
 * parameter rhs - const ComparableBrick &
 */
ComparableBrick::ComparableBrick(const ComparableBrick & rhs) :
	which(rhs.which), value(rhs.value), brick(rhs.brick) {
} // end ComparableBrick()

/*
 * ~ComparableBrick - Destructor
 */
ComparableBrick::~ComparableBrick(void) {
	brick = NULL;
} // end ~ComparableBrick()

/*
 * getBrick - May mutate the brick
 * 
 * return - Brick *
 */
Brick * ComparableBrick::getBrick(void) {
	return brick;
} // end getBrick()

/*
 * setBrick
 * 
 * parameter _brick - Brick *
 */
void ComparableBrick::setBrick(Brick * _brick) {
	brick = _brick;
} // end setBrick()

/*
 * getValue
 * 
 * return - float
 */
float ComparableBrick::getValue(void) const {
	return value;
} // end getValue()

/*
 * setValue
 * 
 * parameter _value - float
 */
void ComparableBrick::setValue(float _value) {
	value = _value;
} // end setValue()

/*
 * getWhich
 * 
 * return - int
 */
int ComparableBrick::getWhich(void) const {
	return which;
} // end getWhich()

/*
 * setWhich
 * 
 * parameter _which - int
 */
void ComparableBrick::setWhich(int _which) {
	which = _which;
} // end setWhich()

/*
 * minimum - Compare lesser
 */
void ComparableBrick::minimum(void) {
   greater = false;
} // end minimum()

/*
 * maximum - Compare greater
 */
void ComparableBrick::maximum(void) {
   greater = true;
} // end maximum()

/*
 * operator= - (SHALLOW)
 * 
 * parameter rhs - const ComparableBrick &
 * return - const ComparableBrick &
 */
const ComparableBrick & ComparableBrick::operator=(const ComparableBrick & rhs) {
	if (this != &rhs) {
		which = rhs.which;
		value = rhs.value;
		brick = rhs.brick;
	}
	return *this;
} // end operator=()

/*
 * operator<
 * 
 * parameter rhs - const ComparableBrick &
 * return bool
 */
bool ComparableBrick::operator<(const ComparableBrick & rhs) const {
	if(greater)
	return value > rhs.value;
	else
	return value < rhs.value;
} // end operator<()

/*
 * toString
 * 
 * return - std::string
 */
std::string ComparableBrick::toString(void) {
	std::ostringstream o;
	if (!(o << which << " : " << value))
		throw BadConversion("ComparableBrick::toString()");
	return o.str();
} // end toString()
