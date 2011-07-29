/*
 * Shading.cpp - Methods for Shading class.
 *
 * Author: Patrick O'Leary
 * Created: October 1, 2008
 * Copyright: 2008. All rights reserved.
 */
#include <iostream>

/* Vrui includes to use the Vrui interface */
#include <Vrui/Vrui.h>

/* Vrui includes */
#include <GL/GLColor.h>
#include <GLMotif/Label.h>
#include <GLMotif/Separator.h>
#include <GLMotif/WidgetManager.h>

#include <COLORMAP/RGBAColor.h>
#include <GRAPHIC/Scene.h>
#include <GUI/Shading.h>
#include <GUI/LightWidget.h>
#include <GUI/SwatchesWidget.h>
#include <Toirt_Samhlaigh.h>

/*
 * Shading - Constructor for Shading class.
 * 		extends GLMotif::PopupWindow
 */
Shading::Shading(Scene * _scene, Toirt_Samhlaigh & _toirt_samhlaigh) :
    GLMotif::PopupWindow("ShadingPopup", Vrui::getWidgetManager(), "Shading"), coolSelected(false), lighting(false),
            preintegration(false), scene(_scene), toirt_samhlaigh(_toirt_samhlaigh), toneContribution(0.0), toonSelected(false),
            warmSelected(false) {
    initialize();
}

/*
 * ~Shading - Destructor for Shading class.
 */
Shading::~Shading(void) {
}

/*
 * toonSelectedCallback
 *
 * parameter callBackData - GLMotif::ToggleButton::ValueChangedCallbackData*
 */
void Shading::toonSelectedCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData) {
    if (strcmp(callBackData->toggle->getName(), "ToonSelectedButton") == 0) {
        coolSelectedButton->setToggle(false);
        warmSelectedButton->setToggle(false);
        toonSelectedButton->setToggle(true);
        warmSelected = false;
        coolSelected = false;
        toonSelected = true;
        float * _color = toonColor->getValues();
        for (int i = 0; i < 3; ++i) {
            colorSliders[i]->setValue(_color[i]);
        }
        colorPane->setBackgroundColor(_color);
        toonPane->setBackgroundColor(_color);
    }
    Vrui::requestUpdate();
} // end toonSelectedCallback()

/*
 * lightingCallback
 *
 * parameter callBackData - GLMotif::ToggleButton::ValueChangedCallbackData*
 */
void Shading::lightingCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData) {
    if (strcmp(callBackData->toggle->getName(), "LightingButton") == 0) {
        lighting = !lighting;
        scene->setLighting(lighting);
    }
    Vrui::requestUpdate();
} // end lightingCallback()

/*
 * preintegrationCallback
 *
 * parameter callBackData - GLMotif::ToggleButton::ValueChangedCallbackData*
 */
void Shading::preintegrationCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData) {
    if (strcmp(callBackData->toggle->getName(), "PreintegrationButton") == 0) {
        if (scene->getDimension() == 1) {
            preintegration = !preintegration;
            scene->setPreintegrated(preintegration);
            if (preintegration)
                toirt_samhlaigh.setPreintegrationColormap();
        } else {
            preintegrationButton->setToggle(false);
            preintegration = false;
        }
    }
    Vrui::requestUpdate();
} // end preintegrationCallback()

/*
 * colorSliderCallback - Callback of change to color slider value.
 *
 * parameter callbackData - Misc::CallbackData*
 */
void Shading::colorSliderCallback(Misc::CallbackData* callbackData) {
    RGBAColor* rgbaColor = new RGBAColor(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 3; ++i) {
        rgbaColor->setValues(i, float(colorSliders[i]->getValue()));
        if (coolSelected)
            coolColor->setValues(i, colorSliders[i]->getValue());
        else if (warmSelected)
            warmColor->setValues(i, colorSliders[i]->getValue());
        else if (toonSelected)
            toonColor->setValues(i, colorSliders[i]->getValue());
    }
    rgbaColor->setValues(3, 0.1f);
    float * _color = rgbaColor->getValues();
    colorPane->setBackgroundColor(_color);
    if (coolSelected) {
        coolPane->setBackgroundColor(_color);
        scene->setCoolColor(_color[0], _color[1], _color[2]);
    } else if (warmSelected) {
        warmPane->setBackgroundColor(_color);
        scene->setWarmColor(_color[0], _color[1], _color[2]);
    } else if (toonSelected) {
        toonPane->setBackgroundColor(_color);
        scene->setToonColor(_color[0], _color[1], _color[2]);
    }
} // end colorSliderCallback()

