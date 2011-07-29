/*
 * HistogramWidget.h - Class for creating a slice and histogram viewer.
 *
 * Author: Patrick O'Leary
 * Created: August 18, 2008
 * Copyright: 2008. All rights reserved.
 */
#ifndef HISTOGRAMWIDGET_INCLUDED
#define HISTOGRAMWIDGET_INCLUDED

#include <GL/gl.h>

/* Vrui includes */
#include <GL/GLContextData.h>
#include <GL/GLObject.h>
#include <GLMotif/Container.h>
#include <GLMotif/Event.h>
#include <GLMotif/Types.h>
#include <GLMotif/Widget.h>
#include <Misc/CallbackList.h>

#include <DATASTRUCTURE/Quadrilateral.h>

#define XY 0
#define XZ 1
#define YZ 2

#define dist2(x1,y1,x2,y2) (((x2)-(x1))*((x2)-(x1)) + ((y2)-(y1))*((y2)-(y1)))
#define dist3(x1,y1,z1,x2,y2,z2) (((x2)-(x1))*((x2)-(x1)) + ((y2)-(y1))*((y2)-(y1)) + ((z2)-(z1))*((z2)-(z1)))

enum Mode {
    modeNone, modeA, modeB, modeC, modeD, modeM
};

// begin Forward Declarations
class RGBAColor;
class Volume;
// end Forward Declarations
class HistogramWidget: public GLMotif::Widget, public GLObject {
protected:
    struct DataItem: public GLObject::DataItem {
        GLuint histogramName;
        bool textureNonPowerOfTwo;
        GLint maximum2DTextureSize;
        DataItem(void);
        ~DataItem(void);
    };
public:
    friend class DataItem;
    HistogramWidget(const char * _name, GLMotif::Container * _parent, Volume * _volume, bool _manageChild = true);
    virtual ~HistogramWidget(void);
    void addQuadrilateral(float x, float y);
    virtual GLMotif::Vector calcNaturalSize(void) const;
    void deleteQuadrilateral(void);
    virtual void draw(GLContextData& contextData) const;
    void drawHistogramWidget(DataItem * dataItem, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height) const;
    void drawMargin(void) const;
    void drawQuadrilaterals(void) const;
    void exportColorMap(unsigned char * _colormap) const;
    virtual bool findRecipient(GLMotif::Event & event);
    void setBounds(GLfloat width, GLfloat height);
    void getColorMap(void);
    void setControlPointSize(GLfloat _controlPointSize);
    void setDirectionChanged(bool directionChanged);
    bool isDragging(void) const;
    Misc::CallbackList & getHistogramWidgetChangedCallbacks(void);
    void setMarginWidth(GLfloat _marginWidth);
    void setPreferredSize(const GLMotif::Vector & _preferredSize);
    void setSliceChanged(bool _sliceChanged);
    bool findQuadrilateral(float x, float y, float z);
    Misc::CallbackList& getQuadrilateralChangedCallbacks(void);
    float * getQuadrilateralColor(void);
    void setQuadrilateralColor(float * values);
    void initContext(GLContextData& glContextData) const;
    void initializeHistogramTexture(DataItem* dataItem) const;
    virtual void pointerButtonDown(GLMotif::Event & event);
    virtual void pointerButtonUp(GLMotif::Event & event);
    virtual void pointerMotion(GLMotif::Event & event);
    void removeQuadrilateral(int which);
    virtual void resize(const GLMotif::Box & _exterior);
    void updateHistogram(int _sliceDirection, int _which);
    void updateHistogramTexture(DataItem * dataItem) const;
private:
    GLMotif::Box areaBox;
    Misc::CallbackList histogramWidgetChangedCallbacks;
    float * colormap;
    GLfloat controlPointSize;
    RGBAColor * currentColor;
    Mode currentMode;
    int currentQuadrilateral;
    DataItem * dataItem;
    bool directionChanged;
    bool dragging;
    unsigned char * histogram;
    GLfloat marginWidth;
    int numberOfQuadrilaterals;
    GLMotif::Vector preferredSize;
    Quadrilateral quadrilaterals[200];
    Misc::CallbackList quadrilateralChangedCallbacks;
    bool selected;
    bool sliceChanged;
    int sliceDirection;
    Volume * volume;
    int which;
};

#endif
