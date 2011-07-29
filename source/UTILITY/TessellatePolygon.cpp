/*
 * TessellatePolygon.cpp
 *
 * Author: Patrick O'Leary
 * Created: September 24, 2008
 * Copyright: 2008. All rights reserved.
 */

#include <UTILITY/TessellatePolygon.h>

/*
 * TessellatePolygon - Constructor for TessellatePolygon class.
 */
TessellatePolygon::TessellatePolygon() {
} // end TessellatePolygon()

/*
 * ~TessellatePolygon - Destructor for TessellatePolygon class.
 */
TessellatePolygon::~TessellatePolygon() {
} // end TessellatePolygon()

/*
 * initialize
 */
void TessellatePolygon::initialize(void) {
    tessellationObject = gluNewTess();
    gluTessCallback(tessellationObject, GLU_TESS_VERTEX, (GLvoid (*) ()) &vertexCallback);
    gluTessCallback(tessellationObject, GLU_TESS_BEGIN, (GLvoid(*)()) &glBegin);
    gluTessCallback(tessellationObject, GLU_TESS_END, (GLvoid(*)()) &glEnd);
    gluTessCallback(tessellationObject, GLU_TESS_COMBINE, (GLvoid(*)()) &combineCallback);
} // end initialize()

/*
 * setWindingRule
 *
 * parameter windingRule - GLenum
 */
void TessellatePolygon::setWindingRule(GLenum windingRule) {
    gluTessProperty(tessellationObject, GLU_TESS_WINDING_RULE, windingRule);
} // end setWindingRule()

/*
 * render
 *
 * parameter objectData - GLdouble[][3]
 * parameter numberOfVertices - int
 */
void TessellatePolygon::render(GLdouble objectData[][3], int numberOfVertices) {
    for (int i = 0; i < numberOfVertices; i++) {
        gluTessVertex(tessellationObject, objectData[i], objectData[i]);
    }
}

/*
 * beginPolygon
 */
void TessellatePolygon::beginPolygon(void) {
    gluTessBeginPolygon(tessellationObject, NULL);
} // end beginPolygon()

/*
 * endPolygon
 */
void TessellatePolygon::endPolygon(void) {
    gluTessEndPolygon(tessellationObject);
} // end endPolygon()

/*
 * beginContour
 */
void TessellatePolygon::beginContour(void) {
    gluTessBeginContour(tessellationObject);
} // end beginContour()

/*
 * endContour
 */
void TessellatePolygon::endContour(void) {
    gluTessEndContour(tessellationObject);
} // end endContour()

/*
 * end
 */
void TessellatePolygon::end(void) {
    gluDeleteTess(tessellationObject);
} // end end()

/*
 * combineCallback
 *
 * parameter coordinates - GLdouble[3]
 * parameter vertexData[4] - GLdouble *
 * parameter weight - GLfloat[4]
 * parameter dataOut - GLdouble **
 */
void combineCallback(GLdouble coordinates[3], GLdouble *vertex_data[4], GLfloat weight[4], GLdouble **dataOut) {
    GLdouble vertex[3];
    vertex[0] = coordinates[0];
    vertex[1] = coordinates[1];
    vertex[2] = coordinates[2];
    *dataOut = vertex;
}

/*
 * vertexCallback
 *
 * parameter vertex - GLvoid *
 */
void vertexCallback(GLvoid * vertex) {
    GLdouble * pointer = (GLdouble *) vertex;
    glVertex3dv((GLdouble *) pointer);
} // end vertexCallback()