/*
 * colorSwatchesWidgetCallback
 *
 * parameter callbackData - Misc::CallbackData*
 */
void Shading::colorSwatchesWidgetCallback(Misc::CallbackData* callbackData) {
    float * _color = swatchesWidget->getCurrentColor();
    for (int i = 0; i < 3; ++i) {
        colorSliders[i]->setValue(_color[i]);
        if (coolSelected)
            coolColor->setValues(i, _color[i]);
        else if (warmSelected)
            warmColor->setValues(i, _color[i]);
        else if (toonSelected)
            toonColor->setValues(i, _color[i]);
    }
    colorPane->setBackgroundColor(_color);
    if (coolSelected) {
        coolPane->setBackgroundColor(_color);
        scene->setCoolColor(_color[0], _color[1], _color[2]);
    } else if (warmSelected) {
        warmPane->setBackgroundColor(_color);
        scene->setWarmColor(_color[0], _color[1], _color[2]);
    } else if (toonSelected) {
        toonPane->setBackgroundColor(_color);
        scene->setToonColor(_color[0], _color[1], _color[2]);
    }
} // end colorSwatchesWidgetCallback()

/*
 * coolSelectedCallback
 *
 * parameter callBackData - GLMotif::ToggleButton::ValueChangedCallbackData*
 */
void Shading::coolSelectedCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData) {
    if (strcmp(callBackData->toggle->getName(), "CoolSelectedButton") == 0) {
        coolSelectedButton->setToggle(true);
        warmSelectedButton->setToggle(false);
        toonSelectedButton->setToggle(false);
        toonSelected = false;
        warmSelected = false;
        coolSelected = true;
        float * _color = coolColor->getValues();
        for (int i = 0; i < 3; ++i) {
            colorSliders[i]->setValue(_color[i]);
        }
        colorPane->setBackgroundColor(_color);
        coolPane->setBackgroundColor(_color);
    }
    Vrui::requestUpdate();
} // end coolSelectedCallback()

/*
 * createColorEditor - Create color editor.
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter transferFunction2_5DDialog - GLMotif::RowColumn*&
 * return - GLMotif::RowColumn*
 */
GLMotif::RowColumn* Shading::createColorEditor(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& shadingDialog) {
    GLMotif::RowColumn* colorEditor = new GLMotif::RowColumn("ColorEditor", shadingDialog, false);
    colorEditor->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    createColorSwatchesWidget(styleSheet, colorEditor);
    GLMotif::RowColumn* colorSlidersBox = createColorSliderBox(styleSheet, colorEditor);
    colorSlidersBox->manageChild();
    colorPane = new GLMotif::Blind("ColorPane", colorEditor);
    colorPane->setBorderWidth(styleSheet.size * 0.5f);
    colorPane->setBorderType(GLMotif::Widget::LOWERED);
    colorPane->setBackgroundColor(GLMotif::Color(0.5f, 0.5f, 0.5f));
    colorPane->setPreferredSize(GLMotif::Vector(styleSheet.fontHeight * 5.0f, styleSheet.fontHeight * 5.0f, 0.0f));
    return colorEditor;
} // end createColorEditor()

/*
 * createColorSwatchesWidget
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter colorEditor - GLMotif::RowColumn*&
 */
void Shading::createColorSwatchesWidget(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& colorEditor) {
    swatchesWidget = new SwatchesWidget("SwatchesWidget", colorEditor);
    swatchesWidget->setBorderWidth(styleSheet.size * 0.5f);
    swatchesWidget->setBorderType(GLMotif::Widget::LOWERED);
    swatchesWidget->setForegroundColor(GLMotif::Color(0.0f, 1.0f, 0.0f));
    swatchesWidget->setMarginWidth(styleSheet.size);
    swatchesWidget->setPreferredSize(GLMotif::Vector(styleSheet.fontHeight * 15.0, styleSheet.fontHeight * 5.0, 0.0f));
    swatchesWidget->getColorChangedCallbacks().add(this, &Shading::colorSwatchesWidgetCallback);
} // end createColorSwatchesWidget()

/*
 * createColorSlider - Create color slider.
 *
 * parameter title - const char*
 * parameter color - GLMotif::Color
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter colorSliderBox - GLMotif::RowColumn*
 * return - GLMotif::Slider*
 */
