/*
 * BoundingSphere.cpp - Methods for BoundingSphere class.
 * 
 * Author: Patrick O'Leary
 * Created: January 24, 2008
 * Copyright 2008. All rights reserved.
 */
#include <DATASTRUCTURE/BoundingSphere.h>

/*
 * BoundingSphere - Constructor for BoundingSphere class
 */
BoundingSphere::BoundingSphere(void) {
	center = new float[3];
	center[0] = 0.0f;
	center[1] = 0.0f;
	center[2] = 0.0f;
	radius = 0.0f;
} // end BoundingSphere()

/*
 * BoundingSphere - Constructor for BoundingSphere class
 * 
 * parameter x - float
 * parameter y - float
 * parameter z - float
 * parameter _radius - float
 */
BoundingSphere::BoundingSphere(float x, float y, float z, float _radius) {
	center = new float[3];
	center[0] = x;
	center[1] = y;
	center[2] = z;
	radius = _radius;
} // end BoundingSphere()

/*
 * BoundingSphere - Constructor for BoundingSphere class
 * 
 * parameter _center - float*
 * parameter _radius - float
 */
BoundingSphere::BoundingSphere(float* _center, float _radius) {
	center = new float[3];
	center[0] = _center[0];
	center[1] = _center[1];
	center[2] = _center[2];
	radius = _radius;
} // end BoundingSphere()

/*
 * ~BoundingSphere - Destructor for BoundingSphere class
 */
BoundingSphere::~BoundingSphere(void) {
	delete[] center;
} // end ~BoundingSphere()

/*
 * getCenter
 * 
 * return float*
 */
float* BoundingSphere::getCenter(void) {
	return center;
} // end getCenter()

/*
 * setCenter
 * 
 * parameter x - float
 * parameter y - float
 * parameter z - float
 */
void BoundingSphere::setCenter(float x, float y, float z) {
	center[0] = x;
	center[1] = y;
	center[2] = z;
} // end setCenter()

/*
 * setCenter
 * 
 * parameter _center - float*
 */
void BoundingSphere::setCenter(float* _center) {
	center[0] = _center[0];
	center[1] = _center[1];
	center[2] = _center[2];
} // end setCenter()

/*
 * getRadius
 * 
 * return - float
 */
float BoundingSphere::getRadius(void) {
	return radius;
} // end getRadius()

/*
 * setRadius
 * 
 * parameter _radius - float
 */
void BoundingSphere::setRadius(float _radius) {
	radius = _radius;
} // end setRadius()
