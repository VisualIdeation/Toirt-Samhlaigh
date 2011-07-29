/*
 * CFrustum.cpp - Methods for CFrustum class.
 * 
 * Author: Patrick O'Leary
 * Created: January 23, 2008
 * Copyright 2008. All rights reserved.
 */
#include <cmath>

#include <GL/gl.h>

#include <GRAPHIC/CFrustum.h>

/*
 * CFrustum - Constructor for CFrustum class.
 */
CFrustum::CFrustum() {
} // end CFrustum()

/*
 * ~CFrustum - Destructor for CFrustum class.
 */
CFrustum::~CFrustum() {
} // end ~CFrustum()

/*
 * cubeInFrustum - Return true when the cube intersects with the view frustum. The parameter fSize has to be the width of the cube divided by two.
 * 
 * parameter fPoint - float*
 * parameter fSize - float
 * return - bool
 */
bool CFrustum::cubeInFrustum(float* fPoint, float fSize) {
	float tol=-0;

	for (int currentPlane = 0; currentPlane<6; currentPlane++) {
		if (fFrustum[currentPlane][0] * (fPoint[0] - fSize) + fFrustum[currentPlane][1] * (fPoint[1] - fSize)
				+ fFrustum[currentPlane][2] * (fPoint[2] - fSize) + fFrustum[currentPlane][3] >= tol)
			continue;
		if (fFrustum[currentPlane][0] * (fPoint[0] + fSize) + fFrustum[currentPlane][1] * (fPoint[1] - fSize)
				+ fFrustum[currentPlane][2] * (fPoint[2] - fSize) + fFrustum[currentPlane][3] >= tol)
			continue;
		if (fFrustum[currentPlane][0] * (fPoint[0] - fSize) + fFrustum[currentPlane][1] * (fPoint[1] + fSize)
				+ fFrustum[currentPlane][2] * (fPoint[2] - fSize) + fFrustum[currentPlane][3] >= tol)
			continue;
		if (fFrustum[currentPlane][0] * (fPoint[0] + fSize) + fFrustum[currentPlane][1] * (fPoint[1] + fSize)
				+ fFrustum[currentPlane][2] * (fPoint[2] - fSize) + fFrustum[currentPlane][3] >= tol)
			continue;
		if (fFrustum[currentPlane][0] * (fPoint[0] - fSize) + fFrustum[currentPlane][1] * (fPoint[1] - fSize)
				+ fFrustum[currentPlane][2] * (fPoint[2] + fSize) + fFrustum[currentPlane][3] >= tol)
			continue;
		if (fFrustum[currentPlane][0] * (fPoint[0] + fSize) + fFrustum[currentPlane][1] * (fPoint[1] - fSize)
				+ fFrustum[currentPlane][2] * (fPoint[2] + fSize) + fFrustum[currentPlane][3] >= tol)
			continue;
		if (fFrustum[currentPlane][0] * (fPoint[0] - fSize) + fFrustum[currentPlane][1] * (fPoint[1] + fSize)
				+ fFrustum[currentPlane][2] * (fPoint[2] + fSize) + fFrustum[currentPlane][3] >= tol)
			continue;
		if (fFrustum[currentPlane][0] * (fPoint[0] + fSize) + fFrustum[currentPlane][1] * (fPoint[1] + fSize)
				+ fFrustum[currentPlane][2] * (fPoint[2] + fSize) + fFrustum[currentPlane][3] >= tol)
			continue;
		return false;
	}
	return true;
} // end cubeInFrustum()

/*
 * cubeLocation - Return FULL_OUTSIDE, PARTIAL_INSIDE or FULL_INSIDE. The parameter fSize has to be the width of the cube divided by two.
 * 
 * parameter fPoint - float*
 * parameter fSize - float
 * return - int
 */
