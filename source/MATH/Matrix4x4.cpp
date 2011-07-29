/*
 * Matrix4x4.cpp - Methods for Matrix4x4 class.
 * 
 * Author: Patrick O'Leary
 * Created: November 12, 2007
 * Copyright 2007. All rights reserved.
 */
#include <MATH/Matrix4x4.h>

/*
 * Matrix4x4 - Constructor for Matrix4x4.
 */
Matrix4x4::Matrix4x4(void) {
	data = new float[16];
	set(0, 0, 1);
	set(0, 1, 0);
	set(0, 2, 0);
	set(0, 3, 0);
	set(1, 0, 0);
	set(1, 1, 1);
	set(1, 2, 0);
	set(1, 3, 0);
	set(2, 0, 0);
	set(2, 1, 0);
	set(2, 2, 1);
	set(2, 3, 0);
	set(3, 0, 0);
	set(3, 1, 0);
	set(3, 2, 0);
	set(3, 3, 1);
} // end Matrix4x4()

/*
 * Matrix4x4 - Constructor for Matrix4x4.
 * 
 * parameter data - float*
 */
Matrix4x4::Matrix4x4(float* _data) {
	data = new float[16];
	for (int i = 0; i < 16; i++) {
		data[i] = _data[i];
	} // end for
} // end Matrix4x4()

/*
 * Matrix4x4 - Constructor for Matrix4x4.
 *
 * parameter x0 - float
 * parameter y0 - float
 * parameter z0 - float
 * parameter w0 - float
 * parameter x1 - float
 * parameter y1 - float
 * parameter z1 - float
 * parameter w1 - float
 * parameter x2 - float
 * parameter y2 - float
 * parameter z2 - float
 * parameter w2 - float
 * parameter x3 - float
 * parameter y3 - float
 * parameter z3 - float
 * parameter w3 - float
 */
Matrix4x4::Matrix4x4(float x0, float y0, float z0, float w0, float x1, float y1, float z1, float w1, float x2, float y2, float z2,
		float w2, float x3, float y3, float z3, float w3) {
	data = new float[16];
	set(0, 0, x0);
	set(0, 1, y0);
	set(0, 2, z0);
	set(0, 3, w0);
	set(1, 0, x1);
	set(1, 1, y1);
	set(1, 2, z1);
	set(1, 3, w1);
	set(2, 0, x2);
	set(2, 1, y2);
	set(2, 2, z2);
	set(2, 3, w2);
	set(3, 0, x3);
	set(3, 1, y3);
	set(3, 2, z3);
	set(3, 3, w3);
} // end Matrix4x4()

/*
 * Matrix4x4 - Constructor for Matrix4x4.
 *
 * parameter x - Vector4
 * parameter y - Vector4
 * parameter z - Vector4
 * parameter w - Vector4
 */
Matrix4x4::Matrix4x4(Vector4 x, Vector4 y, Vector4 z, Vector4 w) {
	data = new float[16];
	set(0, 0, x.getX());
	set(0, 1, x.getY());
	set(0, 2, x.getZ());
	set(0, 3, x.getW());
	set(1, 0, y.getX());
	set(1, 1, y.getY());
	set(1, 2, y.getZ());
	set(1, 3, y.getW());
	set(2, 0, z.getX());
	set(2, 1, z.getY());
	set(2, 2, z.getZ());
	set(2, 3, z.getW());
	set(3, 0, w.getX());
	set(3, 1, w.getY());
	set(3, 2, w.getZ());
	set(3, 3, w.getW());
} // end Matrix4x4()

/*
 * ~Matrix4x4 - Destructor for Matrix4x4.
 */
Matrix4x4::~Matrix4x4(void) {
	delete[] data;
} // end ~Matrix4x4()

/*
 * add
 *
 * parameter m - Matrix4x4
 */
void Matrix4x4::add(Matrix4x4 m) {
	set(0, 0, get(0, 0) + m.get(0, 0));
	set(0, 1, get(0, 1) + m.get(0, 1));
	set(0, 2, get(0, 2) + m.get(0, 2));
	set(0, 3, get(0, 3) + m.get(0, 3));
	set(1, 0, get(1, 0) + m.get(1, 0));
	set(1, 1, get(1, 1) + m.get(1, 1));
	set(1, 2, get(1, 2) + m.get(1, 2));
	set(1, 3, get(1, 3) + m.get(1, 3));
	set(2, 0, get(2, 0) + m.get(2, 0));
	set(2, 1, get(2, 1) + m.get(2, 1));
	set(2, 2, get(2, 2) + m.get(2, 2));
	set(2, 3, get(2, 3) + m.get(2, 3));
	set(3, 0, get(3, 0) + m.get(3, 0));
	set(3, 1, get(3, 1) + m.get(3, 1));
	set(3, 2, get(3, 2) + m.get(3, 2));
	set(3, 3, get(3, 3) + m.get(3, 3));
} // end add()

