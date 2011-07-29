/*
 * HistogramWidget.cpp - Methods for HistogramWidget class.
 *
 * Author: Patrick O'Leary
 * Created: August 18, 2008
 * Copyright: 2008. All rights reserved.
 */

#include <cfloat>
#include <cmath>

/* Vrui includes */
#include <GL/Extensions/GLARBTextureNonPowerOfTwo.h>
#include <GL/GLColorTemplates.h>
#include <GL/GLVertexTemplates.h>

#include <COLORMAP/RGBAColor.h>
#include <DATA/Volume.h>
#include <GUI/HistogramWidgetChangedCallbackData.h>
#include <GUI/HistogramWidget.h>
#include <GUI/QuadrilateralChangedCallbackData.h>
#include <UTILITY/Stringify.h>
#include <UTILITY/TessellatePolygon.h>

/*
 * DataItem - Constructor for structure DataItem
 */
HistogramWidget::DataItem::DataItem(void) {
} // end DataItem()

/*
 * ~DataItem - Destructor for structure DataItem
 */
HistogramWidget::DataItem::~DataItem(void) {
} // end ~DataItem()

/*
 * HistogramWidget - Constructor for the HistogramWidget class.
 *
 * parameter _name - const char *
 * parameter _parent - GLMotif::Container *
 * parameter volume - Volume *
 * parameter _manageChild - bool
 */
HistogramWidget::HistogramWidget(const char * _name, GLMotif::Container * _parent, Volume * _volume, bool _manageChild) :
    GLMotif::Widget(_name, _parent, false), controlPointSize(0), currentMode(modeNone), currentQuadrilateral(-1), directionChanged(
            false), dragging(false), numberOfQuadrilaterals(0), selected(false), sliceChanged(false), sliceDirection(XY), volume(
            _volume) {
    colormap = new float[256 * 256 * 4];
    currentColor = new RGBAColor(1.0f, 0.0f, 0.0f, 0.1f);
    marginWidth = 0.0f;
    preferredSize[0] = 0.0f;
    preferredSize[1] = 0.0f;
    preferredSize[2] = 0.0f;
    histogram = new unsigned char[256 * 256];
    updateHistogram(sliceDirection, volume->getDepth() / 2);
    if (_manageChild)
        manageChild();
} // end HistogramWidget()

/*
 * ~HistogramWidget - Destructor for the HistogramWidget class.
 */
HistogramWidget::~HistogramWidget(void) {
    delete[] colormap;
} // end ~HistogramWidget()

/*
 * addQuadrilateral
 *
 * parameter x - float
 * parameter y - float
 */
void HistogramWidget::addQuadrilateral(float x, float y) {
    quadrilaterals[numberOfQuadrilaterals++].set(x, y, currentColor->getValues());
} // end addQuadrilateral()

/*
 * calcNaturalSize - Determine the natural size of the HistogramWidget. A virtual function of GLMotif::Widget base.
 *
 * return - GLMotif::Vector
 */
GLMotif::Vector HistogramWidget::calcNaturalSize(void) const {
    GLMotif::Vector result = preferredSize;
    result[0] += 2.0f * marginWidth;
    result[1] += 2.0f * marginWidth;
    return calcExteriorSize(result);
} // end calcNaturalSize()

/*
 * deleteQuadrilateral - Delete selected quadrilateral.
 */
void HistogramWidget::deleteQuadrilateral(void) {
    Quadrilateral * _previousQuadrilateral = &quadrilaterals[currentQuadrilateral];
    Quadrilateral * _currentQuadrilateral = 0;
    QuadrilateralChangedCallbackData quadrilateralChangedCallbackData(this, _previousQuadrilateral, _currentQuadrilateral);
    removeQuadrilateral(currentQuadrilateral);
    currentQuadrilateral = numberOfQuadrilaterals;
    currentMode = modeNone;
    quadrilateralChangedCallbacks.call(&quadrilateralChangedCallbackData);
    getColorMap();
    HistogramWidgetChangedCallbackData histogramWidgetChangedCallbackData(this);
    histogramWidgetChangedCallbacks.call(&histogramWidgetChangedCallbackData);
} // end deleteQuadrilateral()

/*
 * draw - Draw the HistogramWidget.
 *
 * parameter glContextData - GLContextData&
 */
