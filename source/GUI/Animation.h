/*
 * Animation.h - Class for animation via keyframes.
 *
 * Author: Patrick O'Leary
 * Created: August 10, 2008
 * Copyright: 2008. All rights reserved.
 */
#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <deque>

/* Vrui includes */
#include <Geometry/OrthogonalTransformation.h>
#include <GL/GLTransformationWrappers.h>
#include <GLMotif/CascadeButton.h>
#include <GLMotif/Popup.h>
#include <GLMotif/PopupWindow.h>
#include <GLMotif/RadioBox.h>
#include <GLMotif/Slider.h>
#include <GLMotif/TextField.h>
#include <GLMotif/ToggleButton.h>
#include <Misc/CallbackData.h>
#include <Misc/Timer.h>

/* Vrui includes to use the Vrui interface */
#include <Vrui/Vrui.h>

// begin Forward Declarations
class Scene;
class Toirt_Samhlaigh;
class Vector4;
// end Forward Declarations

class Animation: public GLMotif::PopupWindow {
public:
    Animation(Toirt_Samhlaigh & _toirt_samhlaigh, Scene * _scene);
    ~Animation(void);
    void frameStatisticsUpdate(void);
    void keyframeButtonsCallback(GLMotif::ToggleButton::ValueChangedCallbackData * callBackData);
    void keyframeCallback(Misc::CallbackData * callbackData);
    void navigateToCallback(Misc::CallbackData * callbackData);
    void reset(void);
    void resetCallback(Misc::CallbackData * callbackData);
    void setCurrentNavigationTransformation(void);
    Vector4 slerp(Vector4 a, Vector4 b, double t);
    void sliderCallback(GLMotif::Slider::ValueChangedCallbackData * callBackData);
    void toggleSelectCallback(GLMotif::ToggleButton::ValueChangedCallbackData * callBackData);
private:
    bool animating;
    GLMotif::ToggleButton * animatingToggle;
    std::vector<float> animationTime;
    int currentKeyFrame;
    double currentTime;
    GLMotif::Slider * interpolationTimeSlider;
    GLMotif::TextField * interpolationTimeTextField;
    std::deque<Vrui::NavTransform> keyframes;
    std::deque<GLMotif::ToggleButton *> keyframeButtons;
    GLMotif::CascadeButton * keyframesCascadeButton;
    int keyframesIterator;
    GLMotif::TextField * keyframeTextField;
    GLMotif::TextField * scaleTextField;
    Scene * scene;
    Misc::Timer* slerpTimer;
    Toirt_Samhlaigh & toirt_samhlaigh;
    GLMotif::TextField * xAngleTextField;
    GLMotif::TextField * xPositionTextField;
    GLMotif::TextField * yAngleTextField;
    GLMotif::TextField * yPositionTextField;
    GLMotif::TextField * zAngleTextField;
    GLMotif::TextField * zPositionTextField;
    void createCaptureAndResetAndAnimatingRowColumn(GLMotif::RowColumn * animationRowColumn);
    void createKeyFrameAndScaleLabelsRowColumn(GLMotif::RowColumn * animationRowColumn);
    void createKeyFrameAndScaleWidgetsRowColumn(GLMotif::RowColumn * animationRowColumn);
    GLMotif::Popup * createKeyFramesSubMenu(void);
    void createLengthOfInterpolationInSecondsLabelRowColumn(GLMotif::RowColumn * animationRowColumn);
    void createLengthOfInterpolationInSecondsWidgetsRowColumn(GLMotif::RowColumn * animationRowColumn);
    void createNavigateToAndKeyFramesRowColumn(GLMotif::RowColumn * animationRowColumn);
    void createXYZAngleLabelsRowColumn(GLMotif::RowColumn * animationRowColumn);
    void createXYZAngleWidgetsRowColumn(GLMotif::RowColumn * animationRowColumn);
    void createXYZLabelsRowColumn(GLMotif::RowColumn * animationRowColumn);
    void createXYZWidgetsRowColumn(GLMotif::RowColumn * animationRowColumn);
    void initialize(void);
};

#endif /* ANIMATION_H_ */
