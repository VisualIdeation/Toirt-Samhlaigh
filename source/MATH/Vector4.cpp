/*
 * Vector4.cpp - Methods for Vector4 class.
 * 
 * Author: Patrick O'Leary
 * Created: November 12, 2007
 * Copyright 2007. All rights reserved.
 */
#include <cmath>

#include <MATH/Vector4.h>

/*
 * Vector4 - Constructor for Vector4.
 */
Vector4::Vector4(void) {
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 0.0f;
} // end Vector4()

/*
 * Vector4 - Constructor for Vector4.
 *
 * parameter _v - float*
 */
Vector4::Vector4(float* _v) {
	x = _v[0];
	y = _v[1];
	z = _v[2];
	w = 1.0f;
} // end Vector4()

/*
 * Vector4 - Constructor for Vector4.
 *
 * parameter _x - float
 * parameter _y - float
 * parameter _z - float
 * parameter _w - float
 */
Vector4::Vector4(float _x, float _y, float _z, float _w) {
	x = _x;
	y = _y;
	z = _z;
	w = _w;
} // end Vector4()

/*
 * Vector4 - Constructor for Vector4.
 *
 * parameter p - Point4
 */
Vector4::Vector4(Point4 p) {
	x = float(p.getX());
	y = float(p.getY());
	z = float(p.getZ());
	w = float(p.getW());
} // end Vector4()

/*
 * Vector4 - Constructor for Vector4.
 *
 * parameter p - Point4*
 */
Vector4::Vector4(Point4* p) {
	x = float(p->getX());
	y = float(p->getY());
	z = float(p->getZ());
	w = float(p->getW());
} // end Vector4()

/*
 * Vector4 - Constructor for Vector4.
 *
 * parameter p1 - Point4
 * parameter p2 - Point4
 */
Vector4::Vector4(Point4 p1, Point4 p2) {
	x = float(p2.getX() - p1.getX());
	y = float(p2.getY() - p1.getY());
	z = float(p2.getZ() - p1.getZ());
	w = float(p2.getW() - p1.getW());
} // end Vector4()

/*
 * ~Vector4 - Destructor for Vector4.
 */
Vector4::~Vector4(void) {
} // end ~Vector4()

/*
 * add
 *
 * parameter scalar - float
 */
void Vector4::add(float scalar) {
	x += scalar;
	y += scalar;
	z += scalar;
	w += scalar;
} // end add()

/*
 * add
 *
 * parameter v - Vector4
 */
void Vector4::add(Vector4 v) {
	x += v.getX();
	y += v.getY();
	z += v.getZ();
	w += v.getW();
} // end add()

/*
 * add
 *
 * parameter v - Vector4*
 */
void Vector4::add(Vector4* v) {
	x += v->getX();
	y += v->getY();
	z += v->getZ();
	w += v->getW();
} // end add()

/*
 * clear
 */
void Vector4::clear(void) {
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 0.0f;
} // end clear()

/*
 * cosAngle
 *
 * parameter v - Vector4
 * return - float
 */
float Vector4::cosAngle(Vector4 v) {
	return dot(v) / magnitude() / v.magnitude();
} // end cosAngle()

/*
 * cross
 *
 * parameter v - Vector4
 * return - Vector4*
 */
Vector4* Vector4::cross(Vector4* v) {
	return new Vector4((y * v->getZ()) - (z * v->getY()),
			(z * v->getX()) - (x * v->getZ()),
			(x * v->getY()) - (y * v->getX()), 0.0f);
} // end cross()

/*
 * divide
 *
 * parameter scalar - float
 */
void Vector4::divide(float scalar) {
	if (scalar != 0.0f) {
		x /= scalar;
		y /= scalar;
		z /= scalar;
		w /= scalar;
	} // end if
	else {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 0.0f;
	} // end else
} // end divide()

/*
 * dot
 *
 * parameter v - Vector4
 * return - double
 */
float Vector4::dot(Vector4 v) {
	return (x * v.getX()) + (y * v.getY()) + (z * v.getZ()) + (w * v.getW());
} // end dot()

/*
 * dot
 *
 * parameter v - Vector4*
 * return - double
 */
float Vector4::dot(Vector4* v) {
	return (getX() * v->getX()) + (getY() * v->getY()) + (getZ() * v->getZ()) + (getW() * v->getW());
} // end dot()

/*
 * dot
 *
 * parameter v - const Vector4*
 * return - double
 */