void HistogramWidget::draw(GLContextData& glContextData) const {
    Widget::draw(glContextData);
    DataItem* dataItem = glContextData.retrieveDataItem<DataItem> (this);
    updateHistogramTexture(dataItem);
    GLfloat x1 = areaBox.getCorner(0)[0];
    GLfloat x2 = areaBox.getCorner(1)[0];
    GLfloat y1 = areaBox.getCorner(0)[1];
    GLfloat y2 = areaBox.getCorner(2)[1];
    GLfloat z = areaBox.getCorner(0)[2];
    GLfloat width = x2 - x1;
    GLfloat height = y2 - y1;
    drawMargin();
    GLboolean lightingEnabled = glIsEnabled(GL_LIGHTING);
    if (lightingEnabled)
        glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    drawHistogramWidget(dataItem, x1, y1, z, width, height);
    drawQuadrilaterals();
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    if (lightingEnabled)
        glEnable(GL_LIGHTING);
} // end draw()

/*
 * drawHistogramWidget - Draw histogram.
 *
 * parameter dataItem - DataItem *
 * parameter x - GLfloat
 * parameter y - GLfloat
 * parameter z - GLfloat
 * parameter width - GLfloat
 * parameter height - GLfloat
 */
void HistogramWidget::drawHistogramWidget(DataItem * dataItem, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height) const {
    GLfloat _x = 0.0f;
    GLfloat _y = 0.0f;
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dataItem->histogramName);
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(x, y, z);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(x + width, y, z);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(x + width, y + height, z);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(x, y + height, z);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
} // end drawHistogramWidget()

/*
 * drawMargin - Draw margin area in background color.
 */
void HistogramWidget::drawMargin(void) const {
    glColor(backgroundColor);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex(getInterior().getCorner(0));
    glVertex(areaBox.getCorner(0));
    glVertex(areaBox.getCorner(2));
    glVertex(getInterior().getCorner(2));
    glVertex(getInterior().getCorner(1));
    glVertex(getInterior().getCorner(3));
    glVertex(areaBox.getCorner(3));
    glVertex(areaBox.getCorner(1));
    glVertex(getInterior().getCorner(0));
    glVertex(getInterior().getCorner(1));
    glVertex(areaBox.getCorner(1));
    glVertex(areaBox.getCorner(0));
    glVertex(getInterior().getCorner(2));
    glVertex(areaBox.getCorner(2));
    glVertex(areaBox.getCorner(3));
    glVertex(getInterior().getCorner(3));
    glEnd();
} // end drawMargin()

/*
 * drawQuadrilaterals - Draw quadrilaterals.
 */
