/*
 * DownSampling.cpp - Methods for DownSampling class.
 *
 * Author: Patrick O'Leary
 * Created: October 9, 2008
 * Copyright: 2008. All rights reserved.
 */
#include <iostream>

/* Vrui includes to use the Vrui interface */
#include <Vrui/Vrui.h>

/* Vrui includes */
#include <GLMotif/Label.h>
#include <GLMotif/WidgetManager.h>

#include <GRAPHIC/Scene.h>
#include <GUI/DownSampling.h>
#include <Toirt_Samhlaigh.h>

/*
 * DownSampling - Constructor for DownSampling class.
 * 		extends GLMotif::PopupWindow
 */
DownSampling::DownSampling(Scene * _scene, Toirt_Samhlaigh & _toirt_samhlaigh, int _stepMaximum) :
    GLMotif::PopupWindow("DownSamplingPopup", Vrui::getWidgetManager(), "DownSampling"), scene(_scene),
    stepMaximum(_stepMaximum), slicingScale(2.0), toirt_samhlaigh(_toirt_samhlaigh), xStep(2), yStep(2), zStep(2) {
    initialize();
}

/*
 * ~DownSampling - Destructor for DownSampling class.
 */
DownSampling::~DownSampling(void) {
}

/*
 * createSlicingScaleSlider
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter downSamplingDialog - GLMotif::RowColumn*&
 */
void DownSampling::createSlicingScaleSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& downSamplingDialog) {
    GLMotif::RowColumn* slicingScaleSliderRowColumn = new GLMotif::RowColumn("SlicingScaleSliderRowColumn", downSamplingDialog, false);
    slicingScaleSliderRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    slicingScaleSliderRowColumn->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    GLMotif::Label * slicingScaleLabel = new GLMotif::Label("SlicingScaleLabel", slicingScaleSliderRowColumn, "Scale");
    slicingScaleLabel->setLabel("Scale");
    slicingScaleTextField = new GLMotif::TextField("SlicingScaleTextField", slicingScaleSliderRowColumn, 6);
    slicingScaleTextField->setFieldWidth(4);
    slicingScaleTextField->setPrecision(3);
    slicingScaleTextField->setValue(slicingScale);
    slicingScaleSlider = new GLMotif::Slider("SlicingScaleSlider", slicingScaleSliderRowColumn, GLMotif::Slider::HORIZONTAL, styleSheet.fontHeight
            * 5.0f);
    slicingScaleSlider->setSliderColor(GLMotif::Color(0.5f, 0.5f, 0.5f));
    slicingScaleSlider->setValueRange(1.0f, float(stepMaximum), 0.25f);
    slicingScaleSlider->setValue(slicingScale);
    slicingScaleSlider->getValueChangedCallbacks().add(this, &DownSampling::slicingScaleSliderCallback);
    slicingScaleSliderRowColumn->manageChild();
} // end createSlicingScaleSlider()

/*
 * createXStepSlider
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter downSamplingDialog - GLMotif::RowColumn*&
 */
void DownSampling::createXStepSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& downSamplingDialog) {
    GLMotif::RowColumn* xStepSliderRowColumn = new GLMotif::RowColumn("XStepSliderRowColumn", downSamplingDialog, false);
    xStepSliderRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    xStepSliderRowColumn->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    GLMotif::Label * xStepLabel = new GLMotif::Label("XStepLabel", xStepSliderRowColumn, "X");
    xStepLabel->setLabel("X");
    xStepTextField = new GLMotif::TextField("XStepTextField", xStepSliderRowColumn, 6);
    xStepTextField->setFieldWidth(4);
    xStepTextField->setPrecision(3);
    xStepTextField->setValue(xStep);
    xStepSlider = new GLMotif::Slider("XStepSlider", xStepSliderRowColumn, GLMotif::Slider::HORIZONTAL, styleSheet.fontHeight
            * 5.0f);
    xStepSlider->setSliderColor(GLMotif::Color(0.5f, 0.5f, 0.5f));
    xStepSlider->setValueRange(1, stepMaximum, 1);
    xStepSlider->setValue(xStep);
    xStepSlider->getValueChangedCallbacks().add(this, &DownSampling::xStepSliderCallback);
    xStepSliderRowColumn->manageChild();
} // end createXStepSlider()

/*
 * createYStepSlider
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter downSamplingDialog - GLMotif::RowColumn*&
 */
