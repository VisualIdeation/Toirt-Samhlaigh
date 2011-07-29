/*
 * Matrix4x4
 *
 * Author: Patrick O'Leary
 * Created: November 4, 2007
 * Copyright 2007. All rights reserved.
 */
#ifndef MATRIX4X4_H_
#define MATRIX4X4_H_

#include <string>

#include <MATH/Point4.h>
#include <MATH/Vector4.h>
#include <UTILITY/Stringify.h>

class Matrix4x4 {
public:
	Matrix4x4(void);
	Matrix4x4(float* _data);
	Matrix4x4(float x0, float y0, float z0, float w0, float x1, float y1, float z1, float w1, float x2, float y2, float z2,
			float w2, float x3, float y3, float z3, float w3);
	Matrix4x4(Vector4 x, Vector4 y, Vector4 z, Vector4 w);
	~Matrix4x4(void);
	void add(Matrix4x4 m);
	/*
	 * add
	 *
	 * parameter m1 - Matrix4x4
	 * parameter m2 - Matrix4x4
	 * return - Matrix4x4*
	 */
	static Matrix4x4* add(Matrix4x4 m1, Matrix4x4 m2) {
		Matrix4x4* m = new Matrix4x4(m1);
		m->add(m2);
		return m;
	} // end add()
	;
	Matrix4x4* adjoint(void);
	float determinant(void);
	float determinant3x3(float a1, float a2, float a3, float b1, float b2, float b3, float c1, float c2, float c3);
	float get(int i, int j) const;
	void set(int i, int j, float value);
	float* getData(void);
	Matrix4x4* inverse(void);
	void multiply(float scalar);
	void multiply(Matrix4x4* m);
	/*
	 * multiply
	 *
	 * parameter m1 - Matrix4x4
	 * parameter scalar - float
	 * return - Matrix4x4*
	 */
	static Matrix4x4* multiply(Matrix4x4 m1, float scalar) {
		Matrix4x4* m = new Matrix4x4(m1);
		m->multiply(scalar);
		return m;
	} // end multiply()
	;
	/*
	 * multiply
	 *
	 * parameter scalar - float
	 * parameter m1 - Matrix4x4
	 * return - Matrix4x4*
	 */
	static Matrix4x4* multiply(float scalar, Matrix4x4 m1) {
		Matrix4x4* m = new Matrix4x4(m1);
		m->multiply(scalar);
		return m;
	} // end multiply()
	;
	/*
	 * multiply
	 *
	 * parameter m - Matrix4x4
	 * parameter v - Vector4
	 * return - Vector4*
	 */
	static Vector4* multiply(Matrix4x4 m, Vector4 v) {
		return new Vector4((m.get(0, 0) * v.getX()) + (m.get(1, 0) * v.getY()) +
				(m.get(2, 0) * v.getZ()) + (m.get(3, 0) * v.getW()),
				(m.get(0, 1) * v.getX()) + (m.get(1, 1) * v.getY()) +
				(m.get(2, 1) * v.getZ()) + (m.get(3, 1) * v.getW()),
				(m.get(0, 2) * v.getX()) + (m.get(1, 2) * v.getY()) +
				(m.get(2, 2) * v.getZ()) + (m.get(3, 2) * v.getW()),
				(m.get(0, 3) * v.getX()) + (m.get(1, 3) * v.getY()) +
				(m.get(2, 3) * v.getZ()) + (m.get(3, 3) * v.getW()));
	} // end multiply()
	;
	/*
	 * multiply
	 *
	 * parameter m - Matrix4x4*
	 * parameter v - Vector4*
	 * return - Vector4*
	 */
	static Vector4* multiply(Matrix4x4* m, Vector4* v) {
		return new Vector4((m->get(0, 0) * v->getX()) + (m->get(1, 0) * v->getY()) +
				(m->get(2, 0) * v->getZ()) + (m->get(3, 0) * v->getW()),
				(m->get(0, 1) * v->getX()) + (m->get(1, 1) * v->getY()) +
				(m->get(2, 1) * v->getZ()) + (m->get(3, 1) * v->getW()),
				(m->get(0, 2) * v->getX()) + (m->get(1, 2) * v->getY()) +
				(m->get(2, 2) * v->getZ()) + (m->get(3, 2) * v->getW()),
				(m->get(0, 3) * v->getX()) + (m->get(1, 3) * v->getY()) +
				(m->get(2, 3) * v->getZ()) + (m->get(3, 3) * v->getW()));
	} // end multiply()
	;
	/*
	 * multiply
	 *
	 * parameter m - Matrix4x4
	 * parameter p - Point4
	 * return - Point4*
	 */
	static Point4* multiply(Matrix4x4 m, Point4 p) {
		return new Point4((m.get(0, 0) * p.getX()) + (m.get(1, 0) * p.getY()) +
				(m.get(2, 0) * p.getZ()) + (m.get(3, 0) * p.getW()),
				(m.get(0, 1) * p.getX()) + (m.get(1, 1) * p.getY()) +
				(m.get(2, 1) * p.getZ()) + (m.get(3, 1) * p.getW()),
				(m.get(0, 2) * p.getX()) + (m.get(1, 2) * p.getY()) +
				(m.get(2, 2) * p.getZ()) + (m.get(3, 2) * p.getW()),
				(m.get(0, 3) * p.getX()) + (m.get(1, 3) * p.getY()) +
				(m.get(2, 3) * p.getZ()) + (m.get(3, 3) * p.getW()));
	} // end multiply()
	;
	std::string toString(void);
	void transpose(void);
private:
	float* data; // Note: Column-major for OpenGL compatibility
};

#endif /*MATRIX4X4_H_*/