/*
 * adjoint
 *
 *         Let a   denote the minor determinant of matrix A obtained by
 *              ij
 *
 *         deleting the ith row and jth column from A. Let
 *
 *                   i+j
 *         b   = (-1)    a    The matrix B = (b  ) is the adjoint of A
 *          ij            ji                   ij
 *
 * return - Matrix4x4*
 */
Matrix4x4* Matrix4x4::adjoint(void) {
	Matrix4x4* out = new Matrix4x4();
	float a1;
	float a2;
	float a3;
	float a4;
	float b1;
	float b2;
	float b3;
	float b4;
	float c1;
	float c2;
	float c3;
	float c4;
	float d1;
	float d2;
	float d3;
	float d4;
	a1 = get(0, 0);
	b1 = get(0, 1);
	c1 = get(0, 2);
	d1 = get(0, 3);
	a2 = get(1, 0);
	b2 = get(1, 1);
	c2 = get(1, 2);
	d2 = get(1, 3);
	a3 = get(2, 0);
	b3 = get(2, 1);
	c3 = get(2, 2);
	d3 = get(2, 3);
	a4 = get(3, 0);
	b4 = get(3, 1);
	c4 = get(3, 2);
	d4 = get(3, 3);
	out->set(0, 0, determinant3x3(b2, b3, b4, c2, c3, c4, d2, d3, d4));
	out->set(1, 0, -determinant3x3(a2, a3, a4, c2, c3, c4, d2, d3, d4));
	out->set(2, 0, determinant3x3(a2, a3, a4, b2, b3, b4, d2, d3, d4));
	out->set(3, 0, -determinant3x3(a2, a3, a4, b2, b3, b4, c2, c3, c4));
	out->set(0, 1, -determinant3x3(b1, b3, b4, c1, c3, c4, d1, d3, d4));
	out->set(1, 1, determinant3x3(a1, a3, a4, c1, c3, c4, d1, d3, d4));
	out->set(2, 1, -determinant3x3(a1, a3, a4, b1, b3, b4, d1, d3, d4));
	out->set(3, 1, determinant3x3(a1, a3, a4, b1, b3, b4, c1, c3, c4));
	out->set(0, 2, determinant3x3(b1, b2, b4, c1, c2, c4, d1, d2, d4));
	out->set(1, 2, -determinant3x3(a1, a2, a4, c1, c2, c4, d1, d2, d4));
	out->set(2, 2, determinant3x3(a1, a2, a4, b1, b2, b4, d1, d2, d4));
	out->set(3, 2, -determinant3x3(a1, a2, a4, b1, b2, b4, c1, c2, c4));
	out->set(0, 3, -determinant3x3(b1, b2, b3, c1, c2, c3, d1, d2, d3));
	out->set(1, 3, determinant3x3(a1, a2, a3, c1, c2, c3, d1, d2, d3));
	out->set(2, 3, -determinant3x3(a1, a2, a3, b1, b2, b3, d1, d2, d3));
	out->set(3, 3, determinant3x3(a1, a2, a3, b1, b2, b3, c1, c2, c3));
	return out;
} // end adjoint()

/*
 * determinant
 *
 * return - float
 */
float Matrix4x4::determinant(void) {
	float a1;
	float a2;
	float a3;
	float a4;
	float b1;
	float b2;
	float b3;
	float b4;
	float c1;
	float c2;
	float c3;
	float c4;
	float d1;
	float d2;
	float d3;
	float d4;
	a1 = get(0, 0);
	b1 = get(0, 1);
	c1 = get(0, 2);
	d1 = get(0, 3);
	a2 = get(1, 0);
	b2 = get(1, 1);
	c2 = get(1, 2);
	d2 = get(1, 3);
	a3 = get(2, 0);
	b3 = get(2, 1);
	c3 = get(2, 2);
	d3 = get(2, 3);
	a4 = get(3, 0);
	b4 = get(3, 1);
	c4 = get(3, 2);
	d4 = get(3, 3);

	return (((a1 * determinant3x3(b2, b3, b4, c2, c3, c4, d2, d3, d4)) - (b1 * determinant3x3(a2, a3, a4, c2, c3, c4, d2, d3, d4))
			+ (c1 * determinant3x3(a2, a3, a4, b2, b3, b4, d2, d3, d4)))
			- (d1 * determinant3x3(a2, a3, a4, b2, b3, b4, c2, c3, c4)));
} // end determinant()