int CFrustum::cubeLocation(float* fPoint, float fSize) {
	int insidePlaneCount = 0;

	for (int currentPlane = 0; currentPlane<6; currentPlane++) {
		int insideCount = 0;
		if (fFrustum[currentPlane][0] * (fPoint[0] - fSize) + fFrustum[currentPlane][1] * (fPoint[1] - fSize)
				+ fFrustum[currentPlane][2] * (fPoint[2] - fSize) + fFrustum[currentPlane][3] > 0)
			insideCount++;
		if (fFrustum[currentPlane][0] * (fPoint[0] + fSize) + fFrustum[currentPlane][1] * (fPoint[1] - fSize)
				+ fFrustum[currentPlane][2] * (fPoint[2] - fSize) + fFrustum[currentPlane][3] > 0)
			insideCount++;
		if (fFrustum[currentPlane][0] * (fPoint[0] - fSize) + fFrustum[currentPlane][1] * (fPoint[1] + fSize)
				+ fFrustum[currentPlane][2] * (fPoint[2] - fSize) + fFrustum[currentPlane][3] > 0)
			insideCount++;
		if (fFrustum[currentPlane][0] * (fPoint[0] + fSize) + fFrustum[currentPlane][1] * (fPoint[1] + fSize)
				+ fFrustum[currentPlane][2] * (fPoint[2] - fSize) + fFrustum[currentPlane][3] > 0)
			insideCount++;
		if (fFrustum[currentPlane][0] * (fPoint[0] - fSize) + fFrustum[currentPlane][1] * (fPoint[1] - fSize)
				+ fFrustum[currentPlane][2] * (fPoint[2] + fSize) + fFrustum[currentPlane][3] > 0)
			insideCount++;
		if (fFrustum[currentPlane][0] * (fPoint[0] + fSize) + fFrustum[currentPlane][1] * (fPoint[1] - fSize)
				+ fFrustum[currentPlane][2] * (fPoint[2] + fSize) + fFrustum[currentPlane][3] > 0)
			insideCount++;
		if (fFrustum[currentPlane][0] * (fPoint[0] - fSize) + fFrustum[currentPlane][1] * (fPoint[1] + fSize)
				+ fFrustum[currentPlane][2] * (fPoint[2] + fSize) + fFrustum[currentPlane][3] > 0)
			insideCount++;
		if (fFrustum[currentPlane][0] * (fPoint[0] + fSize) + fFrustum[currentPlane][1] * (fPoint[1] + fSize)
				+ fFrustum[currentPlane][2] * (fPoint[2] + fSize) + fFrustum[currentPlane][3] > 0)
			insideCount++;
		if (insideCount == 0)
			return FULL_OUTSIDE;
		if (insideCount == 8)
			insidePlaneCount++;
	}
	return (insidePlaneCount == 6) ? FULL_INSIDE : PARTIAL_INSIDE;
} // end cubeLocation()

/*
 * lodSphereInFrustum - Return distance from camera when the sphere is inside the view frustum. This is the case when it is never further behind any plane than its radius.
 * 
 * parameter fPoint - float*
 * parameter fRadius - float
 * return - float
 */
float CFrustum::lodSphereInFrustum(float* fPoint, float fRadius) {
	float distance;

	for (int currentPlane = 0; currentPlane<6; currentPlane++) {
		distance = fFrustum[currentPlane][0] * fPoint[0] + fFrustum[currentPlane][1] * fPoint[1] + fFrustum[currentPlane][2]
				* fPoint[2] + fFrustum[currentPlane][3];
		if (distance <= -fRadius) {
			return 0;
		}
	}
	return distance + fRadius;
}

/*
 * matrixMultiply
 * 
 * return - float*
 */
