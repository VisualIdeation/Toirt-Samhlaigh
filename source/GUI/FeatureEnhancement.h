/*
 * FeatureEnhancement.h - Class for editing the feature enhancement settings for shading.
 *
 * Author: Patrick O'Leary
 * Created: October 6, 2008
 * Copyright: 2008. All rights reserved.
 */
#ifndef FEATUREENHANCEMENT_INCLUDED
#define FEATUREENHANCEMENT_INCLUDED

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
class Scene;
class Toirt_Samhlaigh;
// end Forward Declarations

class FeatureEnhancement: public GLMotif::PopupWindow {
public:
    FeatureEnhancement(Scene * _scene, Toirt_Samhlaigh & _toirt_samhlaigh);
    ~FeatureEnhancement(void);
private:
    float boundaryContribution;
    GLMotif::TextField * boundaryContributionTextField;
    GLMotif::Slider * boundaryContributionSlider;
    float boundaryExponent;
    GLMotif::TextField * boundaryExponentTextField;
    GLMotif::Slider * boundaryExponentSlider;
    float edgeContribution;
    GLMotif::TextField * edgeContributionTextField;
    GLMotif::Slider * edgeContributionSlider;
    float edgeExponent;
    GLMotif::TextField * edgeExponentTextField;
    GLMotif::Slider * edgeExponentSlider;
    float edgeThreshold;
    GLMotif::TextField * edgeThresholdTextField;
    GLMotif::Slider * edgeThresholdSlider;
    Scene * scene;
    float silhouetteContribution;
    GLMotif::TextField * silhouetteContributionTextField;
    GLMotif::Slider * silhouetteContributionSlider;
    float silhouetteExponent;
    GLMotif::TextField * silhouetteExponentTextField;
    GLMotif::Slider * silhouetteExponentSlider;
    Toirt_Samhlaigh & toirt_samhlaigh;
    void boundaryContributionSliderCallback(Misc::CallbackData* callbackData);
    void boundaryExponentSliderCallback(Misc::CallbackData* callbackData);
    void createBoundaryContributionSlider(const GLMotif::StyleSheet& styleSheet,
            GLMotif::RowColumn*& boundaryFeatureEnhancementRowColumn);
    void createBoundaryExponentSlider(const GLMotif::StyleSheet& styleSheet,
            GLMotif::RowColumn*& boundaryFeatureEnhancementRowColumn);
    void createBoundaryFeatureEnhancement(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn * featureEnhancementDialog);
    void createEdgeContributionSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& edgeFeatureEnhancementRowColumn);
    void createEdgeExponentSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& edgeFeatureEnhancementRowColumn);
    void createEdgeFeatureEnhancement(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn * featureEnhancementDialog);
    void createEdgeThresholdSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& edgeFeatureEnhancementRowColumn);
    void createSilhouetteContributionSlider(const GLMotif::StyleSheet& styleSheet,
            GLMotif::RowColumn*& silhouetteFeatureEnhancementRowColumn);
    void createSilhouetteExponentSlider(const GLMotif::StyleSheet& styleSheet,
            GLMotif::RowColumn*& silhouetteFeatureEnhancementRowColumn);
    void createSilhouetteFeatureEnhancement(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn * featureEnhancementDialog);
    void edgeContributionSliderCallback(Misc::CallbackData* callbackData);
    void edgeExponentSliderCallback(Misc::CallbackData* callbackData);
    void edgeThresholdSliderCallback(Misc::CallbackData* callbackData);
    void initialize(void);
    void silhouetteContributionSliderCallback(Misc::CallbackData* callbackData);
    void silhouetteExponentSliderCallback(Misc::CallbackData* callbackData);
};

#endif // FEATUREENHANCEMENT_INCLUDED
