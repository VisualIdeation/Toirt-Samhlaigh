/*
 * Quadrilateral.cpp - Methods for Quadrilateral class.
 *
 * Author: Patrick O'Leary
 * Created on: August 20, 2008
 * Copyright: 2008. All rights reserved.
 */

#include <DATASTRUCTURE/Quadrilateral.h>

Quadrilateral::Quadrilateral(void) :
    ax(0), ay(0), bx(0), by(0), cx(0), cy(0), dx(0), dy(0), mx(0), my(0) {
    rgbaColor = new RGBAColor();
}

Quadrilateral::Quadrilateral(float x, float y) :
    ax(x), ay(y), bx(x), by(y), cx(x), cy(y), dx(x), dy(y), mx(x), my(y) {
    rgbaColor = new RGBAColor();
}

Quadrilateral::Quadrilateral(float x, float y, float * values) :
    ax(x), ay(y), bx(x), by(y), cx(x), cy(y), dx(x), dy(y), mx(x), my(y) {
    rgbaColor = new RGBAColor(values[0], values[1], values[2], values[3]);
}

Quadrilateral::~Quadrilateral() {
    delete rgbaColor;
}

void Quadrilateral::set(float x, float y, float * values) {
    setM(x, y);
    setColor(values);
}

float Quadrilateral::getAx(void) const {
    return ax;
}

float Quadrilateral::getAy(void) const {
    return ay;
}

void Quadrilateral::setA(float x, float y) {
    this->ax = x;
    this->ay = y;
}

float Quadrilateral::getBx() const {
    return bx;
}

float Quadrilateral::getBy() const {
    return by;
}

void Quadrilateral::setB(float x, float y) {
    this->bx = x;
    this->by = y;
}

float * Quadrilateral::getColor(void) const {
    return rgbaColor->getColor();
}

void Quadrilateral::setColor(float * values) {
    rgbaColor->setValues(values);
}

float Quadrilateral::getCx() const {
    return cx;
}

float Quadrilateral::getCy() const {
    return cy;
}

void Quadrilateral::setC(float x, float y) {
    this->cx = x;
    this->cy = y;
}

float Quadrilateral::getDx() const {
    return dx;
}

float Quadrilateral::getDy() const {
    return dy;
}

void Quadrilateral::setD(float x, float y) {
    this->dx = x;
    this->dy = y;
}

float Quadrilateral::getMx(void) const {
    return mx;
}
float Quadrilateral::getMy(void) const {
    return my;
}

void Quadrilateral::setM(float x, float y) {
    float deltaX = x - this->mx;
    float deltaY = y - this->my;
    this->ax += deltaX;
    this->ay += deltaY;
    this->bx += deltaX;
    this->by += deltaY;
    this->cx += deltaX;
    this->cy += deltaY;
    this->dx += deltaX;
    this->dy += deltaY;
    this->mx = x;
    this->my = y;
}

float Quadrilateral::getMaximumX(void) {
    float maximum = ax;
    if (bx > maximum)
        maximum = bx;
    if (cx > maximum)
        maximum = cx;
    if (dx > maximum)
        maximum = dx;
    return maximum;
}

float Quadrilateral::getMaximumY(void) {
    float maximum = ay;
    if (by > maximum)
        maximum = by;
    if (cy > maximum)
        maximum = cy;
    if (dy > maximum)
        maximum = dy;
    return maximum;
}

float Quadrilateral::getMinimumX(void) {
    float minimum = ax;
    if (bx < minimum)
        minimum = bx;
    if (cx < minimum)
        minimum = cx;
    if (dx < minimum)
        minimum = dx;
    return minimum;
}

float Quadrilateral::getMinimumY(void) {
    float minimum = ay;
    if (by < minimum)
        minimum = by;
    if (cy < minimum)
        minimum = cy;
    if (dy < minimum)
        minimum = dy;
    return minimum;
}
