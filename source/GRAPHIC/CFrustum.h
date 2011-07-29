/*
 * CFrustum.h
 *
 * Author: Patrick O'Leary
 * Created: January 23, 2008
 * Copyright 2008. All rights reserved.
 * Credit: Mark Morley - For more information on view frustum culling visit http://www.artlum.com/pub/frustumculling.html This class is based on
 *  this tutorial.
 */

#ifndef CFRUSTUM_H_
#define CFRUSTUM_H_

#define RIGHT_PLANE 0
#define LEFT_PLANE 1
#define BOTTOM_PLANE 2
#define TOP_PLANE 3
#define FAR_PLANE 4
#define NEAR_PLANE 5

#define FULL_OUTSIDE 0
#define PARTIAL_INSIDE 1
#define FULL_INSIDE 2

class CFrustum {
public:
	CFrustum();
	~CFrustum();
	bool cubeInFrustum(float* fPoint, float fSize);
	int cubeLocation(float* fPoint, float fSize);
	float lodSphereInFrustum(float* fPoint, float fRadius);
	float* matrixMultiply(void);
	void normalize(int plane);
	bool pointInFrustum(float* fPoint);
	bool rightParallelepipedInFrustum(float* fMinimum, float* fMaximum);
	int rightParallelepipedLocation(float* fMinimum, float* fMaximum);
	bool sphereInFrustum(float* fPoint, float fRadius);
	int sphereLocation(float* fPoint, float fRadius);
	void updateFrustum();
private:
	float fFrustum[6][4];
};

#endif /*CFRUSTUM_H_*/
