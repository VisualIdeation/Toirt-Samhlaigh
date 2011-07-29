/*
 * Animation.cpp - Methods for Animation class.
 *
 * Author: Patrick O'Leary
 * Created: August 10, 2008
 * Copyright: 2008. All rights reserved.
 */

#include <cmath>

/* Vrui includes */
#include <GLMotif/Blind.h>
#include <GLMotif/StyleSheet.h>
#include <GLMotif/SubMenu.h>
#include <GLMotif/WidgetManager.h>

#include <GRAPHIC/Scene.h>
#include <GUI/Animation.h>
#include <MATH/Vector4.h>
#include <UTILITY/Stringify.h>
#include <Toirt_Samhlaigh.h>

/*
 * Animation - Constructor
 */
Animation::Animation(Toirt_Samhlaigh & _toirt_samhlaigh, Scene * _scene) :
    GLMotif::PopupWindow("AnimationPopup", Vrui::getWidgetManager(), "Animation"), animating(false), animatingToggle(NULL),
            currentKeyFrame(0), currentTime(0), scene(_scene), slerpTimer(NULL), toirt_samhlaigh(_toirt_samhlaigh),
            keyframesIterator(0), keyframesCascadeButton(NULL) {
    initialize();
} // end Animation()

/*
 * ~Animation - Destructor
 */
Animation::~Animation(void) {
    // TODO Auto-generated destructor stub
} // end ~Animation()

/*
 * createCaptureAndResetAndAnimatingRowColumn
 *
 * parameter animationRowColumn - GLMotif::RowColumn *
 */
void Animation::createCaptureAndResetAndAnimatingRowColumn(GLMotif::RowColumn * animationRowColumn) {
    GLMotif::RowColumn * captureAndResetAndAnimatingRowColumn = new GLMotif::RowColumn("CaptureAndResetAndAnimatingRowColumn", animationRowColumn, false);
    captureAndResetAndAnimatingRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    captureAndResetAndAnimatingRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    GLMotif::Button * keyFrameButton = new GLMotif::Button("KeyFrameButton", captureAndResetAndAnimatingRowColumn, "Capture");
    keyFrameButton->getSelectCallbacks().add(this, &Animation::keyframeCallback);
    GLMotif::Button * resetButton = new GLMotif::Button("ResetButton", captureAndResetAndAnimatingRowColumn, "Reset");
    resetButton->getSelectCallbacks().add(this, &Animation::resetCallback);
    animatingToggle = new GLMotif::ToggleButton("AnimatingToggle", captureAndResetAndAnimatingRowColumn, "Animating");
    animatingToggle->setToggle(false);
    animatingToggle->getValueChangedCallbacks().add(this, &Animation::toggleSelectCallback);

    captureAndResetAndAnimatingRowColumn->manageChild();
} //end createCaptureAndResetAndAnimatingRowColumn()

/*
 * createKeyFrameAndScaleLabelsRowColumn
 *
 * parameter animationRowColumn - GLMotif::RowColumn *
 */
void Animation::createKeyFrameAndScaleLabelsRowColumn(GLMotif::RowColumn * animationRowColumn) {
    GLMotif::RowColumn * keyFrameAndScaleLabelsRowColumn = new GLMotif::RowColumn("KeyFrameAndScaleLabelsRowColumn", animationRowColumn, false);
    keyFrameAndScaleLabelsRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    keyFrameAndScaleLabelsRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    GLMotif::Label * keyframeLabel = new GLMotif::Label("KeyFrameLabel", keyFrameAndScaleLabelsRowColumn, " KeyFrame ");
    new GLMotif::Blind("Filler", keyFrameAndScaleLabelsRowColumn);
    GLMotif::Label * scaleLabel = new GLMotif::Label("ScaleLabel", keyFrameAndScaleLabelsRowColumn, " Scale ");

    keyFrameAndScaleLabelsRowColumn->manageChild();
} //end createKeyFrameAndScaleLabelsRowColumn()

/*
 * createKeyFrameAndScaleWidgetsRowColumn
 *
 * parameter animationRowColumn - GLMotif::RowColumn *
 */