GLMotif::Slider* Shading::createColorSlider(const char* title, GLMotif::Color color, const GLMotif::StyleSheet& styleSheet,
        GLMotif::RowColumn* colorSlidersBox) {
    GLMotif::Slider* colorSlider = new GLMotif::Slider(title, colorSlidersBox, GLMotif::Slider::VERTICAL, styleSheet.fontHeight
            * 5.0f);
    colorSlider->setSliderColor(color);
    colorSlider->setValueRange(0.0f, 1.0f, 0.01f);
    colorSlider->setValue(0.5f);
    colorSlider->getValueChangedCallbacks().add(this, &Shading::colorSliderCallback);
    return colorSlider;
} // end createColorSlider()

/*
 * createColorSliderBox - Create box to contain color slider bars.
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter colorEditor - GLMotif::RowColumn*
 * return - GLMotif::RowColumn*
 */
GLMotif::RowColumn* Shading::createColorSliderBox(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn* colorEditor) {
    GLMotif::RowColumn* colorSlidersBox = new GLMotif::RowColumn("ColorSliders", colorEditor, false);
    colorSlidersBox->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    colorSlidersBox->setPacking(GLMotif::RowColumn::PACK_GRID);
    createColorSliders(styleSheet, colorSlidersBox);
    return colorSlidersBox;
} // end createColorSliderBox()

/*
 * createColorSliders - Create RGB color sliders.
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter colorSliderBox - GLMotif::RowColumn*
 */
void Shading::createColorSliders(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn* colorSlidersBox) {
    colorSliders[0] = createColorSlider("RedSlider", GLMotif::Color(1.0f, 0.0f, 0.0f), styleSheet, colorSlidersBox);
    colorSliders[1] = createColorSlider("GreenSlider", GLMotif::Color(0.0f, 1.0f, 0.0f), styleSheet, colorSlidersBox);
    colorSliders[2] = createColorSlider("BlueSlider", GLMotif::Color(0.0f, 0.0f, 1.0f), styleSheet, colorSlidersBox);
} // end createColorSliders()

/*
 * createNormalShading
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter shadingDialog - GLMotif::RowColumn*
 */
void Shading::createNormalShading(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn * shadingDialog) {
    GLMotif::RowColumn * normalShadingRowColumn = new GLMotif::RowColumn("NormalShadingRowColumn", shadingDialog, false);
    normalShadingRowColumn->setOrientation(GLMotif::RowColumn::VERTICAL);
    normalShadingRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    normalShadingRowColumn->setBorderType(GLMotif::Widget::LOWERED);
    createNormalContributionSlider(styleSheet, normalShadingRowColumn);
    GLMotif::RowColumn * lightingAndPreintegrationRowColumn =
            new GLMotif::RowColumn("LightingAndPreintegrationRowColumn", normalShadingRowColumn, false);
    lightingAndPreintegrationRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    lightingAndPreintegrationRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    lightingButton = new GLMotif::ToggleButton("LightingButton", lightingAndPreintegrationRowColumn, "Lighting");
    lightingButton->setToggle(false);
    lightingButton->getSelectCallbacks().add(this, &Shading::lightingCallback);
    new GLMotif::Blind("", lightingAndPreintegrationRowColumn);
    preintegrationButton = new GLMotif::ToggleButton("PreintegrationButton", lightingAndPreintegrationRowColumn, "PreIntegration");
    preintegrationButton->setToggle(false);
    preintegrationButton->getSelectCallbacks().add(this, &Shading::preintegrationCallback);
    new GLMotif::Blind("", lightingAndPreintegrationRowColumn);
    lightingAndPreintegrationRowColumn->manageChild();
    normalShadingRowColumn->manageChild();
} // end createNormalShading()

/*
 * createNormalContributionSlider
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter normalShadingRowColumn - GLMotif::RowColumn*&
 */
