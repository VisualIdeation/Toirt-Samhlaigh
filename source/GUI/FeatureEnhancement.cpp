/*
 * FeatureEnhancement.cpp - Methods for FeatureEnhancement class.
 *
 * Author: Patrick O'Leary
 * Created: October 6, 2008
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

#include <GRAPHIC/Scene.h>
#include <GUI/FeatureEnhancement.h>
#include <Toirt_Samhlaigh.h>

/*
 * FeatureEnhancement - Constructor for FeatureEnhancement class.
 * 		extends GLMotif::PopupWindow
 */
FeatureEnhancement::FeatureEnhancement(Scene * _scene, Toirt_Samhlaigh & _toirt_samhlaigh) :
    GLMotif::PopupWindow("FeatureEnhancementPopup", Vrui::getWidgetManager(), "FeatureEnhancement"), boundaryContribution(0.0),
            boundaryExponent(1.0), edgeContribution(0.0), edgeExponent(1.0), edgeThreshold(0.1), scene(_scene),
            silhouetteContribution(0.0), silhouetteExponent(1.0), toirt_samhlaigh(_toirt_samhlaigh) {
    initialize();
}

/*
 * ~FeatureEnhancement - Destructor for FeatureEnhancement class.
 */
FeatureEnhancement::~FeatureEnhancement(void) {
}

/*
 * boundaryContributionSliderCallback - Callback of change to boundary contribution slider value.
 *
 * parameter callbackData - Misc::CallbackData*
 */
void FeatureEnhancement::boundaryContributionSliderCallback(Misc::CallbackData* callbackData) {
    boundaryContribution = float(boundaryContributionSlider->getValue());
    boundaryContributionTextField->setValue(boundaryContribution);
    scene->setBoundaryContribution(boundaryContribution);
} // end boundaryContributionSliderCallback()

/*
 * boundaryExponentSliderCallback - Callback of change to boundary exponent slider value.
 *
 * parameter callbackData - Misc::CallbackData*
 */
void FeatureEnhancement::boundaryExponentSliderCallback(Misc::CallbackData* callbackData) {
    boundaryExponent = float(boundaryExponentSlider->getValue());
    boundaryExponentTextField->setValue(boundaryExponent);
    scene->setBoundaryExponent(boundaryExponent);
} // end boundaryExponentSliderCallback()

/*
 * createBoundaryContributionSlider
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter boundaryFeatureEnhancementRowColumn - GLMotif::RowColumn*&
 */
void FeatureEnhancement::createBoundaryContributionSlider(const GLMotif::StyleSheet& styleSheet,
        GLMotif::RowColumn*& boundaryFeatureEnhancementRowColumn) {
    GLMotif::RowColumn* boundaryContributionSliderRowColumn =
            new GLMotif::RowColumn("BoundaryContributionSliderRowColumn", boundaryFeatureEnhancementRowColumn, false);
    boundaryContributionSliderRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    boundaryContributionSliderRowColumn->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    GLMotif::Label * contributionLabel =
            new GLMotif::Label("ContributionLabel", boundaryContributionSliderRowColumn, "Contribution");
    contributionLabel->setLabel("Contribution");
    boundaryContributionTextField = new GLMotif::TextField("BoundaryContributionTextField", boundaryContributionSliderRowColumn, 6);
    boundaryContributionTextField->setFieldWidth(4);
    boundaryContributionTextField->setPrecision(3);
    boundaryContributionTextField->setValue(0.0f);
    boundaryContributionSlider
            = new GLMotif::Slider("BoundaryContributionSlider", boundaryContributionSliderRowColumn, GLMotif::Slider::HORIZONTAL, styleSheet.fontHeight
                    * 5.0f);
    boundaryContributionSlider->setSliderColor(GLMotif::Color(0.5f, 0.5f, 0.5f));
    boundaryContributionSlider->setValueRange(0.0f, 1.0f, 0.01f);
    boundaryContributionSlider->setValue(0.0f);
    boundaryContributionSlider->getValueChangedCallbacks().add(this, &FeatureEnhancement::boundaryContributionSliderCallback);
    boundaryContributionSliderRowColumn->manageChild();
} // end createBoundaryContributionSlider()

