/*
 * Materials.cpp - Methods for Materials class.
 *
 * Author: Patrick O'Leary
 * Created: September 30, 2008
 * Copyright: 2008. All rights reserved.
 */
#include <iostream>

/* Vrui includes to use the Vrui interface */
#include <Vrui/Vrui.h>

/* Vrui includes */
#include <GLMotif/Label.h>
#include <GLMotif/WidgetManager.h>

#include <GRAPHIC/Scene.h>
#include <GUI/Materials.h>
#include <Toirt_Samhlaigh.h>

/*
 * Materials - Constructor for Materials class.
 * 		extends GLMotif::PopupWindow
 */
Materials::Materials(Scene * _scene, Toirt_Samhlaigh & _toirt_samhlaigh) :
    GLMotif::PopupWindow("MaterialsPopup", Vrui::getWidgetManager(), "Materials"), ambient(true), diffuse(false), scene(_scene),
            specular(false), toirt_samhlaigh(_toirt_samhlaigh) {
    initialize();
}

/*
 * ~Materials - Destructor for Materials class.
 */
Materials::~Materials(void) {
}

/*
 * ambientCallback
 *
 * parameter callBackData - GLMotif::ToggleButton::ValueChangedCallbackData*
 */
void Materials::ambientCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData) {
    if (strcmp(callBackData->toggle->getName(), "AmbientButton") == 0) {
        ambientButton->setToggle(true);
        diffuseButton->setToggle(false);
        specularButton->setToggle(false);
        ambient = true;
        diffuse = false;
        specular = false;
        for (int i = 0; i < 3; ++i) {
            propertySliders[i]->setValue(ambientProperty[i]);
            propertyTextField[i]->setValue(ambientProperty[i]);
        }
        scene->setKAmbient(ambientProperty[0], ambientProperty[1], ambientProperty[2]);
    }
    Vrui::requestUpdate();
} // end ambientCallback()

/*
 * propertySliderCallback - Callback of change to color slider value.
 *
 * parameter callbackData - Misc::CallbackData *
 */
void Materials::propertySliderCallback(Misc::CallbackData* callbackData) {
    for (int i = 0; i < 3; ++i) {
        if (ambient) {
            ambientProperty[i] = float(propertySliders[i]->getValue());
            propertyTextField[i]->setValue(ambientProperty[i]);
        } else if (diffuse) {
            diffuseProperty[i] = float(propertySliders[i]->getValue());
            propertyTextField[i]->setValue(diffuseProperty[i]);
        } else if (specular) {
            specularProperty[i] = float(propertySliders[i]->getValue());
            propertyTextField[i]->setValue(specularProperty[i]);
        }
    }
    if (ambient)
        scene->setKAmbient(ambientProperty[0], ambientProperty[1], ambientProperty[2]);
    else if (diffuse)
        scene->setKDiffuse(diffuseProperty[0], diffuseProperty[1], diffuseProperty[2]);
    else if (specular)
        scene->setKSpecular(specularProperty[0], specularProperty[1], specularProperty[2]);
} // end propertySliderCallback()

/*
 * createPropertyEditor - Create color editor.
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter transferFunction2_5DDialog - GLMotif::RowColumn*&
 * return - GLMotif::RowColumn*
 */
GLMotif::RowColumn* Materials::createPropertyEditor(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& materialsDialog) {
    GLMotif::RowColumn* propertyEditor = new GLMotif::RowColumn("PropertyEditor", materialsDialog, false);
    propertyEditor->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    GLMotif::RowColumn* propertySlidersBox = createPropertySliderBox(styleSheet, propertyEditor);
    propertySlidersBox->manageChild();
    return propertyEditor;
} // end createPropertyEditor()

/*
 * createPropertySlider - Create color slider.
 *
 * parameter title - const char*
 * parameter color - GLMotif::Color
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter propertySliderBox - GLMotif::RowColumn*
 */
