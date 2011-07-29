/*
 * ComparableOcNode.cpp - Methods for class ComparableOcNode.
 * 
 * Author: Patrick O'Leary
 * Created: February 20, 2008
 * Copyright 2008. All rights reserved.
 */
#include <sstream>

#include <DATASTRUCTURE/ComparableOcNode.h>
#include <DATASTRUCTURE/OcNode.h>
#include <UTILITY/BadConversion.h>

/* Allocate space for static variable */
bool ComparableOcNode::greater;

/*
 * ComparableOcNode - Constructor
 * 
 * parameter _which - int
 * parameter _value - float
 */
ComparableOcNode::ComparableOcNode(int _which, float _value, OcNode * _ocNode) :
	which(_which), value(_value), ocNode(_ocNode) {
} // end ComparableOcNode()

/*
 * ComparableOcNode - Copy constructor (SHALLOW)
 * 
 * parameter rhs - const ComparableOcNode &
 */
ComparableOcNode::ComparableOcNode(const ComparableOcNode & rhs) :
	which(rhs.which), value(rhs.value), ocNode(rhs.ocNode) {
} // end ComparableOcNode()

/*
 * ~ComparableOcNode - Destructor
 */
ComparableOcNode::~ComparableOcNode(void) {
} // edn ~ComparableOcNode()

/*
 * getOcNode
 * 
 * return - OcNode *
 */
OcNode * ComparableOcNode::getOcNode(void) const {
	return ocNode;
} // end getOcNode()

/*
 * setOcNode
 * 
 * parameter _ocNode - OcNode *
 */
void ComparableOcNode::setOcNode(OcNode * _ocNode) {
	ocNode = _ocNode;
} // end setOcNode()

/*
 * getValue
 * 
 * return - float
 */
float ComparableOcNode::getValue(void) const {
	return value;
} // end getValue()

/*
 * setValue
 * 
 * parameter _value - float
 */
void ComparableOcNode::setValue(float _value) {
	value = _value;
} // end setValue()

/*
 * getWhich
 * 
 * return - int
 */
int ComparableOcNode::getWhich(void) const {
	return which;
} // end getWhich()

/*
 * setWhich
 * 
 * parameter _which - int
 */
void ComparableOcNode::setWhich(int _which) {
	which = _which;
} // end setWhich()

/*
 * minimum - Compare lesser
 */
void ComparableOcNode::minimum(void) {
	greater = false;
} // end minimum()

/*
 * maximum - Compare greater
 */
void ComparableOcNode::maximum(void) {
	greater = true;
} // end maximum()

/*
 * operator= - (SHALLOW)
 * 
 * parameter rhs - const ComparableOcNode &
 * return - const ComparableOcNode &
 */
const ComparableOcNode & ComparableOcNode::operator=(const ComparableOcNode & rhs) {
	if (this != &rhs) {
		which = rhs.which;
		value = rhs.value;
		ocNode = rhs.ocNode;
	}
	return *this;
} // end operator=()

/*
 * operator<
 * 
 * parameter rhs - const ComparableOcNode &
 * return bool
 */
bool ComparableOcNode::operator<(const ComparableOcNode & rhs) const {
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
std::string ComparableOcNode::toString(void) {
	std::ostringstream o;
	if (!(o << which << " : " << value))
		throw BadConversion("ComparableOcNode::toString()");
	return o.str();
} // end toString()