/*
 * createBoundaryExponentSlider
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter boundaryFeatureEnhancementRowColumn - GLMotif::RowColumn*&
 */
void FeatureEnhancement::createBoundaryExponentSlider(const GLMotif::StyleSheet& styleSheet,
        GLMotif::RowColumn*& boundaryFeatureEnhancementRowColumn) {
    GLMotif::RowColumn* boundaryExponentSliderRowColumn =
            new GLMotif::RowColumn("BoundaryExponentSliderRowColumn", boundaryFeatureEnhancementRowColumn, false);
    boundaryExponentSliderRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    boundaryExponentSliderRowColumn->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    GLMotif::Label * exponentLabel = new GLMotif::Label("ExponentLabel", boundaryExponentSliderRowColumn, "Exponent");
    exponentLabel->setLabel("Exponent      ");
    boundaryExponentTextField = new GLMotif::TextField("BoundaryExponentTextField", boundaryExponentSliderRowColumn, 6);
    boundaryExponentTextField->setFieldWidth(4);
    boundaryExponentTextField->setPrecision(3);
    boundaryExponentTextField->setValue(1.0f);
    boundaryExponentSlider
            = new GLMotif::Slider("BoundaryExponentSlider", boundaryExponentSliderRowColumn, GLMotif::Slider::HORIZONTAL, styleSheet.fontHeight
                    * 5.0f);
    boundaryExponentSlider->setSliderColor(GLMotif::Color(0.5f, 0.5f, 0.5f));
    boundaryExponentSlider->setValueRange(0.5f, 4.0f, 0.05f);
    boundaryExponentSlider->setValue(1.0f);
    boundaryExponentSlider->getValueChangedCallbacks().add(this, &FeatureEnhancement::boundaryExponentSliderCallback);
    boundaryExponentSliderRowColumn->manageChild();
} // end createBoundaryExponentSlider()

/*
 * createBoundaryFeatureEnhancement
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter featureEnhancementDialog - GLMotif::RowColumn*
 */
void FeatureEnhancement::createBoundaryFeatureEnhancement(const GLMotif::StyleSheet& styleSheet,
        GLMotif::RowColumn * featureEnhancementDialog) {
    GLMotif::RowColumn * boundaryFeatureEnhancementRowColumn =
            new GLMotif::RowColumn("BoundaryFeatureEnhancementRowColumn", featureEnhancementDialog, false);
    boundaryFeatureEnhancementRowColumn->setOrientation(GLMotif::RowColumn::VERTICAL);
    boundaryFeatureEnhancementRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    boundaryFeatureEnhancementRowColumn->setBorderType(GLMotif::Widget::LOWERED);
    GLMotif::RowColumn* boundaryRowColumn = new GLMotif::RowColumn("BoundaryRowColumn", boundaryFeatureEnhancementRowColumn, false);
    boundaryRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    boundaryRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    GLMotif::Label * boundaryLabel = new GLMotif::Label("BoundaryLabel", boundaryRowColumn, "Boundary");
    boundaryLabel->setLabel("Boundary");
    new GLMotif::Blind("", boundaryRowColumn);
    new GLMotif::Blind("", boundaryRowColumn);
    new GLMotif::Blind("", boundaryRowColumn);
    boundaryRowColumn->manageChild();
    createBoundaryContributionSlider(styleSheet, boundaryFeatureEnhancementRowColumn);
    createBoundaryExponentSlider(styleSheet, boundaryFeatureEnhancementRowColumn);
    boundaryFeatureEnhancementRowColumn->manageChild();
} // end createBoundaryFeatureEnhancement()

/*
 * createEdgeContributionSlider
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter edgeFeatureEnhancementRowColumn - GLMotif::RowColumn*&
 */