void DownSampling::createYStepSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& downSamplingDialog) {
    GLMotif::RowColumn* yStepSliderRowColumn = new GLMotif::RowColumn("YStepSliderRowColumn", downSamplingDialog, false);
    yStepSliderRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    yStepSliderRowColumn->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    GLMotif::Label * yStepLabel = new GLMotif::Label("YStepLabel", yStepSliderRowColumn, "Y");
    yStepLabel->setLabel("Y");
    yStepTextField = new GLMotif::TextField("YStepTextField", yStepSliderRowColumn, 6);
    yStepTextField->setFieldWidth(4);
    yStepTextField->setPrecision(3);
    yStepTextField->setValue(yStep);
    yStepSlider = new GLMotif::Slider("YStepSlider", yStepSliderRowColumn, GLMotif::Slider::HORIZONTAL, styleSheet.fontHeight
            * 5.0f);
    yStepSlider->setSliderColor(GLMotif::Color(0.5f, 0.5f, 0.5f));
    yStepSlider->setValueRange(1, stepMaximum, 1);
    yStepSlider->setValue(yStep);
    yStepSlider->getValueChangedCallbacks().add(this, &DownSampling::yStepSliderCallback);
    yStepSliderRowColumn->manageChild();
} // end createYStepSlider()

/*
 * createZStepSlider
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter downSamplingDialog - GLMotif::RowColumn*&
 */
void DownSampling::createZStepSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& downSamplingDialog) {
    GLMotif::RowColumn* zStepSliderRowColumn = new GLMotif::RowColumn("ZStepSliderRowColumn", downSamplingDialog, false);
    zStepSliderRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    zStepSliderRowColumn->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    GLMotif::Label * zStepLabel = new GLMotif::Label("ZStepLabel", zStepSliderRowColumn, "Z");
    zStepLabel->setLabel("Z");
    zStepTextField = new GLMotif::TextField("ZStepTextField", zStepSliderRowColumn, 6);
    zStepTextField->setFieldWidth(4);
    zStepTextField->setPrecision(3);
    zStepTextField->setValue(zStep);
    zStepSlider = new GLMotif::Slider("ZStepSlider", zStepSliderRowColumn, GLMotif::Slider::HORIZONTAL, styleSheet.fontHeight
            * 5.0f);
    zStepSlider->setSliderColor(GLMotif::Color(0.5f, 0.5f, 0.5f));
    zStepSlider->setValueRange(1, stepMaximum, 1);
    zStepSlider->setValue(zStep);
    zStepSlider->getValueChangedCallbacks().add(this, &DownSampling::zStepSliderCallback);
    zStepSliderRowColumn->manageChild();
} // end createZStepSlider()

/*
 * initialize - Initialize the GUI for the DownSampling class.
 */
void DownSampling::initialize(void) {
    const GLMotif::StyleSheet& styleSheet = *Vrui::getWidgetManager()->getStyleSheet();
    GLMotif::RowColumn* downSamplingDialog = new GLMotif::RowColumn("DownSamplingDialog", this, false);
    createSlicingScaleSlider(styleSheet, downSamplingDialog);
    createXStepSlider(styleSheet, downSamplingDialog);
    createYStepSlider(styleSheet, downSamplingDialog);
    createZStepSlider(styleSheet, downSamplingDialog);
    downSamplingDialog->manageChild();
}

/*
 * slicingScaleSliderCallback - Callback of change to slicingScale slider value.
 *
 * parameter callbackData - Misc::CallbackData*
 */
void DownSampling::slicingScaleSliderCallback(Misc::CallbackData* callbackData) {
    slicingScale = float(slicingScaleSlider->getValue());
    slicingScaleTextField->setValue(slicingScale);
    scene->setSlicingScale(slicingScale);
} // end slicingScaleSliderCallback()

/*
 * xStepSliderCallback - Callback of change to xStep slider value.
 *
 * parameter callbackData - Misc::CallbackData*
 */
void DownSampling::xStepSliderCallback(Misc::CallbackData* callbackData) {
    xStep = int(xStepSlider->getValue());
    xStepTextField->setValue(xStep);
    scene->setXStep(xStep);
    toirt_samhlaigh.setDownSamplingChanged(true);
} // end xStepSliderCallback()

/*
 * yStepSliderCallback - Callback of change to yStep slider value.
 *
 * parameter callbackData - Misc::CallbackData*
 */
void DownSampling::yStepSliderCallback(Misc::CallbackData* callbackData) {
    yStep = int(yStepSlider->getValue());
    yStepTextField->setValue(yStep);
    scene->setYStep(yStep);
    toirt_samhlaigh.setDownSamplingChanged(true);
} // end yStepSliderCallback()

/*
 * zStepSliderCallback - Callback of change to zStep slider value.
 *
 * parameter callbackData - Misc::CallbackData*
 */
void DownSampling::zStepSliderCallback(Misc::CallbackData* callbackData) {
    zStep = int(zStepSlider->getValue());
    zStepTextField->setValue(zStep);
    scene->setZStep(zStep);
    toirt_samhlaigh.setDownSamplingChanged(true);
} // end zStepSliderCallback()