void Animation::createKeyFrameAndScaleWidgetsRowColumn(GLMotif::RowColumn * animationRowColumn) {
    GLMotif::RowColumn* keyframeAndScaleWidgetsRowColumn = new GLMotif::RowColumn("KeyFrameAndScaleWidgetsRowColumn", animationRowColumn, false);
    keyframeAndScaleWidgetsRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    keyframeAndScaleWidgetsRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    keyframeTextField = new GLMotif::TextField("KeyframeTextField", keyframeAndScaleWidgetsRowColumn, 6);
    keyframeTextField->setFieldWidth(6);
    keyframeTextField->setPrecision(1);
    new GLMotif::Blind("Filler", keyframeAndScaleWidgetsRowColumn);
    scaleTextField = new GLMotif::TextField("ScaleTextField", keyframeAndScaleWidgetsRowColumn, 6);
    scaleTextField->setFieldWidth(6);
    scaleTextField->setPrecision(3);

    keyframeAndScaleWidgetsRowColumn->manageChild();
} //end createKeyFrameAndScaleWidgetsRowColumn()

/*
 * createKeyFramesSubMenu
 *
 * return - GLMotif::Popup *
 */
GLMotif::Popup * Animation::createKeyFramesSubMenu(void) {
    GLMotif::Popup* keyframesSubMenuPopup = new GLMotif::Popup("KeyFramesSubMenuPopup", Vrui::getWidgetManager());
    GLMotif::SubMenu* keyframesSubMenu = new GLMotif::SubMenu("KeyFramesSubMenu", keyframesSubMenuPopup, false);
    if (!keyframes.empty()) {
        keyframeButtons.clear();
        for (int i = 0; i < keyframes.size(); i++) {
            keyframeButtons.push_back(
                    new GLMotif::ToggleButton(Stringify::toString(i).c_str(), keyframesSubMenu, Stringify::toString(i).c_str()));
            keyframeButtons[i]->getValueChangedCallbacks().add(this, &Animation::keyframeButtonsCallback);
        }
    } else {
        new GLMotif::Label("NoFrames", keyframesSubMenu, "No Frames");
    }
    keyframesSubMenu->manageChild();
    return keyframesSubMenuPopup;
} // end createKeyFramesSubMenu()

/*
 * createLengthOfInterpolationInSecondsLabelRowColumn
 *
 * parameter animationRowColumn - GLMotif::RowColumn *
 */
void Animation::createLengthOfInterpolationInSecondsLabelRowColumn(GLMotif::RowColumn * animationRowColumn) {
    GLMotif::RowColumn * lengthOfInterpolationInSecondsLabelRowColumn = new GLMotif::RowColumn("LengthOfInterpolationInSecondsLabelRowColumn", animationRowColumn, false);
    lengthOfInterpolationInSecondsLabelRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    lengthOfInterpolationInSecondsLabelRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    GLMotif::Label * lengthOfInterpolationInSecondsLabel = new GLMotif::Label("lengthOfInterpolationInSecondsLabel", lengthOfInterpolationInSecondsLabelRowColumn, " Length of interpolation in seconds ");

    lengthOfInterpolationInSecondsLabelRowColumn->manageChild();
} //end createLengthOfInterpolationInSecondsLabelRowColumn()

/*
 * createLengthOfInterpolationInSecondsWidgetsRowColumn
 *
 * parameter animationRowColumn - GLMotif::RowColumn *
 */
