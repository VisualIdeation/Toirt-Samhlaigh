/*
 * Point4.cpp - Methods for Point4 class.
 * 
 * Author: Patrick O'Leary
 * Created: November 12, 2007
 * Copyright 2007. All rights reserved.
 */
#include <cmath>

#include <MATH/Point4.h>
#include <UTILITY/Stringify.h>

/*
 * Point4 - Constructor for Point4.
 */
Point4::Point4(void) {
	x=0.0;
	y=0.0;
	z=0.0;
	w=0.0;
}

/*
 * Point4 - Constructor for Point4.
 *
 * parameter _x - double
 * parameter _y - double
 * parameter _z - double
 */
Point4::Point4(double _x, double _y, double _z) {
	x = _x;
	y = _y;
	z = _z;
	w = 0.0;
} // end Point4()

/*
 * Point4 - Constructor for Point4.
 *
 * parameter _x - double
 * parameter _y - double
 * parameter _z - double
 * parameter _w - double
 */
Point4::Point4(double _x, double _y, double _z, double _w) {
	x = _x;
	y = _y;
	z = _z;
	w = _w;
} // end Point4()

/*
 * ~Point4 - Destructor for Point4.
 */
Point4::~Point4(void) {
} // end ~Point4()

/*
 * distance
 *
 * parameter p - Point4*
 * return - double
 */
double Point4::distance(Point4* p) {
	Point4* tmp = new Point4(x - p->getX(), y - p->getY(), z - p->getZ());
	double _distance = sqrt((tmp->getX() * tmp->getX()) + (tmp->getY() * tmp->getY()) + (tmp->getZ() * tmp->getZ()));
	delete tmp;
	return _distance;
} // end distance()

/*
 * distance
 *
 * parameter p - Point4
 * return - double
 */
double Point4::distance(Point4 p) {
	Point4* tmp = new Point4(x - p.getX(), y - p.getY(), z - p.getZ());
	double _distance = sqrt((tmp->getX() * tmp->getX()) + (tmp->getY() * tmp->getY()) + (tmp->getZ() * tmp->getZ()));
	delete tmp;
	return _distance;
} // end distance()

/*
 * get
 *
 * parameter dimension - int
 * return - double
 */
double Point4::get(int dimension) {
	if (dimension==0)
		return x;
	if (dimension==1)
		return y;
	if (dimension==2)
		return z;
	if (dimension==3)
		return w;
} // end get()
;
/*
 * setPoint4
 *
 * parameter p - Point4*
 */
void Point4::setPoint4(Point4* p) {
	x=p->getX();
	y=p->getY();
	z=p->getZ();
	w=p->getW();
} // end setPoint4()

/*
 * setPoint4
 *
 * parameter p - Point4
 */
void Point4::setPoint4(Point4 p) {
	x = p.getX();
	y = p.getY();
	z = p.getZ();
	w = p.getW();
} // end setPoint4()

/*
 * getW
 *
 * return - double
 */
double Point4::getW(void) {
	return w;
} // end getW()

/*
 * setW
 *
 * parameter _w - double
 */
void Point4::setW(double _w) {
	w = _w;
} // end setW()

/*
 * getX
 *
 * return x - double
 */
double Point4::getX(void) {
	return x;
} // end getX()

/*
 * setX
 *
 * parameter _x - double
 */
void Point4::setX(double _x) {
	x = _x;
} // end setX()

/*
 * setXYZ
 *
 * parameter _x - double
 * parameter _y - double
 * parameter _z - double
 */
void Point4::setXYZ(double _x, double _y, double _z) {
	x = _x;
	y = _y;
	z = _z;
} // end setXYZ()

/*
 * getY
 *
 * return y - double
 */
double Point4::getY(void) {
	return y;
} // end getY()

/*
 * setY
 *
 * parameter _y - double
 */
void Point4::setY(double _y) {
	y = _y;
} // end setY()

/*
 * getZ
 * return z - double
 */
double Point4::getZ(void) {
	return z;
} // end getZ()

/*
 * setZ
 * parameter _z - double
 */
void Point4::setZ(double _z) {
	z = _z;
} // end setZ()

/*
 * midPoint
 *
 * parameter p - Point4*
 * return - Point4*
 */
Point4* Point4::midPoint(Point4* p) {
	double _x;
	double _y;
	double _z;
	double _w;
	_x = (x + p->getX()) * 0.5;
	_y = (y + p->getY()) * 0.5;
	_z = (z + p->getZ()) * 0.5;
	_w = (w + p->getW()) * 0.5;
	return new Point4(_x, _y, _z, _w);
} // end midPoint()

/*
 * normalize
 */
void Point4::normalize(void) {
	double length = sqrt((x * x) + (y * y) + (z * z));

	if (length > 0.0) {
		x /= length;
		y /= length;
		z /= length;
	} // end if
	else {
		x = 0.0;
		y = 0.0;
		z = 0.0;
	} // end else
} // end normalize()

/*
 * toString
 * return - std::string
 */
std::string Point4::toString(void) {
	std::string _string = " " + Stringify::toString(x) + " " + Stringify::toString(y) + " " + Stringify::toString(z) + " "
			+ Stringify::toString(w);
	return _string;
} // end toString()

/*
 * weightedMidPoint
 *
 * parameter p - Point4*
 * parameter xWeight - double
 * parameter yWeight - double
 * parameter zWeight - double
 * parameter wWeight - double
 * return - Point4*
 */
Point4* Point4::weightedMidPoint(Point4* p, double xWeight, double yWeight, double zWeight, double wWeight) {
	double _x;
	double _y;
	double _z;
	double _w;
	_x = (x + p->getX()) * xWeight;
	_y = (y + p->getY()) * yWeight;
	_z = (z + p->getZ()) * zWeight;
	_w = (w + p->getW()) * wWeight;
	return new Point4(_x, _y, _z, _w);
} // end weightedMidPoint()