void Materials::createPropertySlider(const char* title, GLMotif::Color color, const GLMotif::StyleSheet& styleSheet,
        GLMotif::RowColumn* propertySlidersBox, int which) {
    GLMotif::RowColumn* sliderBox = new GLMotif::RowColumn("SliderBox", propertySlidersBox, false);
    sliderBox->setOrientation(GLMotif::RowColumn::VERTICAL);
    sliderBox->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    GLMotif::Slider * propertySlider = new GLMotif::Slider(title, sliderBox, GLMotif::Slider::VERTICAL, styleSheet.fontHeight
            * 5.0f);
    propertySlider->setSliderColor(color);
    propertySlider->setValueRange(0.0f, 1.0f, 0.01f);
    propertySlider->setValue(0.1f);
    propertySlider->getValueChangedCallbacks().add(this, &Materials::propertySliderCallback);
    propertySliders[which] = propertySlider;
    propertyTextField[which] = new GLMotif::TextField("PropertyTextField", sliderBox, 6);
    propertyTextField[which]->setFieldWidth(4);
    propertyTextField[which]->setPrecision(3);
    propertyTextField[which]->setValue(0.1f);
    sliderBox->manageChild();
 } // end createPropertySlider()

/*
 * createPropertySliderBox - Create box to contain color slider bars.
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter propertyEditor - GLMotif::RowColumn*
 * return - GLMotif::RowColumn*
 */
GLMotif::RowColumn* Materials::createPropertySliderBox(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn* propertyEditor) {
    GLMotif::RowColumn* propertySlidersBox = new GLMotif::RowColumn("PropertySliders", propertyEditor, false);
    propertySlidersBox->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    propertySlidersBox->setPacking(GLMotif::RowColumn::PACK_GRID);
    createPropertySliders(styleSheet, propertySlidersBox);
    return propertySlidersBox;
} // end createPropertySliderBox()

/*
 * createPropertySliders - Create RGB color sliders.
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter propertySliderBox - GLMotif::RowColumn*
 */
void Materials::createPropertySliders(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn* propertySlidersBox) {
    createPropertySlider("RedSlider", GLMotif::Color(1.0f, 0.0f, 0.0f), styleSheet, propertySlidersBox, 0);
    createPropertySlider("GreenSlider", GLMotif::Color(0.0f, 1.0f, 0.0f), styleSheet, propertySlidersBox, 1);
    createPropertySlider("BlueSlider", GLMotif::Color(0.0f, 0.0f, 1.0f), styleSheet, propertySlidersBox, 2);
} // end createPropertySliders()

/*
 * createMaterialProperties
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter materialsDialog - GLMotif::RowColumn*
 */
void Materials::createMaterialProperties(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn * materialsDialog) {
    GLMotif::RowColumn * lightPropertiesRowColumn = new GLMotif::RowColumn("MaterialPropertiesRowColumn", materialsDialog, false);
    lightPropertiesRowColumn->setOrientation(GLMotif::RowColumn::VERTICAL);
    lightPropertiesRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    GLMotif::RowColumn * colorRowColumn = new GLMotif::RowColumn("ColorRowColumn", lightPropertiesRowColumn, false);
    colorRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    colorRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    ambientButton = new GLMotif::ToggleButton("AmbientButton", colorRowColumn, "Ambient");
    ambientButton->setToggle(true);
    ambientButton->getSelectCallbacks().add(this, &Materials::ambientCallback);
    diffuseButton = new GLMotif::ToggleButton("DiffuseButton", colorRowColumn, "Diffuse");
    diffuseButton->setToggle(false);
    diffuseButton->getSelectCallbacks().add(this, &Materials::diffuseCallback);
    specularButton = new GLMotif::ToggleButton("SpecularButton", colorRowColumn, "Specular");
    specularButton->setToggle(false);
    specularButton->getSelectCallbacks().add(this, &Materials::specularCallback);
    colorRowColumn->manageChild();
    //shininess slider
    lightPropertiesRowColumn->manageChild();

} // end createMaterialProperties()

/*
 * createShininessSlider
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter materialsDialog - GLMotif::RowColumn*&
 */