/*
 * determinant3x3
 *
 * parameter a1 - float
 * parameter a2 - float
 * parameter a3 - float
 * parameter b1 - float
 * parameter b2 - float
 * parameter b3 - float
 * parameter c1 - float
 * parameter c2 - float
 * parameter c3 - float
 * return - float
 */
float Matrix4x4::determinant3x3(float a1, float a2, float a3, float b1, float b2, float b3, float c1, float c2, float c3) {
	return ((a1 * ((b2 * c3) - (b3 * c2))) - (b1 * ((a2 * c3) - (a3 * c2))) + (c1 * ((a2 * b3) - (a3 * b2))));
} // end determinant3x3()

/*
 * get
 *
 * parameter i - int
 * parameter j - int
 * return - float
 */
float Matrix4x4::get(int i, int j) const {
	return data[(i * 4) + j];
} // end get()

/*
 * set
 *
 * parameter i - int
 * parameter j - int
 * parameter value - float
 */
void Matrix4x4::set(int i, int j, float value) {
	data[(i * 4) + j] = value;
} // end set()

/*
 * getData
 *
 * return - float*
 */
float* Matrix4x4::getData(void) {
	return data;
} // end getData()

/*
 * inverse
 *
 *             -1     1
 *             A  = _____ adjoint A
 *                  det A
 *
 * return - Matrix4x4*
 */
Matrix4x4* Matrix4x4::inverse(void) {
	float det = 0.0f;
	det = determinant();

	if (det == 0.0f) {
		return new Matrix4x4();
	} // end if

	Matrix4x4* out = adjoint();

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			out->set(i, j, out->get(i, j) / det);
		} // end for
	} // end for

	return out;
} // end inverse()

/*
 * multiply
 *
 * parameter scalar - float
 */
void Matrix4x4::multiply(float scalar) {
	set(0, 0, get(0, 0) * scalar);
	set(0, 1, get(0, 1) * scalar);
	set(0, 2, get(0, 2) * scalar);
	set(0, 3, get(0, 3) * scalar);
	set(1, 0, get(1, 0) * scalar);
	set(1, 1, get(1, 1) * scalar);
	set(1, 2, get(1, 2) * scalar);
	set(1, 3, get(1, 3) * scalar);
	set(2, 0, get(2, 0) * scalar);
	set(2, 1, get(2, 1) * scalar);
	set(2, 2, get(2, 2) * scalar);
	set(2, 3, get(2, 3) * scalar);
	set(3, 0, get(3, 0) * scalar);
	set(3, 1, get(3, 1) * scalar);
	set(3, 2, get(3, 2) * scalar);
	set(3, 3, get(3, 3) * scalar);
} // end multiply()

/*
 * multiply
 *
 * parameter m - Matrix4x4
 */