void Shading::createNormalContributionSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& normalShadingRowColumn) {
    GLMotif::RowColumn* normalRowColumn = new GLMotif::RowColumn("NormalRowColumn", normalShadingRowColumn, false);
    normalRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    normalRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    GLMotif::Label * normalLabel = new GLMotif::Label("NormalLabel", normalRowColumn, "Normal");
    normalLabel->setLabel("Normal");
    new GLMotif::Blind("", normalRowColumn);
    new GLMotif::Blind("", normalRowColumn);
    new GLMotif::Blind("", normalRowColumn);
    normalRowColumn->manageChild();
    GLMotif::RowColumn* normalContributionSliderRowColumn =
            new GLMotif::RowColumn("NormalContributionSliderRowColumn", normalShadingRowColumn, false);
    normalContributionSliderRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    normalContributionSliderRowColumn->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    GLMotif::Label * contributionLabel = new GLMotif::Label("ContributionLabel", normalContributionSliderRowColumn, "Contribution");
    contributionLabel->setLabel("Contribution");
    normalContributionTextField = new GLMotif::TextField("NormalContributionTextField", normalContributionSliderRowColumn, 6);
    normalContributionTextField->setFieldWidth(4);
    normalContributionTextField->setPrecision(3);
    normalContributionTextField->setValue(1.0f);
    normalContributionSlider
            = new GLMotif::Slider("NormalContributionSlider", normalContributionSliderRowColumn, GLMotif::Slider::HORIZONTAL, styleSheet.fontHeight
                    * 5.0f);
    normalContributionSlider->setSliderColor(GLMotif::Color(0.5f, 0.5f, 0.5f));
    normalContributionSlider->setValueRange(0.0f, 1.0f, 0.01f);
    normalContributionSlider->setValue(1.0f);
    normalContributionSlider->getValueChangedCallbacks().add(this, &Shading::normalContributionSliderCallback);
    normalContributionSliderRowColumn->manageChild();
} // end createNormalContributionSlider()

/*
 * createToneShading
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter shadingDialog - GLMotif::RowColumn*
 */
void Shading::createToneShading(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn * shadingDialog) {
    GLMotif::RowColumn * toneShadingRowColumn = new GLMotif::RowColumn("ToneShadingRowColumn", shadingDialog, false);
    toneShadingRowColumn->setOrientation(GLMotif::RowColumn::VERTICAL);
    toneShadingRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    toneShadingRowColumn->setBorderType(GLMotif::Widget::LOWERED);
    createToneContributionSlider(styleSheet, toneShadingRowColumn);
    GLMotif::RowColumn * warmAndCoolColorsRowColumn =
            new GLMotif::RowColumn("WarmAndCoolColorsRowColumn", toneShadingRowColumn, false);
    warmAndCoolColorsRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    warmAndCoolColorsRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    warmSelectedButton = new GLMotif::ToggleButton("WarmSelectedButton", warmAndCoolColorsRowColumn, "Warm");
    warmSelectedButton->setToggle(false);
    warmSelectedButton->getSelectCallbacks().add(this, &Shading::warmSelectedCallback);
    warmPane = new GLMotif::Blind("WarmPane", warmAndCoolColorsRowColumn);
    warmPane->setBorderWidth(styleSheet.size * 0.5f);
    warmPane->setBorderType(GLMotif::Widget::LOWERED);
    float * _color = warmColor->getValues();
    warmPane->setBackgroundColor(GLMotif::Color(_color[0], _color[1], _color[2]));
    warmPane->setPreferredSize(GLMotif::Vector(styleSheet.fontHeight * 5.0f, styleSheet.fontHeight * 1.0f, 0.0f));
    coolSelectedButton = new GLMotif::ToggleButton("CoolSelectedButton", warmAndCoolColorsRowColumn, "Cool");
    coolSelectedButton->setToggle(false);
    coolSelectedButton->getSelectCallbacks().add(this, &Shading::coolSelectedCallback);
    coolPane = new GLMotif::Blind("CoolPane", warmAndCoolColorsRowColumn);
    coolPane->setBorderWidth(styleSheet.size * 0.5f);
    coolPane->setBorderType(GLMotif::Widget::LOWERED);
    _color = coolColor->getValues();
    coolPane->setBackgroundColor(GLMotif::Color(_color[0], _color[1], _color[2]));
    coolPane->setPreferredSize(GLMotif::Vector(styleSheet.fontHeight * 5.0f, styleSheet.fontHeight * 1.0f, 0.0f));
    warmAndCoolColorsRowColumn->manageChild();
    toneShadingRowColumn->manageChild();
} // end createToneShading()

/*
 * createToneContributionSlider
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter toneShadingRowColumn - GLMotif::RowColumn*&
 */
