/*
 * BoundingBox.cpp - Methods for BoundingBox class.
 * 
 * Author: Patrick O'Leary
 * Created: November 11, 2007
 * Copyright 2007. All rights reserved.
 */

#include <cmath>
#include <limits>

#include <DATASTRUCTURE/BoundingBox.h>
#include <MATH/Matrix4x4.h>

/*
 * BoundingBox
 */
BoundingBox::BoundingBox(void) {
	data = new float[24];
	setCorners(0, 0, 0, 1, 1, 1);
	maximumIndex = 7;
	minimumIndex = 0;
} // end BoundingBox()

/*
 * BoundingBox
 *
 * parameter originX - float
 * parameter originY - float
 * parameter originZ - float
 * parameter extentX - float
 * parameter extentY - float
 * parameter extentZ - float
 */
BoundingBox::BoundingBox(float originX, float originY, float originZ, float extentX, float extentY, float extentZ) {
	data = new float[24];
	setCorners(originX, originY, originZ, extentX, extentY, extentZ);
	maximumIndex = 7;
	minimumIndex = 0;
} // end BoundingBox()

/*
 * BoundingBox
 * 
 * parameter _data - float*
 */
BoundingBox::BoundingBox(float* _data) {
	data = new float[24];
	for (int i = 0; i< 24; i++)
		data[i] = _data[i];
	maximumIndex = 7;
	minimumIndex = 0;
} // end BoundingBox()

BoundingBox::~BoundingBox(void) {
	delete[] data;
} // end ~BoundingBox()

/*
 * adjustCoordinates
 *
 * parameter dx - float
 * parameter dy - float
 * parameter dz - float
 */
void BoundingBox::adjustCoordinates(float dx, float dy, float dz) {
	for (int i = 0; i < 8; i++) {
		if (i%2 == 0) {
			data[corner(i,0)] += dx;
		} else {
			data[corner(i,0)] -= dx;
		}
		if (i < 4) {
			data[corner(i,1)] += dy;
		} else {
			data[corner(i,1)] -= dy;
		}
		if (((i/2) == 0) || ((i/2) == 2)) {
			data[corner(i,2)] += dz;
		} else {
			data[corner(i,2)] -= dz;
		}
	}
} // end adjustCoordinates()

/*
 * collision
 *
 * parameter point - float*
 * return - bool
 */
bool BoundingBox::collision(float* point) {
    if (point[0] < data[corner(0,0)]) return false;
    if (point[1] < data[corner(0,1)]) return false;
    if (point[2] < data[corner(0,2)]) return false;
    if (point[0] > data[corner(7,0)]) return false;
    if (point[1] > data[corner(7,1)]) return false;
    if (point[2] > data[corner(7,2)]) return false;
	return true;
} // end collision()

/*
 * dotProduct
 * 
 * parameter a - float*
 * parameter b - float*
 * return - float
 */
float BoundingBox::dotProduct(float* a, float* b) {
	float c = 0;
	c = a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
	return c;
} // end dotProduct()


/*
 * findExtrema
 */
void BoundingBox::findExtrema(void) {
	minimumIndex = 0;
	maximumIndex = 0;
	for (int i = 0; i < 8; i++) {
		if (data[corner(minimumIndex,2)] > data[corner(i,2)]) {
			minimumIndex = i;
		} // end if
		if (data[corner(maximumIndex,2)] < data[corner(i,2)]) {
			maximumIndex = i;
		} // end if
	} // end for
} // end findExtrema()

/*
 * getCenter
 *
 * parameter dimension - int
 * return - float
 */
float BoundingBox::getCenter(int dimension) {
	return data[corner(0,dimension)] + (data[corner(7,dimension)] - data[corner(0,dimension)]) * 0.5f;
} // end getCenter()

/*
 * getCenterX
 *
 * return - float
 */
float BoundingBox::getCenterX(void) {
	return data[corner(0,0)] + (data[corner(7,0)] - data[corner(0,0)]) * 0.5f;
} // end getCenterX()

/*
 * getCenterY
 *
 * return - float
 */
float BoundingBox::getCenterY(void) {
	return data[corner(0,1)] + (data[corner(7,1)] - data[corner(0,1)]) * 0.5f;
} // end getCenterY()

/*
 * getCenterZ
 *
 * return - float
 */