void FeatureEnhancement::createEdgeContributionSlider(const GLMotif::StyleSheet& styleSheet,
        GLMotif::RowColumn*& edgeFeatureEnhancementRowColumn) {
    GLMotif::RowColumn* edgeContributionSliderRowColumn =
            new GLMotif::RowColumn("EdgeContributionSliderRowColumn", edgeFeatureEnhancementRowColumn, false);
    edgeContributionSliderRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    edgeContributionSliderRowColumn->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    GLMotif::Label * contributionLabel = new GLMotif::Label("ContributionLabel", edgeContributionSliderRowColumn, "Contribution");
    contributionLabel->setLabel("Contribution");
    edgeContributionTextField = new GLMotif::TextField("EdgeContributionTextField", edgeContributionSliderRowColumn, 6);
    edgeContributionTextField->setFieldWidth(4);
    edgeContributionTextField->setPrecision(3);
    edgeContributionTextField->setValue(0.0f);
    edgeContributionSlider
            = new GLMotif::Slider("EdgeContributionSlider", edgeContributionSliderRowColumn, GLMotif::Slider::HORIZONTAL, styleSheet.fontHeight
                    * 5.0f);
    edgeContributionSlider->setSliderColor(GLMotif::Color(0.5f, 0.5f, 0.5f));
    edgeContributionSlider->setValueRange(0.0f, 1.0f, 1.0f);
    edgeContributionSlider->setValue(0.0f);
    edgeContributionSlider->getValueChangedCallbacks().add(this, &FeatureEnhancement::edgeContributionSliderCallback);
    edgeContributionSliderRowColumn->manageChild();
} // end createEdgeContributionSlider()

/*
 * createEdgeExponentSlider
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter edgeFeatureEnhancementRowColumn - GLMotif::RowColumn*&
 */
void FeatureEnhancement::createEdgeExponentSlider(const GLMotif::StyleSheet& styleSheet,
        GLMotif::RowColumn*& edgeFeatureEnhancementRowColumn) {
    GLMotif::RowColumn* edgeExponentSliderRowColumn =
            new GLMotif::RowColumn("EdgeExponentSliderRowColumn", edgeFeatureEnhancementRowColumn, false);
    edgeExponentSliderRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    edgeExponentSliderRowColumn->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    GLMotif::Label * exponentLabel = new GLMotif::Label("ExponentLabel", edgeExponentSliderRowColumn, "Exponent");
    exponentLabel->setLabel("Exponent      ");
    edgeExponentTextField = new GLMotif::TextField("EdgeExponentTextField", edgeExponentSliderRowColumn, 6);
    edgeExponentTextField->setFieldWidth(4);
    edgeExponentTextField->setPrecision(3);
    edgeExponentTextField->setValue(1.0f);
    edgeExponentSlider
            = new GLMotif::Slider("EdgeExponentSlider", edgeExponentSliderRowColumn, GLMotif::Slider::HORIZONTAL, styleSheet.fontHeight
                    * 5.0f);
    edgeExponentSlider->setSliderColor(GLMotif::Color(0.5f, 0.5f, 0.5f));
    edgeExponentSlider->setValueRange(0.5f, 4.0f, 0.05f);
    edgeExponentSlider->setValue(1.0f);
    edgeExponentSlider->getValueChangedCallbacks().add(this, &FeatureEnhancement::edgeExponentSliderCallback);
    edgeExponentSliderRowColumn->manageChild();
} // end createEdgeExponentSlider()

/*
 * createEdgeFeatureEnhancement
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter featureEnhancementDialog - GLMotif::RowColumn*
 */