float* CFrustum::matrixMultiply(void) {
	float* fProjection = new float[16]; // projection matrix
	float* fModelView = new float[16]; // modelview matrix
	float* fClipping = new float[16]; // projection * modelview

	glGetFloatv(GL_PROJECTION_MATRIX, fProjection);
	glGetFloatv(GL_MODELVIEW_MATRIX, fModelView);
	fClipping[ 0] = fModelView[ 0] * fProjection[ 0] + fModelView[ 1] * fProjection[ 4] + fModelView[ 2] * fProjection[ 8]
			+ fModelView[ 3] * fProjection[12];
	fClipping[ 1] = fModelView[ 0] * fProjection[ 1] + fModelView[ 1] * fProjection[ 5] + fModelView[ 2] * fProjection[ 9]
			+ fModelView[ 3] * fProjection[13];
	fClipping[ 2] = fModelView[ 0] * fProjection[ 2] + fModelView[ 1] * fProjection[ 6] + fModelView[ 2] * fProjection[10]
			+ fModelView[ 3] * fProjection[14];
	fClipping[ 3] = fModelView[ 0] * fProjection[ 3] + fModelView[ 1] * fProjection[ 7] + fModelView[ 2] * fProjection[11]
			+ fModelView[ 3] * fProjection[15];

	fClipping[ 4] = fModelView[ 4] * fProjection[ 0] + fModelView[ 5] * fProjection[ 4] + fModelView[ 6] * fProjection[ 8]
			+ fModelView[ 7] * fProjection[12];
	fClipping[ 5] = fModelView[ 4] * fProjection[ 1] + fModelView[ 5] * fProjection[ 5] + fModelView[ 6] * fProjection[ 9]
			+ fModelView[ 7] * fProjection[13];
	fClipping[ 6] = fModelView[ 4] * fProjection[ 2] + fModelView[ 5] * fProjection[ 6] + fModelView[ 6] * fProjection[10]
			+ fModelView[ 7] * fProjection[14];
	fClipping[ 7] = fModelView[ 4] * fProjection[ 3] + fModelView[ 5] * fProjection[ 7] + fModelView[ 6] * fProjection[11]
			+ fModelView[ 7] * fProjection[15];

	fClipping[ 8] = fModelView[ 8] * fProjection[ 0] + fModelView[ 9] * fProjection[ 4] + fModelView[10] * fProjection[ 8]
			+ fModelView[11] * fProjection[12];
	fClipping[ 9] = fModelView[ 8] * fProjection[ 1] + fModelView[ 9] * fProjection[ 5] + fModelView[10] * fProjection[ 9]
			+ fModelView[11] * fProjection[13];
	fClipping[10] = fModelView[ 8] * fProjection[ 2] + fModelView[ 9] * fProjection[ 6] + fModelView[10] * fProjection[10]
			+ fModelView[11] * fProjection[14];
	fClipping[11] = fModelView[ 8] * fProjection[ 3] + fModelView[ 9] * fProjection[ 7] + fModelView[10] * fProjection[11]
			+ fModelView[11] * fProjection[15];

	fClipping[12] = fModelView[12] * fProjection[ 0] + fModelView[13] * fProjection[ 4] + fModelView[14] * fProjection[ 8]
			+ fModelView[15] * fProjection[12];
	fClipping[13] = fModelView[12] * fProjection[ 1] + fModelView[13] * fProjection[ 5] + fModelView[14] * fProjection[ 9]
			+ fModelView[15] * fProjection[13];
	fClipping[14] = fModelView[12] * fProjection[ 2] + fModelView[13] * fProjection[ 6] + fModelView[14] * fProjection[10]
			+ fModelView[15] * fProjection[14];
	fClipping[15] = fModelView[12] * fProjection[ 3] + fModelView[13] * fProjection[ 7] + fModelView[14] * fProjection[11]
			+ fModelView[15] * fProjection[15];
	delete[] fProjection;
	delete[] fModelView;
	return fClipping;
} // end matrixMultiply()

/*
 * normalize
 * 
 * parameter plane - int
 */
void CFrustum::normalize(int plane) {
	float magnitude = (float) sqrt(fFrustum[plane][0] * fFrustum[plane][0] + fFrustum[plane][1] * fFrustum[plane][1]
			+ fFrustum[plane][2] * fFrustum[plane][2]);
	fFrustum[plane][0] /= magnitude;
	fFrustum[plane][1] /= magnitude;
	fFrustum[plane][2] /= magnitude;
	fFrustum[plane][3] /= magnitude;
} // end normalize()

/*
 * pointInFrustum - Return true when the given point is inside the view frustum. This is the case when its distance is positive to all the frustum planes.
 * 
 * parameter fPoint - float*
 * return - bool
 */
bool CFrustum::pointInFrustum(float* fPoint) {
	for (int currentPlane = 0; currentPlane<6; currentPlane++) {
		if (fFrustum[currentPlane][0] * fPoint[0] + fFrustum[currentPlane][1] * fPoint[1] + fFrustum[currentPlane][2] * fPoint[2]
				+ fFrustum[currentPlane][3] <= 0) {
			return false;
		}
	}
	return true;
} // end pointInFrustum()

/*
 * rightParallelepipedInFrustum - Return true when the right parallelpiped intersects with the view frustum.
 * 
 * parameter fMinimum - float*
 * parameter fMaximum - float*
 * return - bool
 */