void Matrix4x4::multiply(Matrix4x4* m) {
	Matrix4x4* m1 = new Matrix4x4(data);
	set(0, 0, ((m1->get(0, 0) * m->get(0, 0)) + (m1->get(0, 1) * m->get(1, 0)) + (m1->get(0, 2) * m->get(2, 0)) + (m1->get(0, 3) * m->get(3, 0))));
	set(0, 1, ((m1->get(0, 0) * m->get(0, 1)) + (m1->get(0, 1) * m->get(1, 1)) + (m1->get(0, 2) * m->get(2, 1)) + (m1->get(0, 3) * m->get(3, 1))));
	set(0, 2, ((m1->get(0, 0) * m->get(0, 2)) + (m1->get(0, 1) * m->get(1, 2)) + (m1->get(0, 2) * m->get(2, 2)) + (m1->get(0, 3) * m->get(3, 2))));
	set(0, 3, ((m1->get(0, 0) * m->get(0, 3)) + (m1->get(0, 1) * m->get(1, 3)) + (m1->get(0, 2) * m->get(2, 3)) + (m1->get(0, 3) * m->get(3, 3))));
	set(1, 0, ((m1->get(1, 0) * m->get(0, 0)) + (m1->get(1, 1) * m->get(1, 0)) + (m1->get(1, 2) * m->get(2, 0)) + (m1->get(1, 3) * m->get(3, 0))));
	set(1, 1, ((m1->get(1, 0) * m->get(0, 1)) + (m1->get(1, 1) * m->get(1, 1)) + (m1->get(1, 2) * m->get(2, 1)) + (m1->get(1, 3) * m->get(3, 1))));
	set(1, 2, ((m1->get(1, 0) * m->get(0, 2)) + (m1->get(1, 1) * m->get(1, 2)) + (m1->get(1, 2) * m->get(2, 2)) + (m1->get(1, 3) * m->get(3, 2))));
	set(1, 3, ((m1->get(1, 0) * m->get(0, 3)) + (m1->get(1, 1) * m->get(1, 3)) + (m1->get(1, 2) * m->get(2, 3)) + (m1->get(1, 3) * m->get(3, 3))));
	set(2, 0, ((m1->get(2, 0) * m->get(0, 0)) + (m1->get(2, 1) * m->get(1, 0)) + (m1->get(2, 2) * m->get(2, 0)) + (m1->get(2, 3) * m->get(3, 0))));
	set(2, 1, ((m1->get(2, 0) * m->get(0, 1)) + (m1->get(2, 1) * m->get(1, 1)) + (m1->get(2, 2) * m->get(2, 1)) + (m1->get(2, 3) * m->get(3, 1))));
	set(2, 2, ((m1->get(2, 0) * m->get(0, 2)) + (m1->get(2, 1) * m->get(1, 2)) + (m1->get(2, 2) * m->get(2, 2)) + (m1->get(2, 3) * m->get(3, 2))));
	set(2, 3, ((m1->get(2, 0) * m->get(0, 3)) + (m1->get(2, 1) * m->get(1, 3)) + (m1->get(2, 2) * m->get(2, 3)) + (m1->get(2, 3) * m->get(3, 3))));
	set(3, 0, ((m1->get(3, 0) * m->get(0, 0)) + (m1->get(3, 1) * m->get(1, 0)) + (m1->get(3, 2) * m->get(2, 0)) + (m1->get(3, 3) * m->get(3, 0))));
	set(3, 1, ((m1->get(3, 0) * m->get(0, 1)) + (m1->get(3, 1) * m->get(1, 1)) + (m1->get(3, 2) * m->get(2, 1)) + (m1->get(3, 3) * m->get(3, 1))));
	set(3, 2, ((m1->get(3, 0) * m->get(0, 2)) + (m1->get(3, 1) * m->get(1, 2)) + (m1->get(3, 2) * m->get(2, 2)) + (m1->get(3, 3) * m->get(3, 2))));
	set(3, 3, ((m1->get(3, 0) * m->get(0, 3)) + (m1->get(3, 1) * m->get(1, 3)) + (m1->get(3, 2) * m->get(2, 3)) + (m1->get(3, 3) * m->get(3, 3))));
	delete m1;
} // end multiply()

/*
 * toString
 * 
 * return - std::string
 */
std::string Matrix4x4::toString(void) {
	return "" + Stringify::toString(get(0, 0)) + " " + Stringify::toString(get(0, 1)) + " " + Stringify::toString(get(0, 2)) + " "
			+ Stringify::toString(get(0, 3)) + "\n" + Stringify::toString(get(1, 0)) + " " + Stringify::toString(get(1, 1)) + " "
			+ Stringify::toString(get(1, 2)) + " " + Stringify::toString(get(1, 3)) + "\n" + Stringify::toString(get(2, 0)) + " "
			+ Stringify::toString(get(2, 1)) + " " + Stringify::toString(get(2, 2)) + " " + Stringify::toString(get(2, 3)) + "\n"
			+ Stringify::toString(get(3, 0)) + " " + Stringify::toString(get(3, 1)) + " " + Stringify::toString(get(3, 2)) + " "
			+ Stringify::toString(get(3, 3));
} // end toString()

/*
 * transpose
 */
void Matrix4x4::transpose(void) {
	float temp;

	for (int i = 0; i < 4; i++) {
		for (int j = i; j < 4; j++) {
			temp = get(i, j);
			set(i, j, get(j, i));
			set(j, i, temp);
		} // end for
	} // end for
} // end transpose()
