/*
 * LightWidget.cpp - Methods for LightWidget class.
 *
 * Author: Patrick O'Leary
 * Created: September 26, 2008
 * Copyright: 2008. All rights reserved.
 */
#include <iostream>

/* Vrui includes */
#include <GL/GLLight.h>
#include <GL/GLColorTemplates.h>
#include <GL/GLLightTemplates.h>
#include <GL/GLVertexTemplates.h>

#include <GRAPHIC/Scene.h>
#include <GUI/LightWidget.h>

/*
 * DataItem - Constructor for structure DataItem
 */
LightWidget::DataItem::DataItem(void) {
} // end DataItem()

/*
 * ~DataItem - Destructor for structure DataItem
 */
LightWidget::DataItem::~DataItem(void) {
} // end ~DataItem()

/*
 * LightWidget - Constructor for the LightWidget class.
 *
 * parameter _name - const char *
 * parameter _parent - GLMotif::Container *
 * parameter volume - Volume *
 * parameter _manageChild - bool
 */
LightWidget::LightWidget(Scene * _scene, const char * _name, GLMotif::Container * _parent, bool _manageChild) :
    GLMotif::Widget(_name, _parent, false), ambient(0.0f, 0.0f, 0.0f, 1.0f), diffuse(1.0f, 1.0f, 1.0f, 1.0f), objectSize(1.0),
            scene(_scene), specular(1.0f, 1.0f, 1.0f, 1.0f) {
    extent = new float[3];
    origin = new float[3];
    marginWidth = 0.0f;
    preferredSize[0] = 0.0f;
    preferredSize[1] = 0.0f;
    preferredSize[2] = 0.0f;
    if (_manageChild)
        manageChild();
} // end LightWidget()

/*
 * ~LightWidget - Destructor for the LightWidget class.
 */
LightWidget::~LightWidget(void) {
} // end ~LightWidget()

/*
 * calcNaturalSize - Determine the natural size of the LightWidget. A virtual function of GLMotif::Widget base.
 *
 * return - GLMotif::Vector
 */
GLMotif::Vector LightWidget::calcNaturalSize(void) const {
    GLMotif::Vector result = preferredSize;
    result[0] += 2.0f * marginWidth;
    result[1] += 2.0f * marginWidth;
    return calcExteriorSize(result);
} // end calcNaturalSize()

/*
 * draw - Draw the LightWidget.
 *
 * parameter glContextData - GLContextData&
 */