void Animation::createLengthOfInterpolationInSecondsWidgetsRowColumn(GLMotif::RowColumn * animationRowColumn) {
    GLMotif::RowColumn* lengthOfInterpolationInSecondsWidgetsRowColumn = new GLMotif::RowColumn("LengthOfInterpolationInSecondsWidgetsRowColumn", animationRowColumn, false);
    lengthOfInterpolationInSecondsWidgetsRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    lengthOfInterpolationInSecondsWidgetsRowColumn->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    interpolationTimeTextField = new GLMotif::TextField("InterpolationTimeTextField", lengthOfInterpolationInSecondsWidgetsRowColumn, 6);
    interpolationTimeTextField->setFieldWidth(6);
    interpolationTimeTextField->setPrecision(2);
    interpolationTimeTextField->setValue(2.0);
    const GLMotif::StyleSheet & styleSheet = *Vrui::getWidgetManager()->getStyleSheet();
    interpolationTimeSlider = new GLMotif::Slider("InterpolationTimeSlider", lengthOfInterpolationInSecondsWidgetsRowColumn, GLMotif::Slider::HORIZONTAL, styleSheet.fontHeight * 10.0f);
    interpolationTimeSlider->setValueRange(1.0, 5.0, 0.5);
    interpolationTimeSlider->setValue(2);
    interpolationTimeSlider->getValueChangedCallbacks().add(this, &Animation::sliderCallback);

    lengthOfInterpolationInSecondsWidgetsRowColumn->manageChild();
} //end createLengthOfInterpolationInSecondsWidgetsRowColumn()

/*
 * createNavigateToAndKeyFramesRowColumn
 *
 * parameter animationRowColumn - GLMotif::RowColumn *
 */
void Animation::createNavigateToAndKeyFramesRowColumn(GLMotif::RowColumn * animationRowColumn) {
    GLMotif::RowColumn * navigateToAndKeyFramesRowColumn = new GLMotif::RowColumn("NavigateToAndKeyFramesRowColumn", animationRowColumn, false);
    navigateToAndKeyFramesRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    navigateToAndKeyFramesRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    new GLMotif::Blind("Filler", navigateToAndKeyFramesRowColumn);
    GLMotif::Button * navigateToButton = new GLMotif::Button("NavigateToButton", navigateToAndKeyFramesRowColumn, "Navigate To");
    navigateToButton->getSelectCallbacks().add(this, &Animation::navigateToCallback);
    keyframesCascadeButton = new GLMotif::CascadeButton("KeyFramesCascadeButton", navigateToAndKeyFramesRowColumn, "KeyFrames");
    keyframesCascadeButton->setPopup(createKeyFramesSubMenu());

    navigateToAndKeyFramesRowColumn->manageChild();
} //end createNavigateToAndKeyFramesRowColumn()

/*
 * createXYZAngleLabelsRowColumn
 *
 * parameter animationRowColumn - GLMotif::RowColumn *
 */
void Animation::createXYZAngleLabelsRowColumn(GLMotif::RowColumn * animationRowColumn) {
    GLMotif::RowColumn* xyzAngleLabelsRowColumn = new GLMotif::RowColumn("XYZAngleLabelsRowColumn", animationRowColumn, false);
    xyzAngleLabelsRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    xyzAngleLabelsRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    GLMotif::Label * xAngleLabel = new GLMotif::Label("xAngleLabel", xyzAngleLabelsRowColumn, " X Angle ");
    GLMotif::Label * yAngleLabel = new GLMotif::Label("yAngleLabel", xyzAngleLabelsRowColumn, " Y Angle ");
    GLMotif::Label * zAngleLabel = new GLMotif::Label("zAngleLabel", xyzAngleLabelsRowColumn, " Z Angle ");

    xyzAngleLabelsRowColumn->manageChild();
} //end createXYZAngleLabelsRowColumn()

/*
 * createXYZAngleWidgetsRowColumn
 *
 * parameter animationRowColumn - GLMotif::RowColumn *
 */
void Animation::createXYZAngleWidgetsRowColumn(GLMotif::RowColumn * animationRowColumn) {
    GLMotif::RowColumn* xyzAngleWidgetsRowColumn = new GLMotif::RowColumn("XYZAngleWidgetsRowColumn", animationRowColumn, false);
    xyzAngleWidgetsRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    xyzAngleWidgetsRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);

    xAngleTextField = new GLMotif::TextField("XAngleTextField", xyzAngleWidgetsRowColumn, 6);
    xAngleTextField->setFieldWidth(6);
    xAngleTextField->setPrecision(3);
    yAngleTextField = new GLMotif::TextField("YAngleTextField", xyzAngleWidgetsRowColumn, 6);
    yAngleTextField->setFieldWidth(6);
    yAngleTextField->setPrecision(3);
    zAngleTextField = new GLMotif::TextField("ZAngleTextField", xyzAngleWidgetsRowColumn, 6);
    zAngleTextField->setFieldWidth(6);
    zAngleTextField->setPrecision(3);

    xyzAngleWidgetsRowColumn->manageChild();
} //end createXYZAngleWidgetsRowColumn()