bool CFrustum::rightParallelepipedInFrustum(float* fMinimum, float* fMaximum) {
	for (int currentPlane = 0; currentPlane<6; currentPlane++) {
		if (fFrustum[currentPlane][0] * fMinimum[0] + fFrustum[currentPlane][1] * fMaximum[1] + fFrustum[currentPlane][2]
				* fMinimum[2] + fFrustum[currentPlane][3] > 0)
			continue;
		if (fFrustum[currentPlane][0] * fMinimum[0] + fFrustum[currentPlane][1] * fMaximum[1] + fFrustum[currentPlane][2]
				* fMaximum[2] + fFrustum[currentPlane][3] > 0)
			continue;
		if (fFrustum[currentPlane][0] * fMaximum[0] + fFrustum[currentPlane][1] * fMaximum[1] + fFrustum[currentPlane][2]
				* fMaximum[2] + fFrustum[currentPlane][3] > 0)
			continue;
		if (fFrustum[currentPlane][0] * fMaximum[0] + fFrustum[currentPlane][1] * fMaximum[1] + fFrustum[currentPlane][2]
				* fMinimum[2] + fFrustum[currentPlane][3] > 0)
			continue;
		if (fFrustum[currentPlane][0] * fMaximum[0] + fFrustum[currentPlane][1] * fMinimum[1] + fFrustum[currentPlane][2]
				* fMinimum[2] + fFrustum[currentPlane][3] > 0)
			continue;
		if (fFrustum[currentPlane][0] * fMaximum[0] + fFrustum[currentPlane][1] * fMinimum[1] + fFrustum[currentPlane][2]
				* fMaximum[2] + fFrustum[currentPlane][3] > 0)
			continue;
		if (fFrustum[currentPlane][0] * fMinimum[0] + fFrustum[currentPlane][1] * fMinimum[1] + fFrustum[currentPlane][2]
				* fMaximum[2] + fFrustum[currentPlane][3] > 0)
			continue;
		if (fFrustum[currentPlane][0] * fMinimum[0] + fFrustum[currentPlane][1] * fMinimum[1] + fFrustum[currentPlane][2]
				* fMinimum[2] + fFrustum[currentPlane][3] > 0)
			continue;
		return false;
	}
	return true;
} // end rightParallelepipedInFrustum()

/*
 * rightParallelepipedLocation - Return FULL_OUTSIDE, PARTIAL_INSIDE or FULL_INSIDE to indicate the position of the right parallelpiped.
 * 
 * parameter fMinimum - float*
 * parameter fMaximum - float*
 * return - int
 */
int CFrustum::rightParallelepipedLocation(float* fMinimum, float* fMaximum) {
	int insidePlaneCount = 0;

	for (int currentPlane = 0; currentPlane<6; currentPlane++) {
		int insideCount = 0;
		if (fFrustum[currentPlane][0] * fMinimum[0] + fFrustum[currentPlane][1] * fMaximum[1] + fFrustum[currentPlane][2]
				* fMinimum[2] + fFrustum[currentPlane][3] > 0)
			insideCount++;
		if (fFrustum[currentPlane][0] * fMinimum[0] + fFrustum[currentPlane][1] * fMaximum[1] + fFrustum[currentPlane][2]
				* fMaximum[2] + fFrustum[currentPlane][3] > 0)
			insideCount++;
		if (fFrustum[currentPlane][0] * fMaximum[0] + fFrustum[currentPlane][1] * fMaximum[1] + fFrustum[currentPlane][2]
				* fMaximum[2] + fFrustum[currentPlane][3] > 0)
			insideCount++;
		if (fFrustum[currentPlane][0] * fMaximum[0] + fFrustum[currentPlane][1] * fMaximum[1] + fFrustum[currentPlane][2]
				* fMinimum[2] + fFrustum[currentPlane][3] > 0)
			insideCount++;
		if (fFrustum[currentPlane][0] * fMaximum[0] + fFrustum[currentPlane][1] * fMinimum[1] + fFrustum[currentPlane][2]
				* fMinimum[2] + fFrustum[currentPlane][3] > 0)
			continue;
		if (fFrustum[currentPlane][0] * fMaximum[0] + fFrustum[currentPlane][1] * fMinimum[1] + fFrustum[currentPlane][2]
				* fMaximum[2] + fFrustum[currentPlane][3] > 0)
			insideCount++;
		if (fFrustum[currentPlane][0] * fMinimum[0] + fFrustum[currentPlane][1] * fMinimum[1] + fFrustum[currentPlane][2]
				* fMaximum[2] + fFrustum[currentPlane][3] > 0)
			insideCount++;
		if (fFrustum[currentPlane][0] * fMinimum[0] + fFrustum[currentPlane][1] * fMinimum[1] + fFrustum[currentPlane][2]
				* fMinimum[2] + fFrustum[currentPlane][3] > 0)
			insideCount++;
		if (insideCount == 0)
			return FULL_OUTSIDE;
		if (insideCount == 8)
			insidePlaneCount++;
	}
	return (insidePlaneCount == 6) ? FULL_INSIDE : PARTIAL_INSIDE;
} // end rightParallelepipedInFrustum()

