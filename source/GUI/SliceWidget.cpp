/*
 * SliceWidget.cpp - Methods for SliceWidget class.
 *
 * Author: Patrick O'Leary
 * Created: August 18, 2008
 * Copyright: 2008. All rights reserved.
 */

/* Vrui includes */
#include <GL/Extensions/GLARBTextureNonPowerOfTwo.h>
#include <GL/GLColorTemplates.h>
#include <GL/GLVertexTemplates.h>

#include <DATA/Volume.h>
#include <GUI/SliceWidget.h>
#include <UTILITY/Stringify.h>

/*
 * DataItem - Constructor for structure DataItem
 */
SliceWidget::DataItem::DataItem(void) {
} // end DataItem()

/*
 * ~DataItem - Destructor for structure DataItem
 */
SliceWidget::DataItem::~DataItem(void) {
} // end ~DataItem()

/*
 * SliceWidget - Constructor for the SliceWidget class.
 *
 * parameter _name - const char *
 * parameter _parent - GLMotif::Container *
 * parameter volume - Volume *
 * parameter _manageChild - bool
 */
SliceWidget::SliceWidget(const char * _name, GLMotif::Container * _parent, Volume * _volume, bool _manageChild) :
    GLMotif::Widget(_name, _parent, false), directionChanged(false), selected(false), sliceChanged(false), sliceDirection(XY),
            volume(_volume) {
    marginWidth = 0.0f;
    preferredSize[0] = 0.0f;
    preferredSize[1] = 0.0f;
    preferredSize[2] = 0.0f;
    xySlice = new unsigned char[volume->getWidth() * volume->getHeight()];
    xzSlice = new unsigned char[volume->getWidth() * volume->getDepth()];
    yzSlice = new unsigned char[volume->getHeight() * volume->getDepth()];
    updateSlice(sliceDirection, volume->getDepth() / 2);
    if (_manageChild)
        manageChild();
} // end SliceWidget()

/*
 * ~SliceWidget - Destructor for the SliceWidget class.
 */
SliceWidget::~SliceWidget(void) {
    delete[] xySlice;
    delete[] xzSlice;
    delete[] yzSlice;
} // end ~SliceWidget()

/*
 * calcNaturalSize - Determine the natural size of the SliceWidget. A virtual function of GLMotif::Widget base.
 *
 * return - GLMotif::Vector
 */
GLMotif::Vector SliceWidget::calcNaturalSize(void) const {
    GLMotif::Vector result = preferredSize;
    result[0] += 2.0f * marginWidth;
    result[1] += 2.0f * marginWidth;
    return calcExteriorSize(result);
} // end calcNaturalSize()

/*
 * draw - Draw the SliceWidget.
 *
 * parameter glContextData - GLContextData&
 */
void SliceWidget::draw(GLContextData& glContextData) const {
    Widget::draw(glContextData);
    DataItem* dataItem = glContextData.retrieveDataItem<DataItem> (this);
    if (sliceChanged)
        updateSliceTexture(dataItem);
    GLfloat x1 = SliceWidgetAreaBox.getCorner(0)[0];
    GLfloat x2 = SliceWidgetAreaBox.getCorner(1)[0];
    GLfloat y1 = SliceWidgetAreaBox.getCorner(0)[1];
    GLfloat y2 = SliceWidgetAreaBox.getCorner(2)[1];
    GLfloat z = SliceWidgetAreaBox.getCorner(0)[2];
    GLfloat width = x2 - x1;
    GLfloat height = y2 - y1;
    drawMargin();
    GLboolean lightingEnabled = glIsEnabled(GL_LIGHTING);
    if (lightingEnabled)
        glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    drawSliceWidget(dataItem, x1, y1, z, width, height);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    if (lightingEnabled)
        glEnable(GL_LIGHTING);
} // end draw()

/*
 * drawSliceWidget - Draw histogram.
 *
 * parameter dataItem - DataItem *
 * parameter x - GLfloat
 * parameter y - GLfloat
 * parameter z - GLfloat
 * parameter width - GLfloat
 * parameter height - GLfloat
 */