/*
 * createXYZLabelsRowColumn
 *
 * parameter animationRowColumn - GLMotif::RowColumn *
 */
void Animation::createXYZLabelsRowColumn(GLMotif::RowColumn * animationRowColumn) {
    GLMotif::RowColumn* xyzLabelsRowColumn = new GLMotif::RowColumn("XYZLabelsRowColumn", animationRowColumn, false);
    xyzLabelsRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    xyzLabelsRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    GLMotif::Label * xLabel = new GLMotif::Label("XLabel", xyzLabelsRowColumn, " X ");
    GLMotif::Label * yLabel = new GLMotif::Label("YLabel", xyzLabelsRowColumn, " Y ");
    GLMotif::Label * zLabel = new GLMotif::Label("ZLabel", xyzLabelsRowColumn, " Z ");

    xyzLabelsRowColumn->manageChild();
} //end createXYZLabelsRowColumn()

/*
 * createXYZWidgetsRowColumn
 *
 * parameter animationRowColumn - GLMotif::RowColumn *
 */
void Animation::createXYZWidgetsRowColumn(GLMotif::RowColumn * animationRowColumn) {
    GLMotif::RowColumn* xyzWidgetsRowColumn = new GLMotif::RowColumn("XYZAngleWidgetsRowColumn", animationRowColumn, false);
    xyzWidgetsRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    xyzWidgetsRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    xPositionTextField = new GLMotif::TextField("XPositionTextField", xyzWidgetsRowColumn, 6);
    xPositionTextField->setFieldWidth(6);
    xPositionTextField->setPrecision(4);
    yPositionTextField = new GLMotif::TextField("YPositionTextField", xyzWidgetsRowColumn, 6);
    yPositionTextField->setFieldWidth(6);
    yPositionTextField->setPrecision(4);
    zPositionTextField = new GLMotif::TextField("ZPositionTextField", xyzWidgetsRowColumn, 6);
    zPositionTextField->setFieldWidth(6);
    zPositionTextField->setPrecision(4);

    xyzWidgetsRowColumn->manageChild();
} //end createXYZWidgetsRowColumn()

/*
 * frameStatisticsUpdate
 */
void Animation::frameStatisticsUpdate(void) {
    Vrui::Vector translation = keyframes[currentKeyFrame].getTranslation();
    const double * quaternion = keyframes[currentKeyFrame].getRotation().getQuaternion();
    xPositionTextField->setValue(translation[0]);
    yPositionTextField->setValue(translation[1]);
    zPositionTextField->setValue(translation[2]);
    zAngleTextField->setValue(asin(2 * quaternion[0] * quaternion[1] + 2 * quaternion[2] * quaternion[3]));
    yAngleTextField->setValue(atan2(2 * quaternion[1] * quaternion[3] - 2 * quaternion[0] * quaternion[2], 1 - 2 * quaternion[1]
            * quaternion[1] - 2 * quaternion[2] * quaternion[2]));
    xAngleTextField->setValue(atan2(2 * quaternion[0] * quaternion[3] - 2 * quaternion[1] * quaternion[2], 1 - 2 * quaternion[0]
            * quaternion[0] - 2 * quaternion[2] * quaternion[2]));
    scaleTextField->setValue(keyframes[currentKeyFrame].getScaling());
    interpolationTimeSlider->setValue(animationTime[currentKeyFrame]);
    interpolationTimeTextField->setValue(animationTime[currentKeyFrame]);
    keyframeTextField->setValue(currentKeyFrame);
    Vrui::requestUpdate();
} // end frameStatisticsUpdate()

/*
 * initialize
 */
