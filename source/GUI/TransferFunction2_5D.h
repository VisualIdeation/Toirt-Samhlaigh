/*
 * TransferFunction2_5D.h - Class for editing 2.5D transfer function.
 *
 * Author: Patrick O'Leary
 * Created: August 14, 2008
 * Copyright: 2008. All rights reserved.
 */
#ifndef TRASFERFUNCTION2_5D_INCLUDED
#define TRASFERFUNCTION2_5D_INCLUDED

#include <GL/gl.h>

/* Vrui includes */
#include <GL/GLColorMap.h>
#include <GLMotif/Blind.h>
#include <GLMotif/PopupWindow.h>
#include <GLMotif/RadioBox.h>
#include <GLMotif/RowColumn.h>
#include <GLMotif/Slider.h>
#include <GLMotif/StyleSheet.h>
#include <GLMotif/TextField.h>
#include <Misc/CallbackData.h>
#include <Misc/CallbackList.h>

#define XY 0
#define XZ 1
#define YZ 2

// begin Forward Declarations
class HistogramWidget;
class SliceWidget;
class SwatchesWidget;
class Toirt_Samhlaigh;
class Volume;
// end Forward Declarations

class TransferFunction2_5D: public GLMotif::PopupWindow {
public:
    TransferFunction2_5D(Volume* volume, Toirt_Samhlaigh & _toirt_samhlaigh);
    virtual ~TransferFunction2_5D(void);
    void exportColorMap(unsigned char* colormap) const;
    Misc::CallbackList& getHistogramWidgetChangedCallbacks(void);
    bool isDragging(void) const;
    bool isInteractive(void);
    void setInteractive(bool interactive);
    void setSliceChanged(bool sliceChanged);
    void loadTransferFunction2_5D(const char* fileName);
    void quadrilateralChangedCallback(Misc::CallbackData * _callbackData);
    void removeQuadrilateralCallback(Misc::CallbackData * _callbackData);
    void saveTransferFunction2_5D(const char* fileName) const;
private:
    GLMotif::Slider* alphaSlider;
    GLMotif::TextField * alphaTextField;
    GLMotif::Blind* colorPane;
    GLMotif::Slider* colorSliders[3];
    HistogramWidget * histogramWidget;
    bool interactive;
    GLMotif::ToggleButton* interactiveToggleButton;
    SliceWidget * sliceWidget;
    int sliceDirection;
    GLMotif::Slider * sliceSlider;
    GLMotif::TextField * sliceTextField;
    SwatchesWidget * swatchesWidget;
    Toirt_Samhlaigh & toirt_samhlaigh;
    Volume * volume;
    GLMotif::ToggleButton * xyToggle;
    GLMotif::ToggleButton * xzToggle;
    GLMotif::ToggleButton * yzToggle;
    void colorMapChangedCallback(Misc::CallbackData* _callbackData);
    void alphaSliderCallback(Misc::CallbackData* _callbackData);
    void colorSliderCallback(Misc::CallbackData* _callbackData);
    void colorSwatchesWidgetCallback(Misc::CallbackData* _callbackData);
    void createAlphaSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& transferFunction2_5DDialog);
    GLMotif::RowColumn * createButtonBox(GLMotif::RowColumn * & transferFunction2_5DDialog);
    GLMotif::RowColumn * createColorEditor(const GLMotif::StyleSheet & styleSheet,
            GLMotif::RowColumn * & transferFunction2_5DDialog);
    GLMotif::Slider* createColorSlider(const char * title, GLMotif::Color color, const GLMotif::StyleSheet & styleSheet,
            GLMotif::RowColumn * colorSlidersBox);
    GLMotif::RowColumn* createColorSliderBox(const GLMotif::StyleSheet & styleSheet, GLMotif::RowColumn * colorEditor);
    void createColorSliders(const GLMotif::StyleSheet & styleSheet, GLMotif::RowColumn * colorEditor);
    void createColorSwatchesWidget(const GLMotif::StyleSheet & styleSheet, GLMotif::RowColumn * & colorEditor);
    void createHistogramWidget(const GLMotif::StyleSheet & styleSheet, GLMotif::RowColumn * & sliceAndHistogramRowColumn);
    void createSliceAndHistogramRowColumn(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn * transferFunction2_5DDialog);
    void createSliceWidget(const GLMotif::StyleSheet & styleSheet, GLMotif::RowColumn * & sliceAndHistogramRowColumn);
    void createSliceDirection(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn * sliceRowColumn);
    void createSliceRowColumn(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn * transferFunction2_5DDialog);
    void createSliceSlider(const GLMotif::StyleSheet & styleSheet, GLMotif::RowColumn * sliceRowColumn);
    void initialize(void);
    void interactiveToggleButtonCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
    void sliceSliderCallback(Misc::CallbackData * _callbackData);
    void toggleSelectCallback(GLMotif::ToggleButton::ValueChangedCallbackData * callbackData);
};

#endif
