/*
 * Lighting.h - Class for editing the lighting.
 *
 * Author: Patrick O'Leary
 * Created: September 25, 2008
 * Copyright: 2008. All rights reserved.
 */
#ifndef LIGHTING_INCLUDED
#define LIGHTING_INCLUDED

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
class LightWidget;
class RGBAColor;
class Scene;
class SwatchesWidget;
class Toirt_Samhlaigh;
// end Forward Declarations

class Lighting: public GLMotif::PopupWindow {
public:
    Lighting(Scene * _scene, Toirt_Samhlaigh & _toirt_samhlaigh, float * extent, float * origin);
    ~Lighting(void);
private:
    bool ambient;
    GLMotif::ToggleButton * ambientButton;
    RGBAColor * ambientColor;
    GLMotif::Blind* ambientPane;
    GLMotif::Blind* colorPane;
    GLMotif::Slider* colorSliders[3];
    bool diffuse;
    GLMotif::ToggleButton * diffuseButton;
    RGBAColor * diffuseColor;
    GLMotif::Blind* diffusePane;
    float * extent;
    LightWidget * lightWidget;
    float * origin;
    Scene * scene;
    bool specular;
    GLMotif::ToggleButton * specularButton;
    RGBAColor * specularColor;
    GLMotif::Blind* specularPane;
    SwatchesWidget * swatchesWidget;
    Toirt_Samhlaigh & toirt_samhlaigh;
    void ambientCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
    void colorSliderCallback(Misc::CallbackData* callbackData);
    void colorSwatchesWidgetCallback(Misc::CallbackData* callbackData);
    GLMotif::RowColumn* createColorEditor(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& lightingDialog);
    void createColorSwatchesWidget(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& colorEditor);
    GLMotif::Slider* createColorSlider(const char* title, GLMotif::Color color, const GLMotif::StyleSheet& styleSheet,
            GLMotif::RowColumn* colorSlidersBox);
    GLMotif::RowColumn* createColorSliderBox(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn* colorEditor);
    void createColorSliders(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn* colorSlidersBox);
    void createLightWidgetRowColumn(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn * lightingDialog);
    void createLightWidget(const GLMotif::StyleSheet & styleSheet, GLMotif::RowColumn * & lightWidgetRowColumn);
    void createLightProperties(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn * lightingDialog);
    void diffuseCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
    void initialize(void);
    void specularCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
};

#endif
