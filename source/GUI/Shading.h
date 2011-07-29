/*
 * Shading.h - Class for editing the shading.
 *
 * Author: Patrick O'Leary
 * Created: October 1, 2008
 * Copyright: 2008. All rights reserved.
 */
#ifndef SHADING_INCLUDED
#define SHADING_INCLUDED

/* Vrui includes */
#include <GLMotif/Blind.h>
#include <GLMotif/PopupWindow.h>
#include <GLMotif/RowColumn.h>
#include <GLMotif/Slider.h>
#include <GLMotif/StyleSheet.h>
#include <GLMotif/TextField.h>
#include <GLMotif/ToggleButton.h>
#include <Misc/CallbackData.h>

// begin Forward Declarations
class RGBAColor;
class Scene;
class SwatchesWidget;
class Toirt_Samhlaigh;
// end Forward Declarations

class Shading: public GLMotif::PopupWindow {
public:
    Shading(Scene * _scene, Toirt_Samhlaigh & _toirt_samhlaigh);
    ~Shading(void);
private:
    float normalContribution;
    GLMotif::TextField * normalContributionTextField;
    GLMotif::Slider * normalContributionSlider;
    bool lighting;
    GLMotif::ToggleButton * lightingButton;
    bool preintegration;
    GLMotif::ToggleButton * preintegrationButton;
    GLMotif::Blind* colorPane;
    GLMotif::Slider* colorSliders[3];
    Scene * scene;
    float toneContribution;
    GLMotif::TextField * toneContributionTextField;
    GLMotif::Slider * toneContributionSlider;
    bool coolSelected;
    bool warmSelected;
    GLMotif::ToggleButton * warmSelectedButton;
    GLMotif::ToggleButton * coolSelectedButton;
    RGBAColor * warmColor;
    RGBAColor * coolColor;
    GLMotif::Blind* warmPane;
    GLMotif::Blind* coolPane;
    float toonContribution;
    GLMotif::TextField * toonContributionTextField;
    GLMotif::Slider * toonContributionSlider;
    bool toonSelected;
    GLMotif::ToggleButton * toonSelectedButton;
    RGBAColor * toonColor;
    GLMotif::Blind* toonPane;
    SwatchesWidget * swatchesWidget;
    Toirt_Samhlaigh & toirt_samhlaigh;
    void lightingCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
    void preintegrationCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
    void colorSliderCallback(Misc::CallbackData* callbackData);
    void colorSwatchesWidgetCallback(Misc::CallbackData* callbackData);
    void coolSelectedCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
    GLMotif::RowColumn* createColorEditor(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& lightingDialog);
    void createColorSwatchesWidget(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& colorEditor);
    GLMotif::Slider* createColorSlider(const char* title, GLMotif::Color color, const GLMotif::StyleSheet& styleSheet,
            GLMotif::RowColumn* colorSlidersBox);
    GLMotif::RowColumn* createColorSliderBox(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn* colorEditor);
    void createColorSliders(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn* colorSlidersBox);
    void createNormalShading(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn * lightingDialog);
    void createNormalContributionSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& normalShadingRowColumn);
    void createToneShading(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn * shadingDialog);
    void createToneContributionSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& toneShadingRowColumn);
    void createToonShading(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn * shadingDialog);
    void createToonContributionSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& toonShadingRowColumn);
    void initialize(void);
    void normalContributionSliderCallback(Misc::CallbackData* callbackData);
    void toneContributionSliderCallback(Misc::CallbackData* callbackData);
    void toonSelectedCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
    void toonContributionSliderCallback(Misc::CallbackData* callbackData);
    void warmSelectedCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
};

#endif // SHADING_INCLUDED