void FeatureEnhancement::createEdgeFeatureEnhancement(const GLMotif::StyleSheet& styleSheet,
        GLMotif::RowColumn * featureEnhancementDialog) {
    GLMotif::RowColumn * edgeFeatureEnhancementRowColumn =
            new GLMotif::RowColumn("EdgeFeatureEnhancementRowColumn", featureEnhancementDialog, false);
    edgeFeatureEnhancementRowColumn->setOrientation(GLMotif::RowColumn::VERTICAL);
    edgeFeatureEnhancementRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    edgeFeatureEnhancementRowColumn->setBorderType(GLMotif::Widget::LOWERED);
    GLMotif::RowColumn* edgeRowColumn = new GLMotif::RowColumn("EdgeRowColumn", edgeFeatureEnhancementRowColumn, false);
    edgeRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    edgeRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    GLMotif::Label * edgeLabel = new GLMotif::Label("EdgeLabel", edgeRowColumn, "Edge");
    edgeLabel->setLabel("Edge");
    new GLMotif::Blind("", edgeRowColumn);
    new GLMotif::Blind("", edgeRowColumn);
    new GLMotif::Blind("", edgeRowColumn);
    edgeRowColumn->manageChild();
    createEdgeContributionSlider(styleSheet, edgeFeatureEnhancementRowColumn);
    createEdgeExponentSlider(styleSheet, edgeFeatureEnhancementRowColumn);
    createEdgeThresholdSlider(styleSheet, edgeFeatureEnhancementRowColumn);
    edgeFeatureEnhancementRowColumn->manageChild();
} // end createEdgeFeatureEnhancement()

/*
 * createEdgeThresholdSlider
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter edgeFeatureEnhancementRowColumn - GLMotif::RowColumn*&
 */
void FeatureEnhancement::createEdgeThresholdSlider(const GLMotif::StyleSheet& styleSheet,
        GLMotif::RowColumn*& edgeFeatureEnhancementRowColumn) {
    GLMotif::RowColumn* edgeThresholdSliderRowColumn =
            new GLMotif::RowColumn("EdgeThresholdSliderRowColumn", edgeFeatureEnhancementRowColumn, false);
    edgeThresholdSliderRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    edgeThresholdSliderRowColumn->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    GLMotif::Label * exponentLabel = new GLMotif::Label("ThresholdLabel", edgeThresholdSliderRowColumn, "Threshold");
    exponentLabel->setLabel("Threshold     ");
    edgeThresholdTextField = new GLMotif::TextField("EdgeThresholdTextField", edgeThresholdSliderRowColumn, 6);
    edgeThresholdTextField->setFieldWidth(4);
    edgeThresholdTextField->setPrecision(3);
    edgeThresholdTextField->setValue(1.0f);
    edgeThresholdSlider
            = new GLMotif::Slider("EdgeThresholdSlider", edgeThresholdSliderRowColumn, GLMotif::Slider::HORIZONTAL, styleSheet.fontHeight
                    * 5.0f);
    edgeThresholdSlider->setSliderColor(GLMotif::Color(0.5f, 0.5f, 0.5f));
    edgeThresholdSlider->setValueRange(0.0f, 1.0f, 0.01f);
    edgeThresholdSlider->setValue(0.1f);
    edgeThresholdSlider->getValueChangedCallbacks().add(this, &FeatureEnhancement::edgeThresholdSliderCallback);
    edgeThresholdSliderRowColumn->manageChild();
} // end createEdgeThresholdSlider()

/*
 * createSilhouetteContributionSlider
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter silhouetteFeatureEnhancementRowColumn - GLMotif::RowColumn*&
 */
void FeatureEnhancement::createSilhouetteContributionSlider(const GLMotif::StyleSheet& styleSheet,
        GLMotif::RowColumn*& silhouetteFeatureEnhancementRowColumn) {
    GLMotif::RowColumn* silhouetteContributionSliderRowColumn =
            new GLMotif::RowColumn("SilhouetteContributionSliderRowColumn", silhouetteFeatureEnhancementRowColumn, false);
    silhouetteContributionSliderRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    silhouetteContributionSliderRowColumn->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    GLMotif::Label * contributionLabel =
            new GLMotif::Label("ContributionLabel", silhouetteContributionSliderRowColumn, "Contribution");
    contributionLabel->setLabel("Contribution");
    silhouetteContributionTextField
            = new GLMotif::TextField("SilhouetteContributionTextField", silhouetteContributionSliderRowColumn, 6);
    silhouetteContributionTextField->setFieldWidth(4);
    silhouetteContributionTextField->setPrecision(3);
    silhouetteContributionTextField->setValue(0.0f);
    silhouetteContributionSlider
            = new GLMotif::Slider("SilhouetteContributionSlider", silhouetteContributionSliderRowColumn, GLMotif::Slider::HORIZONTAL, styleSheet.fontHeight
                    * 5.0f);
    silhouetteContributionSlider->setSliderColor(GLMotif::Color(0.5f, 0.5f, 0.5f));
    silhouetteContributionSlider->setValueRange(0.0f, 1.0f, 0.01f);
    silhouetteContributionSlider->setValue(0.0f);
    silhouetteContributionSlider->getValueChangedCallbacks().add(this, &FeatureEnhancement::silhouetteContributionSliderCallback);
    silhouetteContributionSliderRowColumn->manageChild();
} // end createSilhouetteContributionSlider()

