/*
 * Quadrilateral.h - Class for creating a quadrilateral.
 *
 * Author: Patrick O'Leary
 * Created on: August 20, 2008
 * Copyright: 2008. All rights reserved.
 */

#ifndef QUADRILATERAL_H_
#define QUADRILATERAL_H_

#include <COLORMAP/RGBAColor.h>

class Quadrilateral {
public:
    Quadrilateral(void);
    Quadrilateral(float x, float y);
    Quadrilateral(float x, float y, float * values);
    ~Quadrilateral(void);
    void set(float x, float y, float * values);
    float getAx(void) const;
    float getAy(void) const;
    void setA(float x, float y);
    float getBx(void) const;
    float getBy(void) const;
    void setB(float x, float y);
    float * getColor(void) const;
    void setColor(float * values);
    float getCx(void) const;
    float getCy(void) const;
    void setC(float x, float y);
    float getDx(void) const;
    float getDy(void) const;
    void setD(float x, float y);
    float getMaximumX(void);
    float getMaximumY(void);
    float getMinimumX(void);
    float getMinimumY(void);
    float getMx(void) const;
    float getMy(void) const;
    void setM(float x, float y);
private:
    float ax;
    float ay;
    float bx;
    float by;
    float cx;
    float cy;
    float dx;
    float dy;
    float mx;
    float my;
    RGBAColor * rgbaColor;
};

#endif /* QUADRILATERAL_H_ */