void Materials::createShininessSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& materialsDialog) {
    GLMotif::RowColumn* shininessSliderRowColumn = new GLMotif::RowColumn("ShininessSliderRowColumn", materialsDialog, false);
    shininessSliderRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    shininessSliderRowColumn->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    GLMotif::Label * shininessLabel = new GLMotif::Label("ShininessLabel", shininessSliderRowColumn, "Shininess");
    shininessLabel->setLabel("Shininess");
    shininessTextField = new GLMotif::TextField("ShininessTextField", shininessSliderRowColumn, 6);
    shininessTextField->setFieldWidth(4);
    shininessTextField->setPrecision(3);
    shininessTextField->setValue(100.0f);
    shininessSlider = new GLMotif::Slider("ShininessSlider", shininessSliderRowColumn, GLMotif::Slider::HORIZONTAL, styleSheet.fontHeight
            * 5.0f);
    shininessSlider->setSliderColor(GLMotif::Color(0.5f, 0.5f, 0.5f));
    shininessSlider->setValueRange(0.0f, 128.0f, 1.0f);
    shininessSlider->setValue(100.0f);
    shininessSlider->getValueChangedCallbacks().add(this, &Materials::shininessSliderCallback);
    shininessSliderRowColumn->manageChild();
} // end createShininessSlider()

/*
 * diffuseCallback
 *
 * parameter callBackData - GLMotif::ToggleButton::ValueChangedCallbackData*
 */
void Materials::diffuseCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData) {
    if (strcmp(callBackData->toggle->getName(), "DiffuseButton") == 0) {
        ambientButton->setToggle(false);
        diffuseButton->setToggle(true);
        specularButton->setToggle(false);
        ambient = false;
        diffuse = true;
        specular = false;
        for (int i = 0; i < 3; ++i) {
            propertySliders[i]->setValue(diffuseProperty[i]);
            propertyTextField[i]->setValue(diffuseProperty[i]);
        }
        scene->setKDiffuse(diffuseProperty[0], diffuseProperty[1], diffuseProperty[2]);
    }
    Vrui::requestUpdate();
} // end diffuseCallback()

/*
 * initialize - Initialize the GUI for the Materials class.
 */
void Materials::initialize(void) {
    ambientProperty[0] = 0.1f;
    ambientProperty[1] = 0.1f;
    ambientProperty[2] = 0.1f;
    diffuseProperty[0] = 0.6f;
    diffuseProperty[1] = 0.6f;
    diffuseProperty[2] = 0.6f;
    specularProperty[0] = 0.2f;
    specularProperty[1] = 0.2f;
    specularProperty[2] = 0.2f;
    shininess = 100.0f;
    const GLMotif::StyleSheet& styleSheet = *Vrui::getWidgetManager()->getStyleSheet();
    GLMotif::RowColumn* materialsDialog = new GLMotif::RowColumn("MaterialsDialog", this, false);
    createMaterialProperties(styleSheet, materialsDialog);
    GLMotif::RowColumn* propertyEditor = createPropertyEditor(styleSheet, materialsDialog);
    propertyEditor->manageChild();
    createShininessSlider(styleSheet, materialsDialog);
    materialsDialog->manageChild();
}

/*
 * shininessSliderCallback - Callback of change to shininess slider value.
 *
 * parameter callbackData - Misc::CallbackData*
 */
void Materials::shininessSliderCallback(Misc::CallbackData* callbackData) {
    shininess = float(shininessSlider->getValue());
    shininessTextField->setValue(shininess);
    scene->setShininess(shininess);
} // end shininessSliderCallback()

/*
 * specularCallback
 *
 * parameter callBackData - GLMotif::ToggleButton::ValueChangedCallbackData*
 */
void Materials::specularCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData) {
    if (strcmp(callBackData->toggle->getName(), "SpecularButton") == 0) {
        ambientButton->setToggle(false);
        diffuseButton->setToggle(false);
        specularButton->setToggle(true);
        ambient = false;
        diffuse = false;
        specular = true;
        for (int i = 0; i < 3; ++i) {
            propertySliders[i]->setValue(specularProperty[i]);
            propertyTextField[i]->setValue(specularProperty[i]);
        }
        scene->setKSpecular(specularProperty[0], specularProperty[1], specularProperty[2]);
    }
    Vrui::requestUpdate();
} // end specularCallback()