/*
 * createSilhouetteExponentSlider
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter silhouetteFeatureEnhancementRowColumn - GLMotif::RowColumn*&
 */
void FeatureEnhancement::createSilhouetteExponentSlider(const GLMotif::StyleSheet& styleSheet,
        GLMotif::RowColumn*& silhouetteFeatureEnhancementRowColumn) {
    GLMotif::RowColumn* silhouetteExponentSliderRowColumn =
            new GLMotif::RowColumn("SilhouetteExponentSliderRowColumn", silhouetteFeatureEnhancementRowColumn, false);
    silhouetteExponentSliderRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    silhouetteExponentSliderRowColumn->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    GLMotif::Label * exponentLabel = new GLMotif::Label("ExponentLabel", silhouetteExponentSliderRowColumn, "Exponent");
    exponentLabel->setLabel("Exponent      ");
    silhouetteExponentTextField = new GLMotif::TextField("SilhouetteExponentTextField", silhouetteExponentSliderRowColumn, 6);
    silhouetteExponentTextField->setFieldWidth(4);
    silhouetteExponentTextField->setPrecision(3);
    silhouetteExponentTextField->setValue(1.0f);
    silhouetteExponentSlider
            = new GLMotif::Slider("SilhouetteExponentSlider", silhouetteExponentSliderRowColumn, GLMotif::Slider::HORIZONTAL, styleSheet.fontHeight
                    * 5.0f);
    silhouetteExponentSlider->setSliderColor(GLMotif::Color(0.5f, 0.5f, 0.5f));
    silhouetteExponentSlider->setValueRange(0.5f, 4.0f, 0.05f);
    silhouetteExponentSlider->setValue(1.0f);
    silhouetteExponentSlider->getValueChangedCallbacks().add(this, &FeatureEnhancement::silhouetteExponentSliderCallback);
    silhouetteExponentSliderRowColumn->manageChild();
} // end createSilhouetteExponentSlider()

/*
 * createSilhouetteFeatureEnhancement
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter featureEnhancementDialog - GLMotif::RowColumn*
 */
void FeatureEnhancement::createSilhouetteFeatureEnhancement(const GLMotif::StyleSheet& styleSheet,
        GLMotif::RowColumn * featureEnhancementDialog) {
    GLMotif::RowColumn * silhouetteFeatureEnhancementRowColumn =
            new GLMotif::RowColumn("SilhouetteFeatureEnhancementRowColumn", featureEnhancementDialog, false);
    silhouetteFeatureEnhancementRowColumn->setOrientation(GLMotif::RowColumn::VERTICAL);
    silhouetteFeatureEnhancementRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    silhouetteFeatureEnhancementRowColumn->setBorderType(GLMotif::Widget::LOWERED);
    GLMotif::RowColumn* silhouetteRowColumn =
            new GLMotif::RowColumn("SilhouetteRowColumn", silhouetteFeatureEnhancementRowColumn, false);
    silhouetteRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    silhouetteRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    GLMotif::Label * silhouetteLabel = new GLMotif::Label("SilhouetteLabel", silhouetteRowColumn, "Silhouette");
    silhouetteLabel->setLabel("Silhouette");
    new GLMotif::Blind("", silhouetteRowColumn);
    new GLMotif::Blind("", silhouetteRowColumn);
    new GLMotif::Blind("", silhouetteRowColumn);
    silhouetteRowColumn->manageChild();
    createSilhouetteContributionSlider(styleSheet, silhouetteFeatureEnhancementRowColumn);
    createSilhouetteExponentSlider(styleSheet, silhouetteFeatureEnhancementRowColumn);
    silhouetteFeatureEnhancementRowColumn->manageChild();
} // end createSilhouetteFeatureEnhancement()

