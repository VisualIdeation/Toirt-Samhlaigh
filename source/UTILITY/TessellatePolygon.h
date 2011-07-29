/*
 * TessellatePolygon.h
 *
 * Author: Patrick O'Leary
 * Created: September 24, 2008
 * Copyright: 2008. All rights reserved.
 */

#include <GL/gl.h>
#include <GL/glu.h>

#ifndef TESSELLATEPOLYGON_H_
#define TESSELLATEPOLYGON_H_

void combineCallback(GLdouble coordinates[3], GLdouble *vertex_data[4], GLfloat weight[4], GLdouble **dataOut);
void vertexCallback(GLvoid * vertex);

class TessellatePolygon {
public:
    TessellatePolygon();
    ~TessellatePolygon();
    void initialize(void);
    void setWindingRule(GLenum winding_rule);
    void render(GLdouble objectData[][3], int numberOfVertices);
    void beginPolygon(void);
    void endPolygon(void);
    void beginContour(void);
    void endContour(void);
    void end(void);
private:
    GLUtesselator * tessellationObject;
};

#endif /* TESSELLATEPOLYGON_H_ */