float BoundingBox::getCenterZ(void) {
	return data[corner(0,2)] + (data[corner(7,2)] - data[corner(0,2)]) * 0.5f;
} // end getCenterZ()


/*
 * getCorner
 *
 * parameter which - int
 * return corner - float*
 */
float* BoundingBox::getCorner(int which) {
	float* point = new float[3];
	point[0]=data[corner(which,0)];
	point[1]=data[corner(which,1)];
	point[2]=data[corner(which,2)];
	return point;
} // end getCorner()

/*
 * getCorners
 *
 * return corner - float*
 */
float* BoundingBox::getCorners(void) {
	return data;
} // end getCorners()

/*
 * setCorners
 *
 * parameter originX - float
 * parameter originY - float
 * parameter originZ - float
 * parameter extentX - float
 * parameter extentY - float
 * parameter extentZ - float
 */
void BoundingBox::setCorners(float originX, float originY, float originZ, float extentX, float extentY, float extentZ) {
	for (int i = 0; i < 8; i++) {
		if (i%2 == 0) {
			data[corner(i,0)] = originX;
		} else {
			data[corner(i,0)] = extentX;
		}
		if (i < 4) {
			data[corner(i,1)] = originY;
		} else {
			data[corner(i,1)] = extentY;
		}
		if (((i/2) == 0) || ((i/2) == 2)) {
			data[corner(i,2)] = originZ;
		} else {
			data[corner(i,2)] = extentZ;
		}
	}
} // end setCorners()

/*
 * getDistanceToCorner
 *
 * parameter p - float*
 * parameter which - int
 * return - float
 */
float BoundingBox::getDistanceToCorner(float* p, int which) {
	float distance = 0;
	float* q = getCorner(which);
	float* pminusq = subtract(p,q);
	delete[] q;
	distance = dotProduct(pminusq,pminusq);
	delete[] pminusq;
	return distance;
} // end getDistanceToCorner()

/*
 * getDistanceToFace
 *
 * parameter p - float*
 * parameter face - int
 * return - float
 */
float BoundingBox::getDistanceToFace(float* p, int face) {
	float distance = 0;
	float* s;
	float* t;
	switch (face) {
		case BottomFace:
			s = getCorner(0);
			t = getCorner(3);
			break;
		case TopFace:
			s = getCorner(4);
			t = getCorner(7);
			break;
		case BackFace:
			s = getCorner(2);
			t = getCorner(7);
			break;
		case FrontFace:
			s = getCorner(0);
			t = getCorner(5);
			break;
		case RightFace:
			s = getCorner(1);
			t = getCorner(7);
			break;
		case LeftFace:
			s = getCorner(0);
			t = getCorner(6);
			break;
	} // end switch
	float* n = new float[3];
	for(int i = 0; i < 3; i++) {
		if (p[i] < s[i]) {
			n[i] = s[i];
		} else if (p[i] > t[i]) {
			n[i] = t[i];
		} else {
			n[i] = p[i];
		}
	}
	delete[] s;
	delete[] t;
	float* pminusn = subtract(p,n);
	delete[] n;
	distance = dotProduct(pminusn,pminusn);
	delete[] pminusn;
	return distance;
} // end getMinimumDistance()

/*
 * getFaceMidPoint
 *	 		windowType AnaglyphicStereo
 * parameter which - int
 * return - float*
 */