void Shading::createToneContributionSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& toneShadingRowColumn) {
    GLMotif::RowColumn* toneRowColumn = new GLMotif::RowColumn("ToneRowColumn", toneShadingRowColumn, false);
    toneRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    toneRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    GLMotif::Label * toneLabel = new GLMotif::Label("ToneLabel", toneRowColumn, "Tone");
    toneLabel->setLabel("Tone");
    new GLMotif::Blind("", toneRowColumn);
    new GLMotif::Blind("", toneRowColumn);
    new GLMotif::Blind("", toneRowColumn);
    toneRowColumn->manageChild();
    GLMotif::RowColumn* toneContributionSliderRowColumn =
            new GLMotif::RowColumn("ToneContributionSliderRowColumn", toneShadingRowColumn, false);
    toneContributionSliderRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    toneContributionSliderRowColumn->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    GLMotif::Label * contributionLabel = new GLMotif::Label("ContributionLabel", toneContributionSliderRowColumn, "Contribution");
    contributionLabel->setLabel("Contribution");
    toneContributionTextField = new GLMotif::TextField("ToneContributionTextField", toneContributionSliderRowColumn, 6);
    toneContributionTextField->setFieldWidth(4);
    toneContributionTextField->setPrecision(3);
    toneContributionTextField->setValue(0.0f);
    toneContributionSlider
            = new GLMotif::Slider("ToneContributionSlider", toneContributionSliderRowColumn, GLMotif::Slider::HORIZONTAL, styleSheet.fontHeight
                    * 5.0f);
    toneContributionSlider->setSliderColor(GLMotif::Color(0.5f, 0.5f, 0.5f));
    toneContributionSlider->setValueRange(0.0f, 1.0f, 0.01f);
    toneContributionSlider->setValue(0.0f);
    toneContributionSlider->getValueChangedCallbacks().add(this, &Shading::toneContributionSliderCallback);
    toneContributionSliderRowColumn->manageChild();
} // end createToneContributionSlider()

/*
 * createToonShading
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter shadingDialog - GLMotif::RowColumn*
 */
void Shading::createToonShading(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn * shadingDialog) {
    GLMotif::RowColumn * toonShadingRowColumn = new GLMotif::RowColumn("ToonShadingRowColumn", shadingDialog, false);
    toonShadingRowColumn->setOrientation(GLMotif::RowColumn::VERTICAL);
    toonShadingRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    toonShadingRowColumn->setBorderType(GLMotif::Widget::LOWERED);
    createToonContributionSlider(styleSheet, toonShadingRowColumn);
    GLMotif::RowColumn * toonColorRowColumn = new GLMotif::RowColumn("ToonColorRowColumn", toonShadingRowColumn, false);
    toonColorRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    toonColorRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    toonSelectedButton = new GLMotif::ToggleButton("ToonSelectedButton", toonColorRowColumn, "Color");
    toonSelectedButton->setToggle(false);
    toonSelectedButton->getSelectCallbacks().add(this, &Shading::toonSelectedCallback);
    toonPane = new GLMotif::Blind("ToonPane", toonColorRowColumn);
    toonPane->setBorderWidth(styleSheet.size * 0.5f);
    toonPane->setBorderType(GLMotif::Widget::LOWERED);
    float * _color = toonColor->getValues();
    toonPane->setBackgroundColor(GLMotif::Color(_color[0], _color[1], _color[2]));
    toonPane->setPreferredSize(GLMotif::Vector(styleSheet.fontHeight * 5.0f, styleSheet.fontHeight * 1.0f, 0.0f));
    new GLMotif::Blind("", toonColorRowColumn);
    new GLMotif::Blind("", toonColorRowColumn);
    toonColorRowColumn->manageChild();
    toonShadingRowColumn->manageChild();
} // end createToonShading()

/*
 * createToonContributionSlider
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter toneShadingRowColumn - GLMotif::RowColumn*&
 */
void Shading::createToonContributionSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& toonShadingRowColumn) {
    GLMotif::RowColumn* toonRowColumn = new GLMotif::RowColumn("ToonRowColumn", toonShadingRowColumn, false);
    toonRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    toonRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    GLMotif::Label * toonLabel = new GLMotif::Label("ToonLabel", toonRowColumn, "Toon");
    toonLabel->setLabel("Toon");
    new GLMotif::Blind("", toonRowColumn);
    new GLMotif::Blind("", toonRowColumn);
    new GLMotif::Blind("", toonRowColumn);
    toonRowColumn->manageChild();
    GLMotif::RowColumn* toonContributionSliderRowColumn =
            new GLMotif::RowColumn("ToonContributionSliderRowColumn", toonShadingRowColumn, false);
    toonContributionSliderRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    toonContributionSliderRowColumn->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    GLMotif::Label * contributionLabel = new GLMotif::Label("ContributionLabel", toonContributionSliderRowColumn, "Contribution");
    contributionLabel->setLabel("Contribution");
    toonContributionTextField = new GLMotif::TextField("ToonContributionTextField", toonContributionSliderRowColumn, 6);
    toonContributionTextField->setFieldWidth(4);
    toonContributionTextField->setPrecision(3);
    toonContributionTextField->setValue(0.0f);
    toonContributionSlider
            = new GLMotif::Slider("ToonContributionSlider", toonContributionSliderRowColumn, GLMotif::Slider::HORIZONTAL, styleSheet.fontHeight
                    * 5.0f);
    toonContributionSlider->setSliderColor(GLMotif::Color(0.5f, 0.5f, 0.5f));
    toonContributionSlider->setValueRange(0.0f, 1.0f, 0.01f);
    toonContributionSlider->setValue(0.0f);
    toonContributionSlider->getValueChangedCallbacks().add(this, &Shading::toonContributionSliderCallback);
    toonContributionSliderRowColumn->manageChild();
} // end createToonContributionSlider()