void SliceWidget::drawSliceWidget(DataItem * dataItem, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height) const {
    GLfloat _x = 0.0f;
    GLfloat _y = 0.0f;
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    if (sliceDirection == XY)
        glBindTexture(GL_TEXTURE_2D, dataItem->xySliceName);
    else if (sliceDirection == XZ)
        glBindTexture(GL_TEXTURE_2D, dataItem->xzSliceName);
    else
        glBindTexture(GL_TEXTURE_2D, dataItem->yzSliceName);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
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
} // end drawSliceWidget()

/*
 * drawMargin - Draw margin area in background color.
 */
void SliceWidget::drawMargin(void) const {
    glColor(backgroundColor);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex(getInterior().getCorner(0));
    glVertex(SliceWidgetAreaBox.getCorner(0));
    glVertex(SliceWidgetAreaBox.getCorner(2));
    glVertex(getInterior().getCorner(2));
    glVertex(getInterior().getCorner(1));
    glVertex(getInterior().getCorner(3));
    glVertex(SliceWidgetAreaBox.getCorner(3));
    glVertex(SliceWidgetAreaBox.getCorner(1));
    glVertex(getInterior().getCorner(0));
    glVertex(getInterior().getCorner(1));
    glVertex(SliceWidgetAreaBox.getCorner(1));
    glVertex(SliceWidgetAreaBox.getCorner(0));
    glVertex(getInterior().getCorner(2));
    glVertex(SliceWidgetAreaBox.getCorner(2));
    glVertex(SliceWidgetAreaBox.getCorner(3));
    glVertex(getInterior().getCorner(3));
    glEnd();
} // end drawMargin()

/*
 * findRecipient - Determine which is the applicable widget of the event. A virtual function of GLMotif::Widget base.
 *
 * parameter event - GLMotif::Event&
 * return - bool
 */
bool SliceWidget::findRecipient(GLMotif::Event& event) {
    if (selected) {
        return event.setTargetWidget(this, event.calcWidgetPoint(this));
    } else
        return GLMotif::Widget::findRecipient(event);
} // end findRecipient()

/*
 * setDirectionChanged
 *
 * parameter directionChanged - bool
 */
void SliceWidget::setDirectionChanged(bool directionChanged) {
    this->directionChanged = directionChanged;
} // end setDirectionChanged()

/*
 * setMarginWidth - Set the margin width.
 *
 * parameter _margineWidth - GLfloat
 */
void SliceWidget::setMarginWidth(GLfloat _marginWidth) {
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
void SliceWidget::setPreferredSize(const GLMotif::Vector& _preferredSize) {
    preferredSize = _preferredSize;
    if (isManaged) {
        parent->requestResize(this, calcNaturalSize());
    } else
        resize(GLMotif::Box(GLMotif::Vector(0.0f, 0.0f, 0.0f), calcNaturalSize()));
    GLfloat x1 = SliceWidgetAreaBox.getCorner(0)[0];
    GLfloat x2 = SliceWidgetAreaBox.getCorner(1)[0];
    GLfloat y1 = SliceWidgetAreaBox.getCorner(0)[1];
    GLfloat y2 = SliceWidgetAreaBox.getCorner(2)[1];
    GLfloat z = SliceWidgetAreaBox.getCorner(0)[2];
    GLfloat width = x2 - x1;
    GLfloat height = y2 - y1;
} // end setPreferredSize()

/*
 * setSliceChanged
 *
 * parameter _sliceChanged - bool
 */
void SliceWidget::setSliceChanged(bool _sliceChanged) {
    sliceChanged = _sliceChanged;
} // end setSliceChanged()

/*
 * initContext
 *
 * parameter glContextData - GLContextData&
 */
void SliceWidget::initContext(GLContextData& glContextData) const {
    DataItem * dataItem = new DataItem();
    glContextData.addDataItem(this, dataItem);
    dataItem->textureNonPowerOfTwo = GLARBTextureNonPowerOfTwo::isSupported();
    //glGetIntegerv(GL_MAX_2D_TEXTURE_SIZE, &dataItem->maximum2DTextureSize);
    initializeSliceTexture(dataItem);
} // end initContext()

/*
 * initializeSliceTexture
 *
 * parameter dataItem - DataItem *
 */
void SliceWidget::initializeSliceTexture(DataItem * dataItem) const {
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &dataItem->xySliceName);
    glBindTexture(GL_TEXTURE_2D, dataItem->xySliceName);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, volume->getWidth(), volume->getHeight(), 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
            xySlice);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &dataItem->xzSliceName);
    glBindTexture(GL_TEXTURE_2D, dataItem->xzSliceName);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, volume->getWidth(), volume->getDepth(), 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
            xySlice);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &dataItem->yzSliceName);
    glBindTexture(GL_TEXTURE_2D, dataItem->yzSliceName);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, volume->getHeight(), volume->getDepth(), 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
            yzSlice);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
} // end initializeSliceTexture()