float* BoundingBox::getFaceMidPoint(int which) {
	float* point0;
	float* point1;
	float* point2;
	float* point3;
	float* p1;
	float* p2;
	float* p3;
	switch (which) {
		case BottomFace:
			point0 = getCorner(0);
			point1 = getCorner(1);
			point2 = getCorner(2);
			point3 = getCorner(3);
			p1 = midPoint(point0, point1);
			p2 = midPoint(point2, point3);
			delete[] point0;
			delete[] point1;
			delete[] point2;
			delete[] point3;
			break;
		case TopFace:
			point0 = getCorner(4);
			point1 = getCorner(5);
			point2 = getCorner(6);
			point3 = getCorner(7);
			p1 = midPoint(point0, point1);
			p2 = midPoint(point2, point3);
			delete[] point0;
			delete[] point1;
			delete[] point2;
			delete[] point3;
			break;
		case BackFace:
			point0 = getCorner(2);
			point1 = getCorner(3);
			point2 = getCorner(6);
			point3 = getCorner(7);
			p1 = midPoint(point0, point1);
			p2 = midPoint(point2, point3);
			delete[] point0;
			delete[] point1;
			delete[] point2;
			delete[] point3;
			break;
		case FrontFace:
			point0 = getCorner(0);
			point1 = getCorner(1);
			point2 = getCorner(4);
			point3 = getCorner(5);
			p1 = midPoint(point0, point1);
			p2 = midPoint(point2, point3);
			delete[] point0;
			delete[] point1;
			delete[] point2;
			delete[] point3;
			break;
		case RightFace:
			point0 = getCorner(1);
			point1 = getCorner(3);
			point2 = getCorner(5);
			point3 = getCorner(7);
			p1 = midPoint(point0, point1);
			p2 = midPoint(point2, point3);
			delete[] point0;
			delete[] point1;
			delete[] point2;
			delete[] point3;
			break;
		case LeftFace:
			point0 = getCorner(0);
			point1 = getCorner(2);
			point2 = getCorner(4);
			point3 = getCorner(6);
			p1 = midPoint(point0, point1);
			p2 = midPoint(point2, point3);
			delete[] point0;
			delete[] point1;
			delete[] point2;
			delete[] point3;
			break;
	} // end switch
	p3 = midPoint(p1, p2);
	delete[] p1;
	delete[] p2;
	return p3;
} // end getFaceMidPoint()

/*
 * getMaximum
 *
 * parameter index - int
 * return - float*
 */
float* BoundingBox::getMaximum(int index) {
	float* point = new float[3];
	point[0] = data[corner(index,0)];
	point[1] = data[corner(index,1)];
	point[2] = data[corner(index,2)];
	return point;
} // end getMaximum()

/*
 * getMaximum
 *
 * return corner[maximumIndex] - float*
 */
float* BoundingBox::getMaximum(void) {
	float* point = new float[3];
	point[0] = data[corner(maximumIndex,0)];
	point[1] = data[corner(maximumIndex,1)];
	point[2] = data[corner(maximumIndex,2)];
	return point;
} // end getMaximum()

/*
 * getMaximumIndex
 *
 * return maximumIndex - int
 */
int BoundingBox::getMaximumIndex(void) {
	return maximumIndex;
} // end getMaximumIndex()

/*
 * getMidPoint
 *
 * parameter p1 - int
 * parameter p2 - int
 * return - float*
 */
float* BoundingBox::getMidPoint(int p1, int p2) {
	float* point0;
	float* point1;
	float* point;
	point0 = getCorner(p1);
	point1 = getCorner(p2);
	point = midPoint(point0, point1);
	delete[] point0;
	delete[] point1;
	return point;
} // end getMidPoint()

/*
 * getMinimum
 *
 * parameter index - int
 * return - float*
 */
float* BoundingBox::getMinimum(int index) {
	float* point = new float[3];
	point[0] = data[corner(index,0)];
	point[1] = data[corner(index,1)];
	point[2] = data[corner(index,2)];
	return point;
} // end getMinimum()

/*
 * getMinimum
 *
 * return corner[minimumIndex] - float*
 */
float* BoundingBox::getMinimum(void) {
	float* point = new float[3];
	point[0] = data[corner(minimumIndex,0)];
	point[1] = data[corner(minimumIndex,1)];
	point[2] = data[corner(minimumIndex,2)];
	return point;
} // end getMinimum()

/*
 * getMinimumDistance
 *
 * parameter point - float*
 * return - float
 */
float BoundingBox::getMinimumDistance(float* point) {
	float minimumDistance = std::numeric_limits<float>::max();
	for (int i = 0; i< 6; i++) {
		float _minimumDistance = getDistanceToFace(point,i);
		if (_minimumDistance < minimumDistance) minimumDistance = _minimumDistance;
	}
	for (int i = 0; i < 8; i++) {
		float _minimumDistance = getDistanceToCorner(point,i);
		if (_minimumDistance < minimumDistance) minimumDistance = _minimumDistance;
	}
	return minimumDistance;
} // end getMinimumDistance()

/*
 * getMinimumIndex
 *
 * return minimumIndex - int
 */
int BoundingBox::getMinimumIndex(void) {
	return minimumIndex;
} // end getMinimumIndex()