void Animation::initialize(void) {
    GLMotif::RowColumn * animationRowColumn = new GLMotif::RowColumn("AnimationRowColumn", this, false);
    animationRowColumn->setOrientation(GLMotif::RowColumn::VERTICAL);
    createCaptureAndResetAndAnimatingRowColumn(animationRowColumn);
    createKeyFrameAndScaleLabelsRowColumn(animationRowColumn);
    createKeyFrameAndScaleWidgetsRowColumn(animationRowColumn);
    createXYZLabelsRowColumn(animationRowColumn);
    createXYZWidgetsRowColumn(animationRowColumn);
    createXYZAngleLabelsRowColumn(animationRowColumn);
    createXYZAngleWidgetsRowColumn(animationRowColumn);
    createLengthOfInterpolationInSecondsLabelRowColumn(animationRowColumn);
    createLengthOfInterpolationInSecondsWidgetsRowColumn(animationRowColumn);
    createNavigateToAndKeyFramesRowColumn(animationRowColumn);
    animationRowColumn->manageChild();
} // end initialize()

/*
 * keyframeButtonsCallback
 *
 * parameter callbackData - GLMotif::ToggleButton::ValueChangedCallbackData *
 */
void Animation::keyframeButtonsCallback(GLMotif::ToggleButton::ValueChangedCallbackData * callbackData) {
    currentKeyFrame = Stringify::toInt(callbackData->toggle->getName());
    for (int i = 0; i < keyframeButtons.size(); i++) {
        if (i != currentKeyFrame) {
            keyframeButtons[i]->setToggle(false);
        }
    }
    frameStatisticsUpdate();
} // end keyframeButtonsCallback()

/*
 * keyframeCallback
 *
 * parameter callbackData - Misc::CallbackData *
 */
void Animation::keyframeCallback(Misc::CallbackData * callbackData) {
    keyframes.push_back(Vrui::getNavigationTransformation());
    animationTime.push_back(2.0);
    currentKeyFrame = keyframes.size() - 1;
    keyframesCascadeButton->setPopup(createKeyFramesSubMenu());
    scene->setSphere(Vrui::getHeadPosition());
    frameStatisticsUpdate();
} // end keyframeCallback()

/*
 * navigateToCallback
 *
 * parameter callbackData - Misc::CallbackData *
 */
void Animation::navigateToCallback(Misc::CallbackData * callbackData) {
    Vrui::setNavigationTransformation(keyframes[currentKeyFrame]);
} // end navigateToCallback()

/*
 * reset
 */
void Animation::reset(void) {
    if (animating) {
        animatingToggle->setToggle(false);
        animating = false;
        scene->setAnimating(false);
    }
    keyframes.clear();
    xPositionTextField->setLabel("");
    yPositionTextField->setLabel("");
    zPositionTextField->setLabel("");
    xAngleTextField->setLabel("");
    yAngleTextField->setLabel("");
    zAngleTextField->setLabel("");
    scaleTextField->setLabel("");
    keyframeTextField->setLabel("");
    interpolationTimeSlider->setValue(2);
    interpolationTimeTextField->setValue(2.0);
    keyframeButtons.clear();
    animationTime.clear();
    scene->clearSpheres();
    keyframesCascadeButton->setPopup(createKeyFramesSubMenu());
} // end reset()

/*
 * resetCallback
 *
 * parameter callbackData - Misc::CallbackData *
 */
void Animation::resetCallback(Misc::CallbackData * callbackData) {
    reset();
} // end resetCallback()

/*
 * setCurrentNavigationTransformation
 */
