/*
 * Point4 - quadruple of doubles (x,y,z,w)
 *
 * Author: Patrick O'Leary
 * Created: November 4, 2007
 * Copyright 2007. All rights reserved.
 */
#ifndef POINT4_H_
#define POINT4_H_

#include <string>

class Point4 {
public:
	Point4(void);
	Point4(double _x, double _y, double _z);
	Point4(double _x, double _y, double _z, double _w);
	~Point4(void);
	double distance(Point4* p);
	double distance(Point4 p);
	double get(int dimension);
	void setPoint4(Point4* p);
	void setPoint4(Point4 p);
	double getW(void);
	void setW(double _w);
	double getX(void);
	void setX(double _x);
	void setXYZ(double _x, double _y, double _z);
	double getY(void);
	void setY(double _y);
	double getZ(void);
	void setZ(double _z);
	Point4* midPoint(Point4* p);
	void normalize(void);
	std::string toString(void);
	Point4* weightedMidPoint(Point4* p, double xWeight, double yWeight, double zWeight, double wWeight);
private:
	double w;
	double x;
	double y;
	double z;
};

#endif /*POINT4_H_*/