void LightWidget::draw(GLContextData& glContextData) const {
    Widget::draw(glContextData);
    DataItem * dataItem = glContextData.retrieveDataItem<DataItem> (this);
    GLfloat x1 = LightWidgetAreaBox.getCorner(0)[0];
    GLfloat x2 = LightWidgetAreaBox.getCorner(1)[0];
    GLfloat y1 = LightWidgetAreaBox.getCorner(0)[1];
    GLfloat y2 = LightWidgetAreaBox.getCorner(2)[1];
    GLfloat z = LightWidgetAreaBox.getCorner(0)[2];
    GLfloat width = x2 - x1;
    GLfloat height = y2 - y1;
    drawMargin();
    GLboolean lightingEnabled = glIsEnabled(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glLight(1, dataItem->light);
    drawLightWidget(height, width, x1, y1, z);
    glDisable(GL_LIGHT1);
    glEnable(GL_LIGHT0);
} // end draw()

/*
 * drawCylinder
 *
 * parameter height - GLfloat
 * parameter width - GLfloat
 * parameter x - GLfloat
 * parameter y - GLfloat
 * parameter z - GLfloat
 */
void LightWidget::drawCylinder(GLfloat height, GLfloat width, GLfloat x, GLfloat y, GLfloat z) const {
    glTranslatef(lightPosition[0] - (x + width / 2.0f), lightPosition[1] - (y + height / 2.0f), lightPosition[2]);
    glColor3f(0.0f, 0.0f, 1.0f);
    GLUquadricObj *gluQuadricObj = gluNewQuadric();
    gluQuadricDrawStyle(gluQuadricObj, GLU_FILL);
    gluQuadricNormals(gluQuadricObj, GLU_NONE);
    gluCylinder(gluQuadricObj, 0.125f * objectSize, 0, 0.25f * objectSize, 20, 20);
    gluDeleteQuadric(gluQuadricObj);
}

/*
 * drawLightWidget
 *
 * parameter height - GLfloat
 * parameter width - GLfloat
 * parameter x - GLfloat
 * parameter y - GLfloat
 * parameter z - GLfloat
 */
void LightWidget::drawLightWidget(GLfloat height, GLfloat width, GLfloat x, GLfloat y, GLfloat z) const {
    glPushMatrix();
    drawOutline(height, width, x, y, z);
    drawSphere(height, width, x, y, z);
    drawCylinder(height, width, x, y, z);
    glPopMatrix();
} // end drawLightWidget()

/*
 * drawMargin - Draw margin area in background color.
 */
void LightWidget::drawMargin(void) const {
    glColor(backgroundColor);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex(getInterior().getCorner(0));
    glVertex(LightWidgetAreaBox.getCorner(0));
    glVertex(LightWidgetAreaBox.getCorner(2));
    glVertex(getInterior().getCorner(2));
    glVertex(getInterior().getCorner(1));
    glVertex(getInterior().getCorner(3));
    glVertex(LightWidgetAreaBox.getCorner(3));
    glVertex(LightWidgetAreaBox.getCorner(1));
    glVertex(getInterior().getCorner(0));
    glVertex(getInterior().getCorner(1));
    glVertex(LightWidgetAreaBox.getCorner(1));
    glVertex(LightWidgetAreaBox.getCorner(0));
    glVertex(getInterior().getCorner(2));
    glVertex(LightWidgetAreaBox.getCorner(2));
    glVertex(LightWidgetAreaBox.getCorner(3));
    glVertex(getInterior().getCorner(3));
    glEnd();
} // end drawMargin()

/*
 * drawOutline
 *
 * parameter height - GLfloat
 * parameter width - GLfloat
 * parameter x - GLfloat
 * parameter y - GLfloat
 * parameter z - GLfloat
 */
void LightWidget::drawOutline(GLfloat height, GLfloat width, GLfloat x, GLfloat y, GLfloat z) const {
    GLfloat lineWidth;
    glGetFloatv(GL_LINE_WIDTH, &lineWidth);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    {
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(x + width / 2.0f - 1.0f * objectSize, y + height / 2.0f - 1.0f * objectSize, z + 1.0f * objectSize);
        glVertex3f(x + width / 2.0f + 1.0f * objectSize, y + height / 2.0f - 1.0f * objectSize, z + 1.0f * objectSize);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex3f(x + width / 2.0f - 1.0f * objectSize, y + height / 2.0f - 1.0f * objectSize, z - 1.0f * objectSize);
        glVertex3f(x + width / 2.0f + 1.0f * objectSize, y + height / 2.0f - 1.0f * objectSize, z - 1.0f * objectSize);
        glVertex3f(x + width / 2.0f - 1.0f * objectSize, y + height / 2.0f + 1.0f * objectSize, z + 1.0f * objectSize);
        glVertex3f(x + width / 2.0f + 1.0f * objectSize, y + height / 2.0f + 1.0f * objectSize, z + 1.0f * objectSize);
        glVertex3f(x + width / 2.0f - 1.0f * objectSize, y + height / 2.0f + 1.0f * objectSize, z - 1.0f * objectSize);
        glVertex3f(x + width / 2.0f + 1.0f * objectSize, y + height / 2.0f + 1.0f * objectSize, z - 1.0f * objectSize);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(x + width / 2.0f - 1.0f * objectSize, y + height / 2.0f - 1.0f * objectSize, z + 1.0f * objectSize);
        glVertex3f(x + width / 2.0f - 1.0f * objectSize, y + height / 2.0f - 1.0f * objectSize, z - 1.0f * objectSize);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex3f(x + width / 2.0f + 1.0f * objectSize, y + height / 2.0f - 1.0f * objectSize, z + 1.0f * objectSize);
        glVertex3f(x + width / 2.0f + 1.0f * objectSize, y + height / 2.0f - 1.0f * objectSize, z - 1.0f * objectSize);
        glVertex3f(x + width / 2.0f - 1.0f * objectSize, y + height / 2.0f + 1.0f * objectSize, z + 1.0f * objectSize);
        glVertex3f(x + width / 2.0f - 1.0f * objectSize, y + height / 2.0f + 1.0f * objectSize, z - 1.0f * objectSize);
        glVertex3f(x + width / 2.0f + 1.0f * objectSize, y + height / 2.0f + 1.0f * objectSize, z + 1.0f * objectSize);
        glVertex3f(x + width / 2.0f + 1.0f * objectSize, y + height / 2.0f + 1.0f * objectSize, z - 1.0f * objectSize);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(x + width / 2.0f - 1.0f * objectSize, y + height / 2.0f - 1.0f * objectSize, z + 1.0f * objectSize);
        glVertex3f(x + width / 2.0f - 1.0f * objectSize, y + height / 2.0f + 1.0f * objectSize, z + 1.0f * objectSize);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex3f(x + width / 2.0f + 1.0f * objectSize, y + height / 2.0f - 1.0f * objectSize, z + 1.0f * objectSize);
        glVertex3f(x + width / 2.0f + 1.0f * objectSize, y + height / 2.0f + 1.0f * objectSize, z + 1.0f * objectSize);
        glVertex3f(x + width / 2.0f - 1.0f * objectSize, y + height / 2.0f - 1.0f * objectSize, z - 1.0f * objectSize);
        glVertex3f(x + width / 2.0f - 1.0f * objectSize, y + height / 2.0f + 1.0f * objectSize, z - 1.0f * objectSize);
        glVertex3f(x + width / 2.0f + 1.0f * objectSize, y + height / 2.0f - 1.0f * objectSize, z - 1.0f * objectSize);
        glVertex3f(x + width / 2.0f + 1.0f * objectSize, y + height / 2.0f + 1.0f * objectSize, z - 1.0f * objectSize);
    }
    glEnd();
    glLineWidth(lineWidth);
}

/*
 * drawSphere
 *
 * parameter height - GLfloat
 * parameter width - GLfloat
 * parameter x - GLfloat
 * parameter y - GLfloat
 * parameter z - GLfloat
 */
void LightWidget::drawSphere(GLfloat height, GLfloat width, GLfloat x, GLfloat y, GLfloat z) const {
    glTranslatef(x + width / 2.0f, y + height / 2.0f, z);
    GLfloat position[4];
    position[0] = lightPosition[0] - (x + width / 2.0f);
    position[1] = lightPosition[1] - (y + height / 2.0f);
    position[2] = lightPosition[2];
    position[3] = lightPosition[3];
    glLightfv(GL_LIGHT1, GL_POSITION, position);
    glColor3f(0.7f, 0.7f, 0.7f);
    GLUquadricObj* gluQuadricObj = gluNewQuadric();
    gluQuadricDrawStyle(gluQuadricObj, GLU_FILL);
    gluQuadricNormals(gluQuadricObj, GLU_SMOOTH);
    gluSphere(gluQuadricObj, 1.0f * objectSize, 20, 20);
    gluDeleteQuadric(gluQuadricObj);
} // end drawSphere()

/*
 * findRecipient - Determine which is the applicable widget of the event. A virtual function of GLMotif::Widget base.
 *
 * parameter event - GLMotif::Event&
 * return - bool
 */
bool LightWidget::findRecipient(GLMotif::Event& event) {
    if (selected) {
        return event.setTargetWidget(this, event.calcWidgetPoint(this));
    } else
        return GLMotif::Widget::findRecipient(event);
} // end findRecipient()

/*
 * getAmbientColor
 *
 * return - float *
 */
float * LightWidget::getAmbientColor(void) {
    float * color = new float[4];
    color[0] = ambient[0];
    color[1] = ambient[1];
    color[2] = ambient[2];
    color[3] = ambient[3];
    return color;
} // end getAmbientColor()

/*
 * setAmbientColor
 *
 * parameter color - float *
 */
void LightWidget::setAmbientColor(float * color) {
    ambient[0] = color[0];
    ambient[1] = color[1];
    ambient[2] = color[2];
    ambient[3] = color[3];
} // end setAmbientColor()

/*
 * getDiffuseColor
 *
 * return - float *
 */
float * LightWidget::getDiffuseColor(void) {
    float * color = new float[4];
    color[0] = diffuse[0];
    color[1] = diffuse[1];
    color[2] = diffuse[2];
    color[3] = diffuse[3];
    return color;
} // end getDiffuseColor()

/*
 * setDiffuseColor
 *
 * parameter color - float *
 */
void LightWidget::setDiffuseColor(float * color) {
    diffuse[0] = color[0];
    diffuse[1] = color[1];
    diffuse[2] = color[2];
    diffuse[3] = color[3];
} // end setDiffuseColor()

/*
 * setExtent
 *
 * parameter _extent - float *
 */
void LightWidget::setExtent(float * _extent) {
    extent[0] = _extent[0];
    extent[1] = _extent[1];
    extent[2] = _extent[2];
} // end setExtent()

/*
 * setLightPosition
 *
 * parameter x - float
 * parameter y - float
 * parameter width - float
 * parameter height - float
 */
void LightWidget::setLightPosition(float x, float y, float width, float height) {
    float * position = new float[3];
    position[0] = (-1.0f * (lightPosition[0] - (x + width / 2.0f)) / (x + width / 2.0f)) * extent[0] * 1.5f;
    position[1] = (-1.0f * (lightPosition[1] - (y + height / 2.0f)) / (y + height / 2.0f)) * extent[1] * 1.5f;
    position[2] = extent[2] * 1.5f;
    scene->setLightPosition(position[0], position[1], position[2]);
} // end setLightPosition()

/*
 * setMarginWidth - Set the margin width.
 *
 * parameter _margineWidth - GLfloat
 */
void LightWidget::setMarginWidth(GLfloat _marginWidth) {
    marginWidth = _marginWidth;
    if (isManaged) {
        parent->requestResize(this, calcNaturalSize());
    } else
        resize(GLMotif::Box(GLMotif::Vector(0.0f, 0.0f, 0.0f), calcNaturalSize()));
} // end setMarginWidth()

/*
 * setObjectSize
 *
 * parameter objectSize - float
 */
void LightWidget::setObjectSize(float objectSize) {
    this->objectSize = objectSize;
} // end setObjectSize()

/*
 * setOrigin
 *
 * parameter _origin - float *
 */
void LightWidget::setOrigin(float * _origin) {
    origin[0] = _origin[0];
    origin[1] = _origin[1];
    origin[2] = _origin[2];
} // end setOrigin()

/*
 * setPreferredSize - Set color map preferred size.
 *
 * parameter _preferredSize - const GLMotif::Vector&
 */
void LightWidget::setPreferredSize(const GLMotif::Vector& _preferredSize) {
    preferredSize = _preferredSize;
    if (isManaged) {
        parent->requestResize(this, calcNaturalSize());
    } else
        resize(GLMotif::Box(GLMotif::Vector(0.0f, 0.0f, 0.0f), calcNaturalSize()));
    GLfloat x1 = LightWidgetAreaBox.getCorner(0)[0];
    GLfloat x2 = LightWidgetAreaBox.getCorner(1)[0];
    GLfloat y1 = LightWidgetAreaBox.getCorner(0)[1];
    GLfloat y2 = LightWidgetAreaBox.getCorner(2)[1];
    GLfloat z = LightWidgetAreaBox.getCorner(0)[2];
    GLfloat width = x2 - x1;
    GLfloat height = y2 - y1;
    lightPosition[0] = x1 + width / 2.0f;
    lightPosition[1] = y1 + height / 2.0f;
    lightPosition[2] = z + 2.0f;
    lightPosition[3] = 1.0f;
} // end setPreferredSize()

/*
 * getSpecularColor
 *
 * return - float *
 */
float * LightWidget::getSpecularColor(void) {
    float * color = new float[4];
    color[0] = specular[0];
    color[1] = specular[1];
    color[2] = specular[2];
    color[3] = specular[3];
    return color;
} // end getSpecularColor()

/*
 * setSpecularColor
 *
 * parameter color - float *
 */
void LightWidget::setSpecularColor(float * color) {
    specular[0] = color[0];
    specular[1] = color[1];
    specular[2] = color[2];
    specular[3] = color[3];
} // end setSpecularColor()

/*
 * glLight
 *
 * parameter lightIndex - GLsizei
 * parameter light - GLLight *
 */
void LightWidget::glLight(GLsizei lightIndex, GLLight * light) const {
    glLightAmbient(lightIndex, ambient);
    glLightDiffuse(lightIndex, diffuse);
    glLightSpecular(lightIndex, specular);
    glLightPosition(lightIndex, light->position);
    glLightSpotDirection(lightIndex, light->spotDirection);
    glLightSpotCutoff(lightIndex, light->spotCutoff);
    glLightSpotExponent(lightIndex, light->spotExponent);
    glLightConstantAttenuation(lightIndex, light->constantAttenuation);
    glLightLinearAttenuation(lightIndex, light->linearAttenuation);
    glLightQuadraticAttenuation(lightIndex, light->quadraticAttenuation);
} // end glLight()

/*
 * initContext
 *
 * parameter glContextData - GLContextData&
 */
void LightWidget::initContext(GLContextData& glContextData) const {
    DataItem * dataItem = new DataItem();
    glContextData.addDataItem(this, dataItem);
    dataItem->light = new GLLight();
    dataItem->light->diffuse = GLColor<float, 4> (1.0, 0.0, 0.0, 1.0);
} // end initContext()

/*
 * pointerButtonDown - Pointer button down event handler. A virtual function of GLMotif::Widget base.
 *
 * parameter event - GLMotif::Event&
 */
void LightWidget::pointerButtonDown(GLMotif::Event& event) {
    selected = true;
    GLfloat x1 = getInterior().getCorner(0)[0];
    GLfloat x2 = getInterior().getCorner(1)[0];
    GLfloat y1 = getInterior().getCorner(0)[1];
    GLfloat y2 = getInterior().getCorner(2)[1];
    GLfloat z = getInterior().getCorner(0)[2];
    GLfloat width = x2 - x1;
    GLfloat height = y2 - y1;
    float x = event.getWidgetPoint().getPoint()[0];
    float y = event.getWidgetPoint().getPoint()[1];
    if (x < x1)
        lightPosition[0] = x1;
    else
        lightPosition[0] = x;
    if (x > (x1 + width))
        lightPosition[0] = x1 + width;
    else
        lightPosition[0] = x;
    if (y < y1)
        lightPosition[1] = y1;
    else
        lightPosition[1] = y;
    if (y > (y1 + height))
        lightPosition[1] = y1 + height;
    else
        lightPosition[1] = y;
    setLightPosition(x1, y1, width, height);
} // end pointerButtonDown()

/*
 * pointerButtonUp - Pointer button up event handler. A virtual function of GLMotif::Widget base.
 *
 * parameter event - GLMotif::Event&
 */
void LightWidget::pointerButtonUp(GLMotif::Event& event) {
    selected = false;
} // end pointerButtonUp()

/*
 * pointerMotion - Pointer motion event handler. A virtual function of GLMotif::Widget base.
 *
 * parameter event - GLMotif::Event&
 */
void LightWidget::pointerMotion(GLMotif::Event& event) {
    GLfloat x1 = getInterior().getCorner(0)[0];
    GLfloat x2 = getInterior().getCorner(1)[0];
    GLfloat y1 = getInterior().getCorner(0)[1];
    GLfloat y2 = getInterior().getCorner(2)[1];
    GLfloat z = getInterior().getCorner(0)[2];
    GLfloat width = x2 - x1;
    GLfloat height = y2 - y1;
    float x = event.getWidgetPoint().getPoint()[0];
    float y = event.getWidgetPoint().getPoint()[1];
    if (x < x1)
        lightPosition[0] = x1;
    else if (x > (x1 + width))
        lightPosition[0] = x1 + width;
    else
        lightPosition[0] = x;
    if (y < y1)
        lightPosition[1] = y1;
    else if (y > (y1 + height))
        lightPosition[1] = y1 + height;
    else
        lightPosition[1] = y;
    setLightPosition(x1, y1, width, height);
} // pointerMotion()

/*
 * resize - Resize the LightWidget display. A virtual function of GLMotif::Widget base.
 *
 * parameter _exterior - const GLMotif::Box&
 */
void LightWidget::resize(const GLMotif::Box& _exterior) {
    GLMotif::Widget::resize(_exterior);
    LightWidgetAreaBox = getInterior();
    LightWidgetAreaBox.doInset(GLMotif::Vector(marginWidth, marginWidth, 0.0f));
} // end resize()