void Animation::setCurrentNavigationTransformation(void) {
    float currentAnimationTime = animationTime[keyframesIterator];
    if (currentTime > currentAnimationTime) {
        currentTime = 0;
        keyframesIterator += 1;
    }
    if (keyframesIterator > (int) keyframes.size() - 2)
        keyframesIterator = 0;
    Vrui::NavTransform a = keyframes[keyframesIterator];
    Vrui::NavTransform b = keyframes[keyframesIterator + 1];
    Vrui::Vector translationA = a.getTranslation();
    Vrui::Vector translationB = b.getTranslation();
    Vrui::Rotation rotationA = a.getRotation();
    Vrui::Rotation rotationB = b.getRotation();
    Vector4 quaternionA(rotationA.getQuaternion()[0], rotationA.getQuaternion()[1], rotationA.getQuaternion()[2],
            rotationA.getQuaternion()[3]);
    Vector4 quaternionB(rotationB.getQuaternion()[0], rotationB.getQuaternion()[1], rotationB.getQuaternion()[2],
            rotationB.getQuaternion()[3]);
    float scalingA = a.getScaling();
    float scalingB = b.getScaling();
    if (slerpTimer->peekTime() > 0.033) {
        currentTime += slerpTimer->peekTime();
        Vrui::Vector translation = a.getTranslation();
        if (translationA != translationB) {
            translation[0] = (currentAnimationTime - currentTime) / currentAnimationTime * translationA[0] + currentTime
                    / currentAnimationTime * translationB[0];
            translation[1] = (currentAnimationTime - currentTime) / currentAnimationTime * translationA[1] + currentTime
                    / currentAnimationTime * translationB[1];
            translation[2] = (currentAnimationTime - currentTime) / currentAnimationTime * translationA[2] + currentTime
                    / currentAnimationTime * translationB[2];
        }
        float scaling = a.getScaling();
        if (scalingA != scalingB) {
            scaling = ((currentAnimationTime - currentTime) / currentAnimationTime * scalingA + currentTime / currentAnimationTime
                    * scalingB);
        }
        Vrui::Rotation rotation = a.getRotation();
        if (rotationA != rotationB) {
            Vector4 result = slerp(quaternionA, quaternionB, currentTime / currentAnimationTime);
            rotation = rotation.fromQuaternion(result.getX(), result.getY(), result.getZ(), result.getW());
        }
        Vrui::NavTransform currentNavigationTransformation =
                Vrui::NavTransform((const Vrui::Vector) translation, rotation, scaling);
        Vrui::setNavigationTransformation(currentNavigationTransformation);
        slerpTimer->elapse();
    }
} // end setCurrentNavigationTransformation()

/*
 * slerp
 *
 * parameter a - Vector4
 * parameter b - Vector4
 * parameter t - double
 * return - Vector4
 */
Vector4 Animation::slerp(Vector4 a, Vector4 b, double t) {
    const double THRESHOLD = 0.9995;
    double dot = a.dot(b);
    if (dot > THRESHOLD) { // If the inputs are too close, linearly interpolate and normalize the result.
        b.subtract(a);
        b.multiply(t);
        a.add(b);
        a.unitize();
        return a;
    } else {
        if (dot > 1)
            dot = 1;
        else if (dot < -1)
            dot = -1;
        double theta_0 = acos(dot);
        double theta = theta_0 * t;
        Vector4 q1 = a;
        q1.multiply(dot);
        b.subtract(q1);
        b.unitize();
        a.multiply(cos(theta));
        b.multiply(sin(theta));
        a.add(b);
        return a;
    }
}

/*
 * sliderCallback
 *
 * parameter callbackData - GLMotif::Slider::ValueChangedCallbackData *
 */
void Animation::sliderCallback(GLMotif::Slider::ValueChangedCallbackData * callbackData) {
    if (strcmp(callbackData->slider->getName(), "InterpolationTimeSlider") == 0) {
        if (keyframes.size() > 0) {
            animationTime[currentKeyFrame] = callbackData->value;
            interpolationTimeTextField->setValue(animationTime[currentKeyFrame]);
            Vrui::requestUpdate();
        }
    }
} // end sliderCallback()

/*
 * toggleSelectCallback - Adjust the gui/program state based on which toggle button changed state.
 *
 * parameter callbackData - GLMotif::ToggleButton::ValueChangedCallbackData *
 */
void Animation::toggleSelectCallback(GLMotif::ToggleButton::ValueChangedCallbackData * callbackData) {
    /* Adjust gui/program state based on which toggle button changed state: */
    if (strcmp(callbackData->toggle->getName(), "AnimatingToggle") == 0) {
        if (!keyframes.empty()) {
            animating = !animating;
            toirt_samhlaigh.setAnimating(animating);
            scene->setAnimating(animating);
            if (animating)
                slerpTimer = new Misc::Timer;
            else {
                delete slerpTimer;
                keyframesIterator = 0;
                currentTime = 0;
            }
        }
    }
    Vrui::requestUpdate();
} // end toggleSelectCallback()
