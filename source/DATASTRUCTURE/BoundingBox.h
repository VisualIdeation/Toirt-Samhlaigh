/*
 * BoundingBox.h
 *
 *       6*--------*7
 *       /|       /|
 *      / |      / |
 *     /  |     /  |
 *    /  2*----/---*3
 *  4*--------*5  /
 *   |  /     |  /
 *   | /      | /
 *   |/       |/
 *  0*--------*1
 *
 * Author: Patrick O'Leary
 * Created: November 3, 2007
 * Copyright 2007. All rights reserved.
 */
#ifndef BOUNDINGBOX_H_origin
#define BOUNDINGBOX_H_

#include <string>

#define FrontFace 0
#define RightFace 1
#define BackFace 2
#define LeftFace 3
#define BottomFace 4
#define TopFace 5

#define corner(point,coordinate) point*3+coordinate

// begin Forward Declarations
class Matrix4x4;
// end Forward Declarations

class BoundingBox {
public:
	BoundingBox(void);
	BoundingBox(float* _data);
	BoundingBox(float originX, float originY, float originZ, float extentX, float extentY, float extentZ);
	~BoundingBox(void);
	void adjustCoordinates(float dx, float dy, float dz);
	bool collision(float* point);
	float dotProduct(float* a, float* b);
	void findExtrema(void);
	float getCenter(int dimension);
	float getCenterX(void);
	float getCenterY(void);
	float getCenterZ(void);
	float* getCorner(int which);
	float* getCorners(void);
	void setCorners(float originX, float originY, float originZ, float extentX, float extentY, float extentZ);
	float getDistanceToCorner(float* p, int which);
	float getDistanceToFace(float* p, int face);
	float* getFaceMidPoint(int which);
	float* getMaximum(int index);
	float* getMaximum(void);
	int getMaximumIndex(void);
	float* getMidPoint(int p1, int p2);
	float* getMinimum(int index);
	float* getMinimum(void);
	float getMinimumDistance(float* point);
	int getMinimumIndex(void);
	float getRadius(void);
	float getW(int which);
	float* getWeightedCenter(float xWeight, float yWeight, float zWeight);
	float* getWeightedFaceMidPoint(int which, float xWeight, float yWeight, float zWeight);
	float* getWeightedMidPoint(int p1, int p2, float xWeight, float yWeight, float zWeight);
	float getX(int which);
	float getY(int which);
	float getZ(int which);
	float* midPoint(float* p, float* q);
	float* subtract(float* a, float* b);
	std::string toString(void) ;
	void transform(const Matrix4x4 & matrix);
	float* weightedMidPoint(float* p, float* q, float xWeight, float yWeight, float zWeight);
private:
	float* data;
	int maximumIndex;
	int minimumIndex;
};

#endif /*BOUNDINGBOX_H_*/