/*
 * getRadius
 *
 * return - float
 */
float BoundingBox::getRadius(void) {
	float x = data[corner(7,0)] - getCenterX();
	float y = data[corner(7,1)] - getCenterY();
	float z = data[corner(7,2)] - getCenterZ();
	return (float) sqrt(double(x*x + y*y + z*z));
} // end getRadius()

/*
 * getW
 *
 * parameter which - int
 * return - float
 */
float BoundingBox::getW(int which) {
	return 1.0f;
} // end getW()

/*
 * getWeightedCenter
 *
 * parameter xWeight - float
 * parameter yWeight - float
 * parameter zWeight - float
 * return - float*
 */
float* BoundingBox::getWeightedCenter(float xWeight, float yWeight, float zWeight) {
	float* p = getCorner(0);
	float* q = getCorner(7);
	float* point = weightedMidPoint(p, q, xWeight, yWeight, zWeight);
	delete[] p;
	delete[] q;
	return point;
} // end getWeightedCenter()

/*
 * getWeightedFaceMidPoint
 *
 * parameter which - int
 * parameter xWeight - float
 * parameter yWeight - float
 * parameter zWeight - float
 * return - float*
 */
float* BoundingBox::getWeightedFaceMidPoint(int which, float xWeight, float yWeight, float zWeight) {
	float* point0;
	float* point1;
	float* point2;
	float* point3;
	float* p1;
	float* p2;
	float* p3;
	switch (which) {
		case BottomFace:
			point0 = getCorner(0);
			point1 = getCorner(1);
			point2 = getCorner(2);
			point3 = getCorner(3);
			p1 = weightedMidPoint(point0, point1, xWeight, 0.5f, 0.5f);
			p2 = weightedMidPoint(point2, point3, xWeight, 0.5f, 0.5f);
			p3 = weightedMidPoint(p1, p2, 0.5, 0.5, zWeight);
			delete[] p1;
			delete[] p2;
			delete[] point0;
			delete[] point1;
			delete[] point2;
			delete[] point3;
			break;
		case TopFace:
			point0 = getCorner(4);
			point1 = getCorner(5);
			point2 = getCorner(6);
			point3 = getCorner(7);
			p1 = weightedMidPoint(point0, point1, xWeight, 0.5f, 0.5f);
			p2 = weightedMidPoint(point2, point3, xWeight, 0.5f, 0.5f);
			p3 = weightedMidPoint(p1, p2, 0.5, 0.5, zWeight);
			delete[] p1;
			delete[] p2;
			delete[] point0;
			delete[] point1;
			delete[] point2;
			delete[] point3;
			break;
		case BackFace:
			point0 = getCorner(2);
			point1 = getCorner(3);
			point2 = getCorner(6);
			point3 = getCorner(7);
			p1 = weightedMidPoint(point0, point1, xWeight, 0.5f, 0.5f);
			p2 = weightedMidPoint(point2, point3, xWeight, 0.5f, 0.5f);
			p3 = weightedMidPoint(p1, p2, 0.5f, yWeight, 0.5f);
			delete[] p1;
			delete[] p2;
			delete[] point0;
			delete[] point1;
			delete[] point2;
			delete[] point3;
			break;
		case FrontFace:
			point0 = getCorner(0);
			point1 = getCorner(1);
			point2 = getCorner(4);
			point3 = getCorner(5);
			p1 = weightedMidPoint(point0, point1, xWeight, 0.5f, 0.5f);
			p2 = weightedMidPoint(point2, point3, xWeight, 0.5f, 0.5f);
			p3 = weightedMidPoint(p1, p2, 0.5f, yWeight, 0.5f);
			delete[] p1;
			delete[] p2;
			delete[] point0;
			delete[] point1;
			delete[] point2;
			delete[] point3;
			break;
		case RightFace:
			point0 = getCorner(1);
			point1 = getCorner(3);
			point2 = getCorner(5);
			point3 = getCorner(7);
			p1 = weightedMidPoint(point0, point1, 0.5f, 0.5f, zWeight);
			p2 = weightedMidPoint(point2, point3, 0.5f, 0.5f, zWeight);
			p3 = weightedMidPoint(p1, p2, 0.5f, yWeight, 0.5f);
			delete[] p1;
			delete[] p2;
			delete[] point0;
			delete[] point1;
			delete[] point2;
			delete[] point3;
			break;
		case LeftFace:
			point0 = getCorner(0);
			point1 = getCorner(2);
			point2 = getCorner(4);
			point3 = getCorner(6);
			p1 = weightedMidPoint(point0, point1, 0.5f, 0.5f, zWeight);
			p2 = weightedMidPoint(point2, point3, 0.5f, 0.5f, zWeight);
			p3 = weightedMidPoint(p1, p2, 0.5f, yWeight, 0.5f);
			delete[] p1;
			delete[] p2;
			delete[] point0;
			delete[] point1;
			delete[] point2;
			delete[] point3;
			break;
	} // end switch
	return p3;
} // end getWeightedFaceMidPoint()

