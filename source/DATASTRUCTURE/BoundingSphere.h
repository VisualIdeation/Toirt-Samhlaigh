/*
 * BoundingSphere.h
 *
 * Author: Patrick O'Leary
 * Created: January 24, 2008
 * Copyright 2008. All rights reserved.
 */
#ifndef BOUNDINGSPHERE_H_
#define BOUNDINGSPHERE_H_

class BoundingSphere {
public:
	BoundingSphere();
	BoundingSphere(float x, float y, float z, float _radius);
	BoundingSphere(float* _center, float _radius);
	~BoundingSphere();
	float* getCenter(void);
	void setCenter(float x, float y, float z);
	void setCenter(float* _center);
	float getRadius(void);
	void setRadius(float _radius);
private:
	float* center;
	float radius;
};

#endif /*BOUNDINGSPHERE_H_*/
