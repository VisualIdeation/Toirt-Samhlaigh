/*
 * LightWidget.h - Class for determining light direction.
 *
 * Author: Patrick O'Leary
 * Created: September 26, 2008
 * Copyright: 2008. All rights reserved.
 */
#ifndef LIGHTWIDGET_INCLUDED
#define LIGHTWIDGET_INCLUDED

#include <GL/gl.h>
#include <GL/glu.h>

/* Vrui includes */
#include <GL/GLColor.h>
#include <GL/GLContextData.h>
#include <GL/GLObject.h>
#include <GLMotif/Container.h>
#include <GLMotif/Event.h>
#include <GLMotif/Types.h>
#include <GLMotif/Widget.h>

// begin Forward Declarations
class GLLight;
class Scene;
// end Forward Declarations
class LightWidget: public GLMotif::Widget, public GLObject {
protected:
    struct DataItem: public GLObject::DataItem {
        DataItem(void);
        ~DataItem(void);
        GLLight * light;
    };
public:
    friend class DataItem;
    LightWidget(Scene * _scene, const char * _name, GLMotif::Container * _parent, bool _manageChild = true);
    virtual ~LightWidget(void);
    virtual GLMotif::Vector calcNaturalSize(void) const;
    virtual void draw(GLContextData& contextData) const;
    void drawCylinder(GLfloat height, GLfloat width, GLfloat x, GLfloat y, GLfloat z) const;
    void drawLightWidget(GLfloat height, GLfloat width, GLfloat x, GLfloat y, GLfloat z) const;
    void drawMargin(void) const;
    void drawOutline(GLfloat height, GLfloat width, GLfloat x, GLfloat y, GLfloat z) const;
    void drawSphere(GLfloat height, GLfloat width, GLfloat x, GLfloat y, GLfloat z) const;
    virtual bool findRecipient(GLMotif::Event & event);
    float * getAmbientColor(void);
    void setAmbientColor(float * color);
    float * getDiffuseColor(void);
    void setDiffuseColor(float * color);
    void setBounds(GLfloat width, GLfloat height);
    void setExtent(float * _extent);
    void setLightPosition(float x, float y, float width, float height);
    void setMarginWidth(GLfloat _marginWidth);
    void setObjectSize(float objectSize);
    void setOrigin(float * _extent);
    void setPreferredSize(const GLMotif::Vector & _preferredSize);
    float * getSpecularColor(void);
    void setSpecularColor(float * color);
    void glLight(GLsizei lightIndex, GLLight * light) const;
    void initContext(GLContextData& glContextData) const;
    virtual void pointerButtonDown(GLMotif::Event & event);
    virtual void pointerButtonUp(GLMotif::Event & event);
    virtual void pointerMotion(GLMotif::Event & event);
    virtual void resize(const GLMotif::Box & _exterior);
private:
    GLColor<float,4> ambient;
    DataItem * dataItem;
    GLColor<float,4> diffuse;
    float * extent;
    float lightPosition[4];
    GLMotif::Box LightWidgetAreaBox;
    GLfloat marginWidth;
    float objectSize;
    float * origin;
    GLMotif::Vector preferredSize;
    Scene * scene;
    bool selected;
    GLColor<float,4> specular;
};

#endif