/*
 * pointerButtonDown - Pointer button down event handler. A virtual function of GLMotif::Widget base.
 *
 * parameter event - GLMotif::Event&
 */
void SliceWidget::pointerButtonDown(GLMotif::Event& event) {
    selected = true;
    double _x = event.getWidgetPoint().getPoint()[0];
    double _y = event.getWidgetPoint().getPoint()[1];
} // end pointerButtonDown()

/*
 * pointerButtonUp - Pointer button up event handler. A virtual function of GLMotif::Widget base.
 *
 * parameter event - GLMotif::Event&
 */
void SliceWidget::pointerButtonUp(GLMotif::Event& event) {
    selected = false;
} // end pointerButtonUp()

/*
 * pointerMotion - Pointer motion event handler. A virtual function of GLMotif::Widget base.
 *
 * parameter event - GLMotif::Event&
 */
void SliceWidget::pointerMotion(GLMotif::Event& event) {
} // pointerMotion()

/*
 * resize - Resize the SliceWidget display. A virtual function of GLMotif::Widget base.
 *
 * parameter _exterior - const GLMotif::Box&
 */
void SliceWidget::resize(const GLMotif::Box& _exterior) {
    GLMotif::Widget::resize(_exterior);
    SliceWidgetAreaBox = getInterior();
    SliceWidgetAreaBox.doInset(GLMotif::Vector(marginWidth, marginWidth, 0.0f));
} // end resize()

/*
 * updateSlice
 *
 * parameter _sliceDirection - int
 * parameter _which - int
 */
void SliceWidget::updateSlice(int _sliceDirection, int _which) {
    sliceDirection = _sliceDirection;
    which = _which;
    int width = volume->getWidth();
    int height = volume->getHeight();
    int depth = volume->getDepth();
    if (sliceDirection == XY) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                xySlice[j * width + i] = volume->getVoxel(i + j * width + which * (width * height));
            }
        }
    } else if (sliceDirection == XZ) {
        for (int j = 0; j < depth; j++) {
            for (int i = 0; i < width; i++) {
                xzSlice[j * width + i] = volume->getVoxel(i + which * width + j * (width * height));
            }
        }
    } else {
        for (int j = 0; j < depth; j++) {
            for (int i = 0; i < height; i++) {
                yzSlice[j * height + i] = volume->getVoxel(which + i * width + j * (width * height));
            }
        }
    }
} // end updateSlice()

/*
 * updateSliceTexture
 *
 * parameter dataItem - DataItem *
 */
void SliceWidget::updateSliceTexture(DataItem * dataItem) const {
    glEnable(GL_TEXTURE_2D);
    if (sliceDirection == XY) {
        glBindTexture(GL_TEXTURE_2D, dataItem->xySliceName);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, volume->getWidth(), volume->getHeight(), GL_LUMINANCE, GL_UNSIGNED_BYTE, xySlice);
    } else if (sliceDirection == XZ) {
        glBindTexture(GL_TEXTURE_2D, dataItem->xzSliceName);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, volume->getWidth(), volume->getDepth(), GL_LUMINANCE, GL_UNSIGNED_BYTE, xzSlice);
    } else {
        glBindTexture(GL_TEXTURE_2D, dataItem->yzSliceName);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, volume->getHeight(), volume->getDepth(), GL_LUMINANCE, GL_UNSIGNED_BYTE, yzSlice);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
} // end updateSliceTexture()
