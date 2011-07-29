/*
 * DownSampling.h - Class for editing the down sampling properties.
 *
 * Author: Patrick O'Leary
 * Created: October 9, 2008
 * Copyright: 2008. All rights reserved.
 */
#ifndef DOWNSAMPLE_INCLUDED
#define DOWNSAMPLE_INCLUDED

/* Vrui includes */
#include <GLMotif/PopupWindow.h>
#include <GLMotif/RowColumn.h>
#include <GLMotif/Slider.h>
#include <GLMotif/StyleSheet.h>
#include <GLMotif/TextField.h>
#include <Misc/CallbackData.h>

// begin Forward Declarations
class Scene;
class Toirt_Samhlaigh;
// end Forward Declarations

class DownSampling: public GLMotif::PopupWindow {
public:
    DownSampling(Scene * _scene, Toirt_Samhlaigh & _toirt_samhlaigh, int _stepMaximum);
    ~DownSampling(void);
private:
    Scene * scene;
    float slicingScale;
    GLMotif::Slider * slicingScaleSlider;
    GLMotif::TextField * slicingScaleTextField;
    int stepMaximum;
    Toirt_Samhlaigh & toirt_samhlaigh;
    int xStep;
    GLMotif::Slider * xStepSlider;
    GLMotif::TextField * xStepTextField;
    int yStep;
    GLMotif::Slider * yStepSlider;
    GLMotif::TextField * yStepTextField;
    int zStep;
    GLMotif::Slider * zStepSlider;
    GLMotif::TextField * zStepTextField;
    void createSlicingScaleSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& downSamplingDialog);
    void createXStepSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& downSamplingDialog);
    void createYStepSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& downSamplingDialog);
    void createZStepSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& downSamplingDialog);
    void initialize(void);
    void slicingScaleSliderCallback(Misc::CallbackData* callbackData);
    void xStepSliderCallback(Misc::CallbackData* callbackData);
    void yStepSliderCallback(Misc::CallbackData* callbackData);
    void zStepSliderCallback(Misc::CallbackData* callbackData);
};

#endif
