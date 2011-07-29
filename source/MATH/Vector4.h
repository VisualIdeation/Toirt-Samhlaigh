/*
 * Vector4
 *
 * Author: Patrick O'Leary
 * Created November 4, 2007
 * Copyright 2007. All rights reserved.
 */
#ifndef VECTOR4_H_
#define VECTOR4_H_

#include <string>

#include <MATH/Point4.h>
#include <UTILITY/Stringify.h>

class Vector4 {
public:
	Vector4(void);
	Vector4(float* _v);
	Vector4(float _x, float _y, float _z, float _w);
	Vector4(Point4 p);
	Vector4(Point4* p);
	Vector4(Point4 p1, Point4 p2);
	~Vector4(void);
	void add(float scalar);
	void add(Vector4 v);
	void add(Vector4* v);
	/*
	 * add
	 *
	 * parameter v - Vector4
	 * parameter scalar - float
	 * return - Vector4*
	 */
	static Vector4* add(Vector4 v, float scalar) {
		return new Vector4(v.getX() + scalar, v.getY() + scalar,
				v.getZ() + scalar, v.getW() + scalar);
	} // end add()
	;
	/*
	 * add
	 *
	 * parameter scalar - float
	 * parameter v - Vector4
	 * return - Vector4*
	 */
	static Vector4* add(float scalar, Vector4 v) {
		return new Vector4(v.getX() + scalar, v.getY() + scalar,
				v.getZ() + scalar, v.getW() + scalar);
	} // end add()
	;
	/*
	 * add
	 *
	 * parameter p - Point4
	 * parameter v - Vector4
	 * return - Point4*
	 */
	static Point4* add(Point4 p, Vector4 v) {
		return new Point4(p.getX() + (double) v.getX(),
				p.getY() + (double) v.getY(),
				p.getZ() + (double) v.getZ(),
				p.getW() + (double) v.getW());
	} // end add()
	;
	/*
	 * add
	 *
	 * parameter v1 - Vector4
	 * parameter v2 - Vector4
	 * return - Vector4*
	 */
	static Vector4* add(Vector4 v1, Vector4 v2) {
		return new Vector4(v1.getX() + v2.getX(), v1.getY() + v2.getY(),
				v1.getZ() + v2.getZ(), v1.getW() + v2.getW());
	} // end add()
	;
	/*
	 * add
	 *
	 * parameter v1 - Vector4*
	 * parameter v2 - Vector4*
	 * return - Vector4*
	 */
	static Vector4* add(Vector4* v1, Vector4* v2) {
		return new Vector4(v1->getX() + v2->getX(), v1->getY() + v2->getY(),
				v1->getZ() + v2->getZ(), v1->getW() + v2->getW());
	} // end add()
	;
	void clear(void);
	float cosAngle(Vector4 v);
	Vector4* cross(Vector4* v);
	void divide(float scalar);
	/*
	 * divide
	 *
	 * parameter v - Vector4x, float _y, float _z, float _w
	 * parameter scalar - float
	 * return - Vector4*
	 */
	static Vector4* divide(Vector4 v, float scalar) {
		float _x;
		float _y;
		float _z;
		float _w;
		if (scalar == 0.0f) {
			_x = 0.0f;
			_y = 0.0f;
			_z = 0.0f;
			_w = 0.0f;
		} // end if
		else {
			_x = v.getX() / scalar;
			_y = v.getY() / scalar;
			_z = v.getZ() / scalar;
			_w = v.getW() / scalar;
		} // end else
		return new Vector4(_x, _y, _z, _w);
	} // end divide()
	;
	float dot(Vector4 v);
	float dot(Vector4* v);
	float dot(const Vector4* v);
	float getW(void);
	float getW(void) const;
	void setW(float _w);
	float getX(void);
	float getX(void) const;
	void setX(float _x);
	void setXYZW(float _x, float _y, float _z, float _w);
	float getY(void);
	float getY(void) const;
	void setY(float _y);
	float getZ(void);
	float getZ(void) const;
	void setZ(float _z);
	float magnitude(void);
	float magnitudeSquared(void);
	void multiply(float scalar);
	/*
	 * multiply
	 *
	 * parameter scalar - float
	 * parameter v - Vector4
	 * return - Vector4*
	 */
	static Vector4* multiply(float scalar, Vector4 v) {
		return new Vector4(v.getX() * scalar, v.getY() * scalar,
				v.getZ() * scalar, v.getW() * scalar);
	} // end multiply()
	;
	/*
	 * multiply
	 *
	 * parameter v - Vector4
	 * parameter scalar - float
	 * return - Vector4*
	 */
	static Vector4* multiply(Vector4 v, float scalar) {
		return new Vector4(v.getX() * scalar, v.getY() * scalar,
				v.getZ() * scalar, v.getW() * scalar);
	} // end multiply()
	;
	/*
	 * multiply
	 *
	 * parameter v - Vector4*
	 * parameter scalar - float
	 * return - Vector4*
	 */
	static Vector4* multiply(Vector4* v, float scalar) {
		return new Vector4(v->getX() * scalar, v->getY() * scalar,
				v->getZ() * scalar, v->getW() * scalar);
	} // end multiply()
	;
	/*
	 * multiply
	 *
	 * parameter v - const Vector4*
	 * parameter scalar - float
	 * return - Vector4*
	 */
	static Vector4* multiply(const Vector4* v, float scalar) {
		return new Vector4(v->getX() * scalar, v->getY() * scalar,
				v->getZ() * scalar, v->getW() * scalar);
	} // end multiply()
	;
	void negate(void);
	void subtract(float scalar);
	void subtract(Vector4 v);
	void subtract(Vector4* v);
	/*
	 * subtract
	 *
	 * parameter scalar - float
	 * parameter v - Vector4
	 * return - Vector4*
	 */
	static Vector4* subtract(float scalar, Vector4 v) {
		return new Vector4(scalar - v.getX(), scalar - v.getY(),
				scalar - v.getZ(), scalar - v.getW());
	} // end subtract()
	;
	/*
	 * subtract
	 *
	 * parameter v - Vector4
	 * parameter scalar - float
	 * return - Vector4*
	 */
	static Vector4* subtract(Vector4 v, float scalar) {
		return new Vector4(v.getX() - scalar, v.getY() - scalar,
				v.getZ() - scalar, v.getW() - scalar);
	} // end subtract()
	;
	/*
	 * subtract
	 *
	 * parameter v1 - Vector4
	 * parameter v2 - Vector4
	 * return - Vector4*
	 */
	static Vector4* subtract(Vector4 v1, Vector4 v2) {
		return new Vector4(v1.getX() - v2.getX(), v1.getY() - v2.getY(),
				v1.getZ() - v2.getZ(), v1.getW() - v2.getW());
	} // end subtract()
	;
	/*
	 * subtract
	 *
	 * parameter v1 - Vector4*
	 * parameter v2 - float*
	 * return - Vector4*
	 */
	static Vector4* subtract(Vector4* v1, float* v2) {
		return new Vector4(v1->getX() - v2[0], v1->getY() - v2[1],
				v1->getZ() - v2[2], v1->getW() - 1.0f);
	} // end subtract()
	;
	/*
	 * subtract
	 *
	 * parameter v1 - Vector4*
	 * parameter v2 - float*
	 * return - Vector4*
	 */
	static Vector4* subtract(const Vector4* v1, float* v2) {
		return new Vector4(v1->getX() - v2[0], v1->getY() - v2[1],
				v1->getZ() - v2[2], v1->getW() - 1.0f);
	} // end subtract()
	;
	/*
	 * subtract
	 *
	 * parameter v1 - float*
	 * parameter v2 - float*
	 * return - Vector4*
	 */
	static Vector4* subtract(float* v1, float* v2) {
		return new Vector4(v1[0] - v2[0], v1[1] - v2[1],
				v1[2] - v2[2], 0.0f);
	} // end subtract()
	;
	/*
	 * subtract
	 *
	 * parameter p1 - Point4
	 * parameter p2 - Point4
	 * return - Vector4*
	 */
	static Vector4* subtract(Point4 p1, Point4 p2) {
		return new Vector4((float) (p1.getX() - p2.getX()),
				(float) (p1.getY() - p2.getY()),
				(float) (p1.getZ() - p2.getZ()),
				(float) (p1.getW() - p2.getW()));
	} // end subtract()
	;
	/*
	 * subtract
	 *
	 * parameter p1 - Point4*
	 * parameter p2 - Point4*
	 * return - Vector4*
	 */
	static Vector4* subtract(Point4* p1, Point4* p2) {
		return new Vector4((float) (p1->getX() - p2->getX()),
				(float) (p1->getY() - p2->getY()),
				(float) (p1->getZ() - p2->getZ()),
				(float) (p1->getW() - p2->getW()));
	} // end subtract()
	;
	/*
	 * subtract
	 *
	 * parameter v - Vector4
	 * parameter p - Point4
	 * return - Vector4*
	 */
	static Vector4* subtract(Vector4 v, Point4 p) {
		return new Vector4((float) (v.getX() - p.getX()),
				(float) (v.getY() - p.getY()),
				(float) (v.getZ() - p.getZ()),
				(float) (v.getW() - p.getW()));
	} // end subtract()
	;

	/*
	 * subtract
	 *
	 * parameter v - const Vector4*
	 * parameter p - Point4*
	 * return - Vector4*
	 */
	static Vector4* subtract(const Vector4* v, Point4* p) {
		return new Vector4((float) (v->getX() - p->getX()),
				(float) (v->getY() - p->getY()),
				(float) (v->getZ() - p->getZ()),
				(float) (v->getW() - p->getW()));
	} // end subtract()
	;
	std::string toString(void);
	Vector4* unit(void);
	void unitize(void);
private:
	float w;
	float x;
	float y;
	float z;
};

#endif /*VECTOR4_H_*/