void HistogramWidget::drawQuadrilaterals(void) const {
    GLfloat _normal = 1.0f / Math::sqrt(3.0f);
    GLfloat x1 = areaBox.getCorner(0)[0];
    GLfloat x2 = areaBox.getCorner(1)[0];
    GLfloat y1 = areaBox.getCorner(0)[1];
    GLfloat y2 = areaBox.getCorner(2)[1];
    for (int p = 0; p < numberOfQuadrilaterals; p++) {
        GLfloat ax = GLfloat(quadrilaterals[p].getAx() * (x2 - x1) + x1);
        GLfloat ay = GLfloat(quadrilaterals[p].getAy() * (y2 - y1) + y1);
        GLfloat bx = GLfloat(quadrilaterals[p].getBx() * (x2 - x1) + x1);
        GLfloat by = GLfloat(quadrilaterals[p].getBy() * (y2 - y1) + y1);
        GLfloat cx = GLfloat(quadrilaterals[p].getCx() * (x2 - x1) + x1);
        GLfloat cy = GLfloat(quadrilaterals[p].getCy() * (y2 - y1) + y1);
        GLfloat dx = GLfloat(quadrilaterals[p].getDx() * (x2 - x1) + x1);
        GLfloat dy = GLfloat(quadrilaterals[p].getDy() * (y2 - y1) + y1);
        GLfloat mx = GLfloat(quadrilaterals[p].getMx() * (x2 - x1) + x1);
        GLfloat my = GLfloat(quadrilaterals[p].getMy() * (y2 - y1) + y1);
        glColor4fv(quadrilaterals[p].getColor());
        // tessellate if neccessary (concave)
        GLdouble vertices[4][3];
        vertices[0][0] = ax;
        vertices[0][1] = ay;
        vertices[0][2] = areaBox.getCorner(0)[2] + marginWidth * 0.25f;
        vertices[1][0] = bx;
        vertices[1][1] = by;
        vertices[1][2] = areaBox.getCorner(0)[2] + marginWidth * 0.25f;
        vertices[2][0] = dx;
        vertices[2][1] = dy;
        vertices[2][2] = areaBox.getCorner(0)[2] + marginWidth * 0.25f;
        vertices[3][0] = cx;
        vertices[3][1] = cy;
        vertices[3][2] = areaBox.getCorner(0)[2] + marginWidth * 0.25f;
        TessellatePolygon polygon;
        polygon.initialize();
        polygon.setWindingRule(GLU_TESS_WINDING_POSITIVE);
        polygon.beginPolygon();
        polygon.beginContour();
        polygon.render(vertices, 4);
        polygon.endContour();
        polygon.endPolygon();
        polygon.end();
        // quadrilateral: position
        if (currentQuadrilateral == p) {
            if (dragging)
                glColor3f(0.0f, 1.0f, 0.0f);
            else
                glColor3f(0.0f, 1.0f, 1.0f);
        } else
            glColor3f(0.0f, 0.0f, 1.0f);
        glBegin(GL_LINE_STRIP);
        {
            glNormal3f(_normal, _normal, _normal);
            glVertex3f(ax, ay, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
            glVertex3f(bx, by, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
            glVertex3f(dx, dy, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
            glVertex3f(cx, cy, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
            glVertex3f(ax, ay, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
        }
        glEnd();

        if (currentQuadrilateral == p && currentMode == modeM) {
            if (dragging)
                glColor3f(0.0f, 1.0f, 0.0f);
            else
                glColor3f(0.0f, 1.0f, 1.0f);
        } else
            glColor3f(0.0f, 0.0f, 1.0f);
        glBegin(GL_QUADS);
        {
            glNormal3f(_normal, _normal, _normal);
            glVertex3f(mx + controlPointSize, my, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
            glVertex3f(mx, my + controlPointSize, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
            glVertex3f(mx - controlPointSize, my, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
            glVertex3f(mx, my - controlPointSize, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
        }
        glEnd();
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        {
            glNormal3f(_normal, _normal, _normal);
            glVertex3f(mx - controlPointSize * 0.5f, my - controlPointSize * 0.5f, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
            glVertex3f(mx + controlPointSize * 0.5f, my - controlPointSize * 0.5f, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
            glVertex3f(mx + controlPointSize * 0.5f, my + controlPointSize * 0.5f, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
            glVertex3f(mx - controlPointSize * 0.5f, my + controlPointSize * 0.5f, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
        }
        glEnd();
        // A vertex
        if (currentQuadrilateral == p && currentMode == modeA) {
            if (dragging)
                glColor3f(0.0f, 1.0f, 0.0f);
            else
                glColor3f(0.0f, 1.0f, 1.0f);
        } else
            glColor3f(0.0f, 0.0f, 1.0f);
        glBegin(GL_TRIANGLES);
        {
            glNormal3f(_normal, _normal, _normal);
            glVertex3f(ax - controlPointSize, ay - controlPointSize, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
            glVertex3f(ax, ay - controlPointSize, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
            glVertex3f(ax - controlPointSize, ay, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
        }
        glEnd();
        // B vertex
        if (currentQuadrilateral == p && currentMode == modeB) {
            if (dragging)
                glColor3f(0.0f, 1.0f, 0.0f);
            else
                glColor3f(0.0f, 1.0f, 1.0f);
        } else
            glColor3f(0.0f, 0.0f, 1.0f);
        glBegin(GL_TRIANGLES);
        {
            glNormal3f(_normal, _normal, _normal);
            glVertex3f(bx, by - controlPointSize, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
            glVertex3f(bx + controlPointSize, by - controlPointSize, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
            glVertex3f(bx + controlPointSize, by, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
        }
        glEnd();
        // C vertex
        if (currentQuadrilateral == p && currentMode == modeC) {
            if (dragging)
                glColor3f(0.0f, 1.0f, 0.0f);
            else
                glColor3f(0.0f, 1.0f, 1.0f);
        } else
            glColor3f(0.0f, 0.0f, 1.0f);
        glBegin(GL_TRIANGLES);
        {
            glNormal3f(_normal, _normal, _normal);
            glVertex3f(cx - controlPointSize, cy, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
            glVertex3f(cx, cy + controlPointSize, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
            glVertex3f(cx - controlPointSize, cy + controlPointSize, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
        }
        glEnd();
        // D vertex
        if (currentQuadrilateral == p && currentMode == modeD) {
            if (dragging)
                glColor3f(0.0f, 1.0f, 0.0f);
            else
                glColor3f(0.0f, 1.0f, 1.0f);
        } else
            glColor3f(0.0f, 0.0f, 1.0f);
        glBegin(GL_TRIANGLES);
        {
            glNormal3f(_normal, _normal, _normal);
            glVertex3f(dx + controlPointSize, dy, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
            glVertex3f(dx + controlPointSize, dy + controlPointSize, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
            glVertex3f(dx, dy + controlPointSize, areaBox.getCorner(0)[2] + marginWidth * 0.25f);
        }
        glEnd();
    }
} // end drawControlPoints()

/*
 * exportColorMap
 *
 * parameter _colormap - unsigned char *
 */
void HistogramWidget::exportColorMap(unsigned char * _colormap) const {
    for (int i = 0; i < 256 * 256; i++) {
        _colormap[4 * i + 0] = (unsigned char) (colormap[4 * i + 0] * 255.0f);
        _colormap[4 * i + 1] = (unsigned char) (colormap[4 * i + 1] * 255.0f);
        _colormap[4 * i + 2] = (unsigned char) (colormap[4 * i + 2] * 255.0f);
        _colormap[4 * i + 3] = (unsigned char) (colormap[4 * i + 3] * 255.0f);
    }
} // end exportColorMap()

/*
 * findQuadrilateral
 *
 * parameter x - float
 * parameter y - float
 * parameter z - float
 */
bool HistogramWidget::findQuadrilateral(float x, float y, float z) {
    Quadrilateral * _previousQuadrilateral = &quadrilaterals[currentQuadrilateral];
    currentQuadrilateral = -1;
    currentMode = modeNone;
    bool found = false;
    float mindist = FLT_MAX;
    float x1 = areaBox.getCorner(0)[0];
    float x2 = areaBox.getCorner(1)[0];
    float y1 = areaBox.getCorner(0)[1];
    float y2 = areaBox.getCorner(2)[1];
    float tx = x * (x2 - x1) + x1;
    float ty = y * (y2 - y1) / (1.0f - 0.0f) + y1;
    float tz = z;
    for (int p = 0; p < numberOfQuadrilaterals; p++) {
        float ax = float(quadrilaterals[p].getAx()) * (x2 - x1) + x1  - controlPointSize * 0.5f;
        float ay = float(quadrilaterals[p].getAy()) * (y2 - y1) / (1.0f - 0.0f) + y1 - controlPointSize * 0.5f;
        float bx = float(quadrilaterals[p].getBx()) * (x2 - x1) + x1 + controlPointSize * 0.5f;
        float by = float(quadrilaterals[p].getBy()) * (y2 - y1) / (1.0f - 0.0f) + y1 - controlPointSize * 0.5f;
        float cx = float(quadrilaterals[p].getCx()) * (x2 - x1) + x1 - controlPointSize * 0.5f;
        float cy = float(quadrilaterals[p].getCy()) * (y2 - y1) / (1.0f - 0.0f) + y1 + controlPointSize * 0.5f;
        float dx = float(quadrilaterals[p].getDx()) * (x2 - x1) + x1 + controlPointSize * 0.5f;
        float dy = float(quadrilaterals[p].getDy()) * (y2 - y1) / (1.0f - 0.0f) + y1 + controlPointSize * 0.5f;
        float mx = float(quadrilaterals[p].getMx()) * (x2 - x1) + x1;
        float my = float(quadrilaterals[p].getMy()) * (y2 - y1) / (1.0f - 0.0f) + y1;
        float z0 = float(areaBox.getCorner(0)[2]);

        float d1 = dist3(tx, ty, tz, ax, ay, z0);
        float d2 = dist3(tx, ty, tz, bx, by, z0);
        float d3 = dist3(tx, ty, tz, cx, cy, z0);
        float d4 = dist3(tx, ty, tz, dx, dy, z0);
        float d5 = dist3(tx, ty, tz, mx, my, z0);

        float rad = Math::sqr(1.0f * controlPointSize);

        if (d1 < rad && mindist > d1) {
            currentQuadrilateral = p;
            currentMode = modeA;
            mindist = d1;
            found = true;
        }
        if (d2 < rad && mindist > d2) {
            currentQuadrilateral = p;
            currentMode = modeB;
            mindist = d2;
            found = true;
        }
        if (d3 < rad && mindist > d3) {
            currentQuadrilateral = p;
            currentMode = modeC;
            mindist = d3;
            found = true;
        }
        if (d4 < rad && mindist > d4) {
            currentQuadrilateral = p;
            currentMode = modeD;
            mindist = d4;
            found = true;
        }
        if (d5 < rad && mindist > d5) {
            currentQuadrilateral = p;
            currentMode = modeM;
            mindist = d5;
            found = true;
        }
    }
    if (found) {
        Quadrilateral * _currentQuadrilateral = &quadrilaterals[currentQuadrilateral];
        QuadrilateralChangedCallbackData quadrilateralChangedCallbackData(this, _previousQuadrilateral, _currentQuadrilateral);
        quadrilateralChangedCallbacks.call(&quadrilateralChangedCallbackData);
    }
    return found;
} // end findQuadrilateral()

/*
 * findRecipient - Determine which is the applicable widget of the event. A virtual function of GLMotif::Widget base.
 *
 * parameter event - GLMotif::Event&
 * return - bool
 */
bool HistogramWidget::findRecipient(GLMotif::Event& event) {
    if (selected) {
        return event.setTargetWidget(this, event.calcWidgetPoint(this));
    } else
        return GLMotif::Widget::findRecipient(event);
} // end findRecipient()

/*
 * getColorMap
 */
void HistogramWidget::getColorMap(void) {
    for (int i = 0; i < 256 * 256 * 4; i++)
        colormap[i] = float(0);
    int numberOfPolygonCorners = 4;
    int nodes = 0;
    int nodeX[numberOfPolygonCorners];
    float polygonX[numberOfPolygonCorners];
    float polygonY[numberOfPolygonCorners];
    int top = 0;
    int bottom = 0;
    int right = 0;
    int left = 0;
    float * color;
    for (int p = 0; p < numberOfQuadrilaterals; p++) {
        top = int(255.0f * quadrilaterals[p].getMinimumY());
        bottom = int(255.0f * quadrilaterals[p].getMaximumY());
        left = int(255.0f * quadrilaterals[p].getMinimumX());
        right = int(255.0f * quadrilaterals[p].getMaximumX());
        polygonX[0] = (quadrilaterals[p].getAx() * 255.0f);
        polygonX[1] = (quadrilaterals[p].getBx() * 255.0f);
        polygonX[2] = (quadrilaterals[p].getCx() * 255.0f);
        polygonX[3] = (quadrilaterals[p].getDx() * 255.0f);
        polygonY[0] = (quadrilaterals[p].getAy() * 255.0f);
        polygonY[1] = (quadrilaterals[p].getBy() * 255.0f);
        polygonY[2] = (quadrilaterals[p].getCy() * 255.0f);
        polygonY[3] = (quadrilaterals[p].getDy() * 255.0f);
        color = quadrilaterals[p].getColor();
        //  Loop through the rows
        for (int pixelY = top; pixelY < bottom; pixelY++) {
            //  Build a list of nodes.
            nodes = 0;
            int j = numberOfPolygonCorners - 1;
            for (int i = 0; i < numberOfPolygonCorners; i++) {
                if (polygonY[i] < (double) pixelY && polygonY[j] >= (double) pixelY || polygonY[j] < (double) pixelY && polygonY[i]
                        >= (double) pixelY) {
                    nodeX[nodes++] = (int) (polygonX[i] + (pixelY - polygonY[i]) / (polygonY[j] - polygonY[i]) * (polygonX[j]
                            - polygonX[i]));
                }
                j = i;
            }

            //  Sort the nodes, via a simple “Bubble” sort.
            int i = 0;
            int tmp = 0;
            while (i < nodes - 1) {
                if (nodeX[i] > nodeX[i + 1]) {
                    tmp = nodeX[i];
                    nodeX[i] = nodeX[i + 1];
                    nodeX[i + 1] = tmp;
                    if (i)
                        i--;
                } else {
                    i++;
                }
            }

            //  Fill the pixels between node pairs.
            for (int i = 0; i < nodes; i += 2) {
                if (nodeX[i] >= right)
                    break;
                if (nodeX[i + 1] > left) {
                    if (nodeX[i] < left)
                        nodeX[i] = left;
                    if (nodeX[i + 1] > right)
                        nodeX[i + 1] = right;
                    for (j = nodeX[i]; j < nodeX[i + 1]; j++) {
                        colormap[4 * ((255 - pixelY) * 256 + j) + 0] = color[0];
                        colormap[4 * ((255 - pixelY) * 256 + j) + 1] = color[1];
                        colormap[4 * ((255 - pixelY) * 256 + j) + 2] = color[2];
                        colormap[4 * ((255 - pixelY) * 256 + j) + 3] = color[3];
                    }
                }
            }
        }
    }
} // end getColorMap()

/*
 * setControlPointSize - Set control point size.
 *
 * parameter _controlPointSize - GLfloat
 */
void HistogramWidget::setControlPointSize(GLfloat _controlPointSize) {
    controlPointSize = _controlPointSize;
} // end setControlPointSize()

/*
 * setDirectionChanged
 *
 * parameter directionChanged - bool
 */
void HistogramWidget::setDirectionChanged(bool directionChanged) {
    this->directionChanged = directionChanged;
} // end setDirectionChanged()

/*
 * isDragging
 *
 * return - bool
 */
bool HistogramWidget::isDragging(void) const {
    return dragging;
} // end isDragging()

/*
 * getHistogramWidgetChangedCallbacks
 *
 * return - Misc::CallbackList &
 */
Misc::CallbackList & HistogramWidget::getHistogramWidgetChangedCallbacks(void) {
    return histogramWidgetChangedCallbacks;
} // end getHistogramWidgetChangedCallbacks()

/*
 * setMarginWidth - Set the margin width.
 *
 * parameter _margineWidth - GLfloat
 */
void HistogramWidget::setMarginWidth(GLfloat _marginWidth) {
    marginWidth = _marginWidth;
    if (isManaged) {
        parent->requestResize(this, calcNaturalSize());
    } else
        resize(GLMotif::Box(GLMotif::Vector(0.0f, 0.0f, 0.0f), calcNaturalSize()));
} // end setMarginWidth()

/*
 * setPreferredSize - Set color map preferred size.
 *
 * parameter _preferredSize - const GLMotif::Vector&
 */
void HistogramWidget::setPreferredSize(const GLMotif::Vector& _preferredSize) {
    preferredSize = _preferredSize;
    if (isManaged) {
        parent->requestResize(this, calcNaturalSize());
    } else
        resize(GLMotif::Box(GLMotif::Vector(0.0f, 0.0f, 0.0f), calcNaturalSize()));
    GLfloat x1 = areaBox.getCorner(0)[0];
    GLfloat x2 = areaBox.getCorner(1)[0];
    GLfloat y1 = areaBox.getCorner(0)[1];
    GLfloat y2 = areaBox.getCorner(2)[1];
    GLfloat z = areaBox.getCorner(0)[2];
    GLfloat width = x2 - x1;
    GLfloat height = y2 - y1;
} // end setPreferredSize()

/*
 * getQuadrilateralChangedCallbacks
 *
 * return - Misc::CallbackList &
 */
Misc::CallbackList& HistogramWidget::getQuadrilateralChangedCallbacks(void) {
    return quadrilateralChangedCallbacks;
} // end getQuadrilateralChangedCallbacks()

/*
 * getQuadrilateralColor - Get current quadrilateral color.
 *
 * return - float *
 */
float * HistogramWidget::getQuadrilateralColor(void) {
    float * color = currentColor->getValues();
    if (currentQuadrilateral != -1) {
        color = quadrilaterals[currentQuadrilateral].getColor();
    }
    return color;
} // end getQuadrilateralColor()

/*
 * setQuadrilateralColor - Set current quadrilateral color.
 *
 * parameter values - float *
 */
void HistogramWidget::setQuadrilateralColor(float * values) {
    if (currentQuadrilateral != -1) {
        quadrilaterals[currentQuadrilateral].setColor(values);
        getColorMap();
        HistogramWidgetChangedCallbackData callbackData(this);
        histogramWidgetChangedCallbacks.call(&callbackData);
    }
} // end setQuadrilateralColor()

/*
 * setHistogramChanged
 *
 * parameter _sliceChanged - bool
 */
void HistogramWidget::setSliceChanged(bool _sliceChanged) {
    sliceChanged = _sliceChanged;
} // end setHistogramChanged()

/*
 * initContext
 *
 * parameter glContextData - GLContextData&
 */
void HistogramWidget::initContext(GLContextData& glContextData) const {
    DataItem * dataItem = new DataItem();
    glContextData.addDataItem(this, dataItem);
    dataItem->textureNonPowerOfTwo = GLARBTextureNonPowerOfTwo::isSupported();
    //glGetIntegerv(GL_MAX_2D_TEXTURE_SIZE, &dataItem->maximum2DTextureSize);
    initializeHistogramTexture(dataItem);
} // end initContext()

/*
 * initializeHistogramTexture
 *
 * parameter dataItem - DataItem *
 */
void HistogramWidget::initializeHistogramTexture(DataItem * dataItem) const {
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &dataItem->histogramName);
    glBindTexture(GL_TEXTURE_2D, dataItem->histogramName);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, 256, 256, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, histogram);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
} // end initializeHistogramTexture()

/*
 * pointerButtonDown - Pointer button down event handler. A virtual function of GLMotif::Widget base.
 *
 * parameter event - GLMotif::Event&
 */
void HistogramWidget::pointerButtonDown(GLMotif::Event& event) {
    selected = true;
    GLMotif::Point _point = event.getWidgetPoint().getPoint();
    float x = (_point[0] - float(areaBox.getCorner(0)[0])) * (1.0 - 0.0) / float(areaBox.getCorner(1)[0] - areaBox.getCorner(0)[0])
            + 0.0;
    float y = (_point[1] - areaBox.getCorner(0)[1]) * (1.0f - 0.0f) / (areaBox.getCorner(2)[1] - areaBox.getCorner(0)[1]) + 0.0f;
    float z = _point[2];
    if (!findQuadrilateral(x, y, z)) {
        Quadrilateral * _previousQuadrilateral = &quadrilaterals[currentQuadrilateral];
        Quadrilateral * _currentQuadrilateral = 0;
        QuadrilateralChangedCallbackData quadrilateralChangedCallbackData(this, _previousQuadrilateral, _currentQuadrilateral);
        currentQuadrilateral = -1;
        currentMode = modeNone;
        addQuadrilateral(x, y);
        quadrilateralChangedCallbacks.call(&quadrilateralChangedCallbackData);
    }
    dragging = true;
} // end pointerButtonDown()

/*
 * pointerButtonUp - Pointer button up event handler. A virtual function of GLMotif::Widget base.
 *
 * parameter event - GLMotif::Event&
 */
void HistogramWidget::pointerButtonUp(GLMotif::Event& event) {
    selected = false;
    if (dragging) {
        dragging = false;
        getColorMap();
        HistogramWidgetChangedCallbackData callbackData(this);
        histogramWidgetChangedCallbacks.call(&callbackData);
    }
} // end pointerButtonUp()

/*
 * pointerMotion - Pointer motion event handler. A virtual function of GLMotif::Widget base.
 *
 * parameter event - GLMotif::Event&
 */
void HistogramWidget::pointerMotion(GLMotif::Event& event) {
    GLMotif::Point _point = event.getWidgetPoint().getPoint();
    float x = (_point[0] - float(areaBox.getCorner(0)[0])) * (1.0f - 0.0f) / float(areaBox.getCorner(1)[0]
            - areaBox.getCorner(0)[0]) + 0.0f;
    if (x < 0.0f)
        x = 0.0f;
    if (x > 1.0f)
        x = 1.0f;
    float y = (_point[1] - areaBox.getCorner(0)[1]) * (1.0f - 0.0f) / (areaBox.getCorner(2)[1] - areaBox.getCorner(0)[1]) + 0.0f;
    if (y < 0.0f)
        y = 0.0f;
    else if (y > 1.0f)
        y = 1.0f;
    float z = _point[2];
    if (!dragging) {
        int oldQuadrilateral = currentQuadrilateral;
        Mode oldMode = currentMode;
        findQuadrilateral(x, y, z);
        if (oldQuadrilateral != currentQuadrilateral || oldMode != currentMode) {
            getColorMap();
            HistogramWidgetChangedCallbackData callbackData(this);
            histogramWidgetChangedCallbacks.call(&callbackData);
        }
    } else {
        switch (currentMode) {
            case modeA:
                quadrilaterals[currentQuadrilateral].setA(x, y);
                break;
            case modeB:
                quadrilaterals[currentQuadrilateral].setB(x, y);
                break;
            case modeC:
                quadrilaterals[currentQuadrilateral].setC(x, y);
                break;
            case modeD:
                quadrilaterals[currentQuadrilateral].setD(x, y);
                break;
            case modeM:
                bool move = true;
                float deltaX = x - quadrilaterals[currentQuadrilateral].getMx();
                float deltaY = y - quadrilaterals[currentQuadrilateral].getMy();
                float ax = quadrilaterals[currentQuadrilateral].getAx();
                float ay = quadrilaterals[currentQuadrilateral].getAy();
                float bx = quadrilaterals[currentQuadrilateral].getBx();
                float by = quadrilaterals[currentQuadrilateral].getBy();
                float cx = quadrilaterals[currentQuadrilateral].getCx();
                float cy = quadrilaterals[currentQuadrilateral].getCy();
                float dx = quadrilaterals[currentQuadrilateral].getDx();
                float dy = quadrilaterals[currentQuadrilateral].getDy();
                if (ax + deltaX < 0.0f || ax + deltaX > 1.0f)
                    move = false;
                else if (bx + deltaX < 0.0f || bx + deltaX > 1.0f)
                    move = false;
                else if (cx + deltaX < 0.0f || cx + deltaX > 1.0f)
                    move = false;
                else if (dx + deltaX < 0.0f || dx + deltaX > 1.0f)
                    move = false;
                if (ay + deltaY < 0.0f || ay + deltaY > 1.0f)
                    move = false;
                else if (by + deltaY < 0.0f || by + deltaY > 1.0f)
                    move = false;
                else if (cy + deltaY < 0.0f || cy + deltaY > 1.0f)
                    move = false;
                else if (cy + deltaY < 0.0f || cy + deltaY > 1.0f)
                    move = false;
                if (move)
                    quadrilaterals[currentQuadrilateral].setM(x, y);
                break;
        }
        getColorMap();
        HistogramWidgetChangedCallbackData callbackData(this);
        histogramWidgetChangedCallbacks.call(&callbackData);
    }
} // pointerMotion()

/*
 * removeQuadrilateral
 *
 * parameter which - int
 */
void HistogramWidget::removeQuadrilateral(int which) {
    for (int i = which; i < numberOfQuadrilaterals - 1; i++)
        quadrilaterals[i] = quadrilaterals[i + 1];
    numberOfQuadrilaterals--;
} // end removeQuadrilateral()

/*
 * resize - Resize the HistogramWidget display. A virtual function of GLMotif::Widget base.
 *
 * parameter _exterior - const GLMotif::Box&
 */
void HistogramWidget::resize(const GLMotif::Box& _exterior) {
    GLMotif::Widget::resize(_exterior);
    areaBox = getInterior();
    areaBox.doInset(GLMotif::Vector(marginWidth, marginWidth, 0.0f));
} // end resize()

/*
 * updateHistogram
 *
 * parameter _sliceDirection - int
 * parameter _which - int
 */
void HistogramWidget::updateHistogram(int _sliceDirection, int _which) {
    sliceDirection = _sliceDirection;
    which = _which;
    int width = volume->getWidth();
    int height = volume->getHeight();
    int depth = volume->getDepth();
    float maximum = volume->getMaximumGradient();
    for (int i = 0; i < 256 * 256; i++) {
        histogram[i] = 0;
    }
    if (sliceDirection == XY) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                int x = volume->getVoxel(i + j * width + which * (width * height));
                int y = int((log2(volume->getGradient(i + j * width + which * (width * height))) / log2(maximum)) * 255.0);
                histogram[y * 256 + x] = (unsigned char) (255);
            }
        }
    } else if (sliceDirection == XZ) {
        for (int j = 0; j < depth; j++) {
            for (int i = 0; i < width; i++) {
                int x = volume->getVoxel(i + which * width + j * (width * height));
                int y = int((log2(volume->getGradient(i + which * width + j * (width * height))) / log2(maximum)) * 255.0);
                histogram[y * 256 + x] = (unsigned char) (255);
            }
        }
    } else {
        for (int j = 0; j < depth; j++) {
            for (int i = 0; i < height; i++) {
                int x = volume->getVoxel(which + i * width + j * (width * height));
                int y = int((log2(volume->getGradient(which + i * width + j * (width * height))) / log2(maximum)) * 255.0);
                histogram[y * 256 + x] = (unsigned char) (255);
            }
        }
    }
} // end updateHistogram()

/*
 * updateHistogramTexture
 *
 * parameter dataItem - DataItem *
 */
void HistogramWidget::updateHistogramTexture(DataItem * dataItem) const {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, dataItem->histogramName);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_LUMINANCE, GL_UNSIGNED_BYTE, histogram);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
} // end updateHistogramTexture()