/*
 * sphereInFrustum - Return true when the sphere is inside the view frustum. This is the case when it is never further behind any plane than its radius.
 * 
 * parameter fPoint - float*
 * parameter fRadius - float
 * return - bool
 */
bool CFrustum::sphereInFrustum(float* fPoint, float fRadius) {
	for (int currentPlane = 0; currentPlane<6; currentPlane++) {
		if (fFrustum[currentPlane][0] * fPoint[0] + fFrustum[currentPlane][1] * fPoint[1] + fFrustum[currentPlane][2] * fPoint[2]
				+ fFrustum[currentPlane][3] <= -fRadius) {
			return false;
		}
	}
	return true;
} // end sphereInFrustum()

/*
 * sphereLocation - Return FULL_OUTSIDE, PARTIAL_INSIDE or FULL_INSIDE to indicate the position of the sphere.
 * 
 * parameter fPoint - float*
 * parameter fRadius - float
 * return - int
 */
int CFrustum::sphereLocation(float* fPoint, float fRadius) {
	int insideCount = 0;
	float fDistance;

	for (int currentPlane=0; currentPlane<6; currentPlane++) {
		fDistance = fFrustum[currentPlane][0] * fPoint[0] + fFrustum[currentPlane][1] * fPoint[1] + fFrustum[currentPlane][2]
				* fPoint[2] + fFrustum[currentPlane][3];
		if (fDistance <= -fRadius)
			return FULL_OUTSIDE;
		if (fDistance > fRadius)
			insideCount++;
	}
	return (insideCount == 6) ? FULL_INSIDE : PARTIAL_INSIDE;
} // end sphereLocation()

/*
 * updateFrustum
 */
void CFrustum::updateFrustum() {
	float* fClipping = matrixMultiply();

	/*
	 * Right plane
	 */
	fFrustum[RIGHT_PLANE][0] = fClipping[ 3] - fClipping[ 0];
	fFrustum[RIGHT_PLANE][1] = fClipping[ 7] - fClipping[ 4];
	fFrustum[RIGHT_PLANE][2] = fClipping[11] - fClipping[ 8];
	fFrustum[RIGHT_PLANE][3] = fClipping[15] - fClipping[12];
	normalize(RIGHT_PLANE);
	/*
	 * Left plane
	 */
	fFrustum[LEFT_PLANE][0] = fClipping[ 3] + fClipping[ 0];
	fFrustum[LEFT_PLANE][1] = fClipping[ 7] + fClipping[ 4];
	fFrustum[LEFT_PLANE][2] = fClipping[11] + fClipping[ 8];
	fFrustum[LEFT_PLANE][3] = fClipping[15] + fClipping[12];
	normalize(LEFT_PLANE);
	/*
	 * Bottom plane
	 */
	fFrustum[BOTTOM_PLANE][0] = fClipping[ 3] + fClipping[ 1];
	fFrustum[BOTTOM_PLANE][1] = fClipping[ 7] + fClipping[ 5];
	fFrustum[BOTTOM_PLANE][2] = fClipping[11] + fClipping[ 9];
	fFrustum[BOTTOM_PLANE][3] = fClipping[15] + fClipping[13];
	normalize(BOTTOM_PLANE);
	/*
	 * Top plane
	 */
	fFrustum[TOP_PLANE][0] = fClipping[ 3] - fClipping[ 1];
	fFrustum[TOP_PLANE][1] = fClipping[ 7] - fClipping[ 5];
	fFrustum[TOP_PLANE][2] = fClipping[11] - fClipping[ 9];
	fFrustum[TOP_PLANE][3] = fClipping[15] - fClipping[13];
	normalize(TOP_PLANE);
	/*
	 * Far plane
	 */
	fFrustum[FAR_PLANE][0] = fClipping[ 3] - fClipping[ 2];
	fFrustum[FAR_PLANE][1] = fClipping[ 7] - fClipping[ 6];
	fFrustum[FAR_PLANE][2] = fClipping[11] - fClipping[10];
	fFrustum[FAR_PLANE][3] = fClipping[15] - fClipping[14];
	normalize(FAR_PLANE);
	/*
	 * Near plane
	 */
	fFrustum[NEAR_PLANE][0] = fClipping[ 3] + fClipping[ 2];
	fFrustum[NEAR_PLANE][1] = fClipping[ 7] + fClipping[ 6];
	fFrustum[NEAR_PLANE][2] = fClipping[11] + fClipping[10];
	fFrustum[NEAR_PLANE][3] = fClipping[15] + fClipping[14];
	normalize(NEAR_PLANE);

	delete[] fClipping;
} // end updateFrustum()
