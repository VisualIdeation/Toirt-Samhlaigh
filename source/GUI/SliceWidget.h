/*
 * SliceWidget.h - Class for creating a slice viewer.
 *
 * Author: Patrick O'Leary
 * Created: August 18, 2008
 * Copyright: 2008. All rights reserved.
 */
#ifndef SLICEWIDGET_INCLUDED
#define SLICEWIDGET_INCLUDED

#include <GL/gl.h>

/* Vrui includes */
#include <GL/GLContextData.h>
#include <GL/GLObject.h>
#include <GLMotif/Container.h>
#include <GLMotif/Event.h>
#include <GLMotif/Types.h>
#include <GLMotif/Widget.h>
#include <Misc/CallbackList.h>

#define XY 0
#define XZ 1
#define YZ 2

// begin Forward Declarations
class Volume;
// end Forward Declarations
class SliceWidget: public GLMotif::Widget, public GLObject {
protected:
    struct DataItem: public GLObject::DataItem {
        GLuint xySliceName;
        GLuint xzSliceName;
        GLuint yzSliceName;
        bool textureNonPowerOfTwo;
        GLint maximum2DTextureSize;
        DataItem(void);
        ~DataItem(void);
    };
public:
    friend class DataItem;
    SliceWidget(const char * _name, GLMotif::Container * _parent, Volume * _volume, bool _manageChild = true);
    virtual ~SliceWidget(void);
    virtual GLMotif::Vector calcNaturalSize(void) const;
    virtual void draw(GLContextData& contextData) const;
    void drawSliceWidget(DataItem * dataItem, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height) const;
    void drawMargin(void) const;
    virtual bool findRecipient(GLMotif::Event & event);
    void setBounds(GLfloat width, GLfloat height);
    void setDirectionChanged(bool directionChanged);
    void setMarginWidth(GLfloat _marginWidth);
    void setPreferredSize(const GLMotif::Vector & _preferredSize);
    void setSliceChanged(bool _sliceChanged);
    void initContext(GLContextData& glContextData) const;
    void initializeSliceTexture(DataItem* dataItem) const;
    virtual void pointerButtonDown(GLMotif::Event & event);
    virtual void pointerButtonUp(GLMotif::Event & event);
    virtual void pointerMotion(GLMotif::Event & event);
    virtual void resize(const GLMotif::Box & _exterior);
    void updateSlice(int _sliceDirection, int _which);
    void updateSliceTexture(DataItem * dataItem) const;
private:
    DataItem * dataItem;
    bool directionChanged;
    GLfloat marginWidth;
    GLMotif::Vector preferredSize;
    bool selected;
    GLMotif::Box SliceWidgetAreaBox;
    bool sliceChanged;
    int sliceDirection;
    Volume * volume;
    int which;
    unsigned char * xySlice;
    unsigned char * xzSlice;
    unsigned char * yzSlice;
};

#endif