/*
 * initialize - Initialize the GUI for the Shading class.
 */
void Shading::initialize(void) {
    toonColor = new RGBAColor(0.0f, 1.0f, 0.0f, 1.0f);
    warmColor = new RGBAColor(1.0f, 0.0f, 0.0f, 1.0f);
    coolColor = new RGBAColor(0.0f, 0.0f, 1.0f, 1.0f);
    const GLMotif::StyleSheet& styleSheet = *Vrui::getWidgetManager()->getStyleSheet();
    GLMotif::RowColumn* shadingDialog = new GLMotif::RowColumn("ShadingDialog", this, false);
    createNormalShading(styleSheet, shadingDialog);
    new GLMotif::Separator("", shadingDialog, GLMotif::Separator::HORIZONTAL, styleSheet.fontHeight * 5.0f, GLMotif::Separator::RAISED);
    createToneShading(styleSheet, shadingDialog);
    new GLMotif::Separator("", shadingDialog, GLMotif::Separator::HORIZONTAL, styleSheet.fontHeight * 5.0f, GLMotif::Separator::RAISED);
    createToonShading(styleSheet, shadingDialog);
    new GLMotif::Separator("", shadingDialog, GLMotif::Separator::HORIZONTAL, styleSheet.fontHeight * 5.0f, GLMotif::Separator::RAISED);
    GLMotif::RowColumn* colorEditor = createColorEditor(styleSheet, shadingDialog);
    colorEditor->manageChild();
    shadingDialog->manageChild();
}

/*
 * normalContributionSliderCallback - Callback of change to shininess slider value.
 *
 * parameter callbackData - Misc::CallbackData*
 */
void Shading::normalContributionSliderCallback(Misc::CallbackData* callbackData) {
    normalContribution = float(normalContributionSlider->getValue());
    normalContributionTextField->setValue(normalContribution);
    scene->setNormalContribution(normalContribution);
} // end normalContributionSliderCallback()

/*
 * toneContributionSliderCallback - Callback of change to shininess slider value.
 *
 * parameter callbackData - Misc::CallbackData*
 */
void Shading::toneContributionSliderCallback(Misc::CallbackData* callbackData) {
    toneContribution = float(toneContributionSlider->getValue());
    toneContributionTextField->setValue(toneContribution);
    scene->setToneContribution(toneContribution);
} // end toneContributionSliderCallback()

/*
 * toonContributionSliderCallback - Callback of change to shininess slider value.
 *
 * parameter callbackData - Misc::CallbackData*
 */
void Shading::toonContributionSliderCallback(Misc::CallbackData* callbackData) {
    toonContribution = float(toonContributionSlider->getValue());
    toonContributionTextField->setValue(toonContribution);
    scene->setToonContribution(toonContribution);
} // end toonContributionSliderCallback()

/*
 * warmSelectedCallback
 *
 * parameter callBackData - GLMotif::ToggleButton::ValueChangedCallbackData*
 */
void Shading::warmSelectedCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData) {
    if (strcmp(callBackData->toggle->getName(), "WarmSelectedButton") == 0) {
        toonSelectedButton->setToggle(false);
        coolSelectedButton->setToggle(false);
        warmSelectedButton->setToggle(true);
        warmSelected = true;
        coolSelected = false;
        toonSelected = false;
        float * _color = warmColor->getValues();
        for (int i = 0; i < 3; ++i) {
            colorSliders[i]->setValue(_color[i]);
        }
        colorPane->setBackgroundColor(_color);
        warmPane->setBackgroundColor(_color);
    }
    Vrui::requestUpdate();
} // end warmSelectedCallback()