/*
 * getWeightedMidPoint
 *
 * parameter p1 - int
 * parameter p2 - int
 * parameter xWeight - float
 * parameter yWeight - float
 * parameter zWeight - float
 * return - float*
 */
float* BoundingBox::getWeightedMidPoint(int p1, int p2, float xWeight, float yWeight, float zWeight) {
	float* point0;
	float* point1;
	float* point;
	point0 = getCorner(p1);
	point1 = getCorner(p2);
	point = weightedMidPoint(point0, point1, xWeight, yWeight, zWeight);
	delete[] point0;
	delete[] point1;
	return point;
} // end getWeightedMidPoint()

/*
 * getX
 *
 * parameter which - int
 * return - float
 */
float BoundingBox::getX(int which) {
	return data[corner(which,0)];
} // end getX()

/*
 * getY
 *
 * parameter which - int
 * return - float
 */
float BoundingBox::getY(int which) {
	return data[corner(which,1)];
} // end getY()

/*
 * getZ
 *
 * parameter which - int
 * return - float
 */
float BoundingBox::getZ(int which) {
	return data[corner(which,2)];
} // end getZ()

/*
 * midPoint
 *
 * parameter p - float*
 * parameter q - float*
 * return - float*
 */
float* BoundingBox::midPoint(float* p, float* q) {
	float* point = new float[3];
	point[0] = (p[0] + q[0]) * 0.5f;
	point[1] = (p[1] + q[1]) * 0.5f;
	point[2] = (p[2] + q[2]) * 0.5f;
	return point;
} // end midPoint()

/*
 * subtract
 * 
 * parameter a - float*
 * parameter b - float*
 * return - float*
 */
float* BoundingBox::subtract(float* a, float* b) {
	float* c = new float[3];
	for (int i = 0; i < 3; i++) c[i] = a[i]-b[i];
	return c;
} // end subtract()

/*
 * toString
 * 
 * return - std::string
 */
std::string BoundingBox::toString(void) {
	return "\n";
} // end toString()

/*
 * transform
 *
 * parameter matrix - const Matrix4x4 &
 */
void BoundingBox::transform(const Matrix4x4 & matrix) {
	float* p;
	for (int i = 0; i < 8; i++) {
		p = getCorner(i);
		data[corner(i,0)] = (matrix.get(0, 0) * p[0]) + (matrix.get(1, 0) * p[1]) + (matrix.get(2, 0) * p[2]) + (matrix.get( 3, 0)
				* 1.0f);
		data[corner(i,1)] = (matrix.get(0, 1) * p[0]) + (matrix.get(1, 1) * p[1]) + (matrix.get(2, 1) * p[2]) + (matrix.get( 3, 1)
				* 1.0f);
		data[corner(i,2)] = (matrix.get(0, 2) * p[0]) + (matrix.get(1, 2) * p[1]) + (matrix.get(2, 2) * p[2]) + (matrix.get( 3, 2)
				* 1.0f);
		delete[] p;
	} // end for
} // end transform()

/*
 * weightedMidPoint
 *
 * parameter p - float*
 * parameter q - float*
 * parameter xWeight - float
 * parameter yWeight - float
 * parameter zWeight - float
 * return - float*
 */
float* BoundingBox::weightedMidPoint(float* p, float* q, float xWeight, float yWeight, float zWeight) {
	float* point = new float[3];
	point[0] = (p[0] + q[0]) * xWeight;
	point[1] = (p[1] + q[1]) * yWeight;
	point[2] = (p[2] + q[2]) * zWeight;
	return point;
} // end weightedMidPoint()