float Vector4::dot(const Vector4* v) {
	return (getX() * v->getX()) + (getY() * v->getY()) + (getZ() * v->getZ()) + (getW() * v->getW());
} // end dot()

/*
 * getW
 *
 * return - float
 */
float Vector4::getW(void) {
	return w;
} // end getW()

/*
 * getW
 *
 * return - float
 */
float Vector4::getW(void) const {
	return w;
} // end getW()

/*
 * setW
 *
 * parameter _w - float
 */
void Vector4::setW(float _w) {
	w = _w;
} // end setW()

/*
 * getX
 *
 * return - float
 */
float Vector4::getX(void) {
	return x;
} // end getX()

/*
 * getX
 *
 * return - float
 */
float Vector4::getX(void) const {
	return x;
} // end getX()

/*
 * setX
 *
 * parameter _x - float
 */
void Vector4::setX(float _x) {
	x = _x;
} // end setX()

/*
 * setXYZW
 *
 * parameter _x - float
 * parameter _y - float
 * parameter _z - float
 * parameter _w - float
 */
void Vector4::setXYZW(float _x, float _y, float _z, float _w) {
	x = _x;
	y = _y;
	z = _z;
	w = _w;
} // end setZ()

/*
 * getY
 *
 * return - float
 */
float Vector4::getY(void) {
	return y;
} // end getY()

/*
 * getY
 *
 * return - float
 */
float Vector4::getY(void) const {
	return y;
} // end getY()

/*
 * setY
 *
 * parameter _y - float
 */
void Vector4::setY(float _y) {
	y = _y;
} // end setY()

/*
 * getZ
 *
 * return - float
 */
float Vector4::getZ(void) {
	return z;
} // end getZ()

/*
 * getZ
 *
 * return - float
 */
float Vector4::getZ(void) const {
	return z;
} // end getZ()

/*
 * setZ
 *
 * parameter _z - float
 */
void Vector4::setZ(float _z) {
	z = _z;
} // end setZ()

/*
 * magnitude
 *
 * return - float
 */
float Vector4::magnitude(void) {
	return float(sqrt(double(magnitudeSquared())));
} // end magnitude()

/*
 * magnitudeSquared
 *
 * return - float
 */
float Vector4::magnitudeSquared(void) {
	return (x * x) + (y * y) + (z * z) + (w * w);
} // end magnitudeSquared()

/*
 * multiply
 *
 * parameter scalar - float
 */
void Vector4::multiply(float scalar) {
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
} // end multiply()

/*
 * negate
 */
void Vector4::negate(void) {
	x = -x;
	y = -y;
	z = -z;
	w = -w;
} // end negate()

/*
 * subtract
 *
 * parameter scalar - float
 */
void Vector4::subtract(float scalar) {
	x -= scalar;
	y -= scalar;
	z -= scalar;
	w -= scalar;
} // end subtract()

/*
 * subtract
 *
 * parameter v - Vector4
 */
void Vector4::subtract(Vector4 v) {
	x -= v.getX();
	y -= v.getY();
	z -= v.getZ();
	w -= v.getW();
} // end subtract()

/*
 * subtract
 *
 * parameter v - Vector4*
 */
void Vector4::subtract(Vector4* v) {
	x -= v->getX();
	y -= v->getY();
	z -= v->getZ();
	w -= v->getW();
} // end subtract()

/*
 * toString
 *
 * return - std::string
 */
std::string Vector4::toString(void) {
	return "" + Stringify::toString(x) + " " + Stringify::toString(y) + " " + Stringify::toString(z) + " " + Stringify::toString(w);
} // end toString()

/*
 * unit
 *
 * return - Vector4*
 */
Vector4* Vector4::unit(void) {
	float _x;
	float _y;
	float _z;
	float _w;
	float m = magnitude();

	if (m <= 0.0) {
		_x = 0.0f;
		_y = 0.0f;
		_z = 0.0f;
		_w = 0.0f;
	} // end if
	else {
		_x = x / m;
		_y = y / m;
		_z = z / m;
		_w = w / m;
	} // end else

	return new Vector4(_x, _y, _z, _w);
} // end unit()

/*
 * unitize
 */
void Vector4::unitize(void) {
	float m = 0.0f;
	m = magnitude();

	if (m <= 0.0) {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 0.0f;
	} // end if
	else {
		x /= m;
		y /= m;
		z /= m;
		w /= m;
	} // end else
} // end unitize()
