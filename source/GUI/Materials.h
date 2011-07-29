/*
 * Materials.h - Class for editing the material properties.
 *
 * Author: Patrick O'Leary
 * Created: September 30, 2008
 * Copyright: 2008. All rights reserved.
 */
#ifndef MATERIALS_INCLUDED
#define MATERIALS_INCLUDED

/* Vrui includes */
#include <GLMotif/PopupWindow.h>
#include <GLMotif/RowColumn.h>
#include <GLMotif/Slider.h>
#include <GLMotif/StyleSheet.h>
#include <GLMotif/TextField.h>
#include <GLMotif/ToggleButton.h>
#include <Misc/CallbackData.h>

// begin Forward Declarations
class Scene;
class Toirt_Samhlaigh;
// end Forward Declarations

class Materials: public GLMotif::PopupWindow {
public:
    Materials(Scene * _scene, Toirt_Samhlaigh & _toirt_samhlaigh);
    ~Materials(void);
private:
    bool ambient;
    GLMotif::ToggleButton * ambientButton;
    float ambientProperty[3];
    bool diffuse;
    GLMotif::ToggleButton * diffuseButton;
    float diffuseProperty[3];
    GLMotif::Slider * propertySliders[3];
    GLMotif::TextField * propertyTextField[3];
    Scene * scene;
    float shininess;
    GLMotif::Slider * shininessSlider;
    GLMotif::TextField * shininessTextField;
    bool specular;
    GLMotif::ToggleButton * specularButton;
    float specularProperty[3];
    Toirt_Samhlaigh & toirt_samhlaigh;
    void ambientCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
    void propertySliderCallback(Misc::CallbackData* callbackData);
    GLMotif::RowColumn* createPropertyEditor(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& materialsDialog);
    void createPropertySlider(const char* title, GLMotif::Color color, const GLMotif::StyleSheet& styleSheet,
            GLMotif::RowColumn* propertySlidersBox, int which);
    GLMotif::RowColumn* createPropertySliderBox(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn* propertyEditor);
    void createPropertySliders(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn* propertySlidersBox);
    void createMaterialProperties(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn * materialsDialog);
    void createShininessSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& materialsDialog);
    void diffuseCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
    void initialize(void);
    void shininessSliderCallback(Misc::CallbackData* callbackData);
    void specularCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
};

#endif