/*
 * edgeContributionSliderCallback - Callback of change to edge contribution slider value.
 *
 * parameter callbackData - Misc::CallbackData*
 */
void FeatureEnhancement::edgeContributionSliderCallback(Misc::CallbackData* callbackData) {
    edgeContribution = float(edgeContributionSlider->getValue());
    edgeContributionTextField->setValue(edgeContribution);
    scene->setEdgeContribution(edgeContribution);
} // end edgeContributionSliderCallback()

/*
 * edgeExponentSliderCallback - Callback of change to edge exponent slider value.
 *
 * parameter callbackData - Misc::CallbackData*
 */
void FeatureEnhancement::edgeExponentSliderCallback(Misc::CallbackData* callbackData) {
    edgeExponent = float(edgeExponentSlider->getValue());
    edgeExponentTextField->setValue(edgeExponent);
    scene->setEdgeExponent(edgeExponent);
} // end edgeExponentSliderCallback()

/*
 * edgeThresholdSliderCallback - Callback of change to edge threshold slider value.
 *
 * parameter callbackData - Misc::CallbackData*
 */
void FeatureEnhancement::edgeThresholdSliderCallback(Misc::CallbackData* callbackData) {
    edgeThreshold = float(edgeThresholdSlider->getValue());
    edgeThresholdTextField->setValue(edgeThreshold);
    scene->setEdgeThreshold(edgeThreshold);
} // end edgeThresholdSliderCallback()

/*
 * initialize - Initialize the GUI for the FeatureEnhancement class.
 */
void FeatureEnhancement::initialize(void) {
    const GLMotif::StyleSheet& styleSheet = *Vrui::getWidgetManager()->getStyleSheet();
    GLMotif::RowColumn* featureEnhancementDialog = new GLMotif::RowColumn("FeatureEnhancementDialog", this, false);
    createBoundaryFeatureEnhancement(styleSheet, featureEnhancementDialog);
    new GLMotif::Separator("", featureEnhancementDialog, GLMotif::Separator::HORIZONTAL, styleSheet.fontHeight * 5.0f, GLMotif::Separator::RAISED);
    createEdgeFeatureEnhancement(styleSheet, featureEnhancementDialog);
    new GLMotif::Separator("", featureEnhancementDialog, GLMotif::Separator::HORIZONTAL, styleSheet.fontHeight * 5.0f, GLMotif::Separator::RAISED);
    createSilhouetteFeatureEnhancement(styleSheet, featureEnhancementDialog);
    featureEnhancementDialog->manageChild();
}

/*
 * silhouetteContributionSliderCallback - Callback of change to silhouette contribution slider value.
 *
 * parameter callbackData - Misc::CallbackData*
 */
void FeatureEnhancement::silhouetteContributionSliderCallback(Misc::CallbackData* callbackData) {
    silhouetteContribution = float(silhouetteContributionSlider->getValue());
    silhouetteContributionTextField->setValue(silhouetteContribution);
    scene->setSilhouetteContribution(silhouetteContribution);
} // end silhouetteContributionSliderCallback()

/*
 * silhouetteExponentSliderCallback - Callback of change to silhouette exponent slider value.
 *
 * parameter callbackData - Misc::CallbackData*
 */
void FeatureEnhancement::silhouetteExponentSliderCallback(Misc::CallbackData* callbackData) {
    silhouetteExponent = float(silhouetteExponentSlider->getValue());
    silhouetteExponentTextField->setValue(silhouetteExponent);
    scene->setSilhouetteExponent(silhouetteExponent);
} // end silhouetteExponentSliderCallback()
