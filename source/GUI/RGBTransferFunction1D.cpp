/*
 * RGBTransferFunction1D.cpp - Methods for RGBTransferFunction1D class.
 *
 * Author: Patrick O'Leary
 * Created: January 21, 2008
 * Copyright: 2008. All rights reserved.
 */
#include <iostream>

/* Vrui includes */
#include <GL/GLColor.h>
#include <GLMotif/Label.h>
#include <GLMotif/Button.h>
#include <GLMotif/WidgetManager.h>
#include <Misc/File.h>

/* Vrui includes to use the Vrui interface */
#include <Vrui/Vrui.h>

#include <DATA/Volume.h>
#include <GUI/RGBTransferFunction1D.h>
#include <GUI/ScalarWidget.h>
#include <GUI/ScalarWidgetControlPointChangedCallbackData.h>
#include <Toirt_Samhlaigh.h>

/*
 * RGBTransferFunction1D - Constructor for RGBTransferFunction1D class.
 * 		extends GLMotif::PopupWindow
 *
 * parameter volume - Volume*
 */
RGBTransferFunction1D::RGBTransferFunction1D(Toirt_Samhlaigh & _toirt_samhlaigh, Volume * volume) :
    GLMotif::PopupWindow("RGBTransferFunction1DPopup", Vrui::getWidgetManager(), "RGB Components 1D Transfer Function Editor"), interactive(false), toirt_samhlaigh(_toirt_samhlaigh) {
    initialize(volume);
}

/*
 * ~RGBTransferFunction1D - Destructor for RGBTransferFunction1D class.
 */
RGBTransferFunction1D::~RGBTransferFunction1D(void) {
}

/*
 * blueToggleButtonCallback
 *
 * parameter callBackData - GLMotif::ToggleButton::ValueChangedCallbackData*
 */
void RGBTransferFunction1D::blueToggleButtonCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData) {
    if (strcmp(callBackData->toggle->getName(), "BlueToggleButton") == 0) {
        redToggleButton->setToggle(false);
        greenToggleButton->setToggle(false);
        blueToggleButton->setToggle(true);
        scalarWidget->setComponent(2);
        gaussianToggleButton->setToggle(scalarWidget->getGaussian());
    }
    Vrui::requestUpdate();
} // end blueToggleButtonCallback()

/*
 * changeBlue - Change the blue ramp.
 *
 * parameter ramp - int
 */
void RGBTransferFunction1D::changeBlue(int ramp) const {
    scalarWidget->create(ramp, 2);
} // end changeBlue()

/*
 * changeGreen - Change the green ramp.
 *
 * parameter ramp - int
 */
void RGBTransferFunction1D::changeGreen(int ramp) const {
    scalarWidget->create(ramp, 1);
} // end changeGreen()

/*
 * changeRed - Change the red ramp.
 *
 * parameter ramp - int
 */
void RGBTransferFunction1D::changeRed(int ramp) const {
    scalarWidget->create(ramp, 0);
} // end changeRed()

/*
 * clamp
 *
 * parameter _value - int
 * parameter _minimum - int
 * parameter _maximum - int
 *
 * return - unsigned char
 */
unsigned char RGBTransferFunction1D::clamp(int _value, int _minimum, int _maximum) {
    if (_value > _maximum)
        _value = _maximum;
    if (_value < _minimum)
        _value = _minimum;
    return (unsigned char) _value;
} // end clamp()

/*
 * createButtonBox - Create a box to hold buttons.
 *
 * parameter colorMapDialog - GLMotif::RowColumn*&
 * return - GLMotif::RowColumn*
 */
GLMotif::RowColumn* RGBTransferFunction1D::createButtonBox(GLMotif::RowColumn*& colorMapDialog) {
    GLMotif::RowColumn* buttonBox = new GLMotif::RowColumn("ButtonBox", colorMapDialog, false);
    buttonBox->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    buttonBox->setPacking(GLMotif::RowColumn::PACK_GRID);
    GLMotif::Button* removeControlPointButton = new GLMotif::Button("RemoveControlPointButton", buttonBox, "Remove Control Point");
    removeControlPointButton->getSelectCallbacks().add(this, &RGBTransferFunction1D::removeScalarWidgetControlPointCallback);
    gaussianToggleButton = new GLMotif::ToggleButton("GaussianToggleButton", buttonBox, "Gaussian");
    gaussianToggleButton->setToggle(false);
    gaussianToggleButton->getValueChangedCallbacks().add(this, &RGBTransferFunction1D::gaussianToggleButtonCallback);
    interactiveToggleButton = new GLMotif::ToggleButton("InteractiveToggleButton", buttonBox, "Interactive");
    interactiveToggleButton->setToggle(false);
    interactiveToggleButton->getValueChangedCallbacks().add(this, &RGBTransferFunction1D::interactiveToggleButtonCallback);
    return buttonBox;
} // end createButtonBox()

/*
 * createRGBButtonBox - Create a box to hold buttons.
 *
 * parameter colorMapDialog - GLMotif::RowColumn*&
 * return - GLMotif::RowColumn*
 */
GLMotif::RowColumn* RGBTransferFunction1D::createRGBButtonBox(GLMotif::RowColumn*& colorMapDialog) {
    GLMotif::RowColumn* rgbButtonBox = new GLMotif::RowColumn("RGBButtonBox", colorMapDialog, false);
    rgbButtonBox->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    rgbButtonBox->setPacking(GLMotif::RowColumn::PACK_GRID);
    redToggleButton = new GLMotif::ToggleButton("RedToggleButton", rgbButtonBox, "Red");
    redToggleButton->setToggle(true);
    redToggleButton->getValueChangedCallbacks().add(this, &RGBTransferFunction1D::redToggleButtonCallback);
    greenToggleButton = new GLMotif::ToggleButton("GreenToggleButton", rgbButtonBox, "Green");
    greenToggleButton->setToggle(false);
    greenToggleButton->getValueChangedCallbacks().add(this, &RGBTransferFunction1D::greenToggleButtonCallback);
    blueToggleButton = new GLMotif::ToggleButton("BlueToggleButton", rgbButtonBox, "Blue");
    blueToggleButton->setToggle(false);
    blueToggleButton->getValueChangedCallbacks().add(this, &RGBTransferFunction1D::blueToggleButtonCallback);
    return rgbButtonBox;
} // end createRGBButtonBox()

/*
 * createRGBTransferFunction1D - Create the transfer function 1D.
 *
 * parameter redRampCreationType - int
 * parameter greenRampCreationType - int
 * parameter blueRampCreationType - int
 * parameter _minimum - double
 * parameter _maximum - double
 */
void RGBTransferFunction1D::createRGBTransferFunction1D(int redRampCreationType, int greenRampCreationType,
        int blueRampCreationType, double _minimum, double _maximum) {
    scalarWidget->create(redRampCreationType, _minimum, _maximum, 0);
    scalarWidget->create(greenRampCreationType, _minimum, _maximum, 1);
    scalarWidget->create(blueRampCreationType, _minimum, _maximum, 2);
} // end createRGBRGBTransferFunction1D()

/*
 * createScalarWidgetDialog - Create color map dialog.
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter volume - Volume*
 */
void RGBTransferFunction1D::createScalarWidgetDialog(const GLMotif::StyleSheet& styleSheet, Volume* volume) {
    GLMotif::RowColumn* scalarComponentDialog = new GLMotif::RowColumn("ScalarWidgetDialog", this, false);
    GLMotif::RowColumn* rgbButtonBox = createRGBButtonBox(scalarComponentDialog);
    rgbButtonBox->manageChild();
    createScalarWidget(styleSheet, scalarComponentDialog, volume);
    GLMotif::RowColumn* buttonBox = createButtonBox(scalarComponentDialog);
    buttonBox->manageChild();
    scalarComponentDialog->manageChild();
} // end createScalarWidgetDialog()

/*
 * createScalarWidget - Create blue component.
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter scalarComponentDialog - GLMotif::RowColumn*&
 * parameter volume - Volume*
 */
void RGBTransferFunction1D::createScalarWidget(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& scalarComponentDialog,
        Volume* volume) {
    scalarWidget = new ScalarWidget("ScalarWidget", scalarComponentDialog, volume, 0);
    scalarWidget->setBorderWidth(styleSheet.size * 0.5f);
    scalarWidget->setBorderType(GLMotif::Widget::LOWERED);
    scalarWidget->setForegroundColor(GLMotif::Color(0.0f, 1.0f, 0.0f));
    scalarWidget->setMarginWidth(styleSheet.size);
    scalarWidget->setPreferredSize(GLMotif::Vector(styleSheet.fontHeight * 20.0, styleSheet.fontHeight * 10.0, 0.0f));
    scalarWidget->setControlPointSize(styleSheet.size);
    scalarWidget->setControlPointScalar(1.0f);
    scalarWidget->getControlPointChangedCallbacks().add(this, &RGBTransferFunction1D::scalarWidgetControlPointChangedCallback);
} // end createScalarWidget()

/*
 * exportBlue - Export the blue.
 *
 * parameter colormap - unsigned char*
 */
void RGBTransferFunction1D::exportBlue(unsigned char* colormap) const {
    scalarWidget->exportScalar(colormap);
} // end exportBlue()

/*
 * exportGreen - Export the green.
 *
 * parameter colormap - unsigned char*
 */
void RGBTransferFunction1D::exportGreen(unsigned char* colormap) const {
    scalarWidget->exportScalar(colormap, 1);
} // end exportGreen()

/*
 * exportRed - Export the red.
 *
 * parameter colormap - unsigned char*
 */
void RGBTransferFunction1D::exportRed(unsigned char* colormap) const {
    scalarWidget->exportScalar(colormap, 0);
} // end exportRed()

/*
 * exportPreIntegrationColorMap
 *
 * parameter colormap - unsigned char*
 */
void RGBTransferFunction1D::exportPreIntegrationColorMap(unsigned char* colormap) {
    scalarWidget->exportScalar(colormap, 0);
    scalarWidget->exportScalar(colormap, 1);
    scalarWidget->exportScalar(colormap, 2);
    double r = 0;
    double g = 0;
    double b = 0;
    double a = 0;
    int rcol;
    int gcol;
    int bcol;
    int acol;
    double* rInt = new double[256];
    double* gInt = new double[256];
    double* bInt = new double[256];
    double* aInt = new double[256];
    int smin;
    int smax;
    double factor;
    int lookupIndex = 0;
    unsigned char* colorTable = new unsigned char[4 * 256];
    for (int i = 0; i < 256; i++) {
        colorTable[4 * i + 0] = colormap[4 * (i * 256 + i) + 0];
        colorTable[4 * i + 1] = colormap[4 * (i * 256 + i) + 1];
        colorTable[4 * i + 2] = colormap[4 * (i * 256 + i) + 2];
        colorTable[4 * i + 3] = colorTable[4 * i + 0] + colorTable[4 * i + 1] + colorTable[4 * i + 2];
    }
    rInt[0] = 0;
    gInt[0] = 0;
    bInt[0] = 0;
    aInt[0] = 0;
    for (int i = 1; i < 256; i++) {
        r = r + ((double(colorTable[4 * (i - 1) + 0]) + double(colorTable[4 * i + 0])) / 2.0);
        g = g + ((double(colorTable[4 * (i - 1) + 1]) + double(colorTable[4 * i + 1])) / 2.0);
        b = b + ((double(colorTable[4 * (i - 1) + 2]) + double(colorTable[4 * i + 2])) / 2.0);
        a = a + ((double(colorTable[4 * (i - 1) + 3]) + double(colorTable[4 * i + 3])) / 2.0);
        rInt[i] = r;
        gInt[i] = g;
        bInt[i] = b;
        aInt[i] = a;
    }
    for (int sb = 0; sb < 256; sb++) {
        for (int sf = 0; sf < 256; sf++) {
            if (sb < sf) {
                smin = sb;
                smax = sf;
            } // end if
            else {
                smin = sf;
                smax = sb;
            } // end else

            if (smax != smin) {
                factor = 1.0 / (double) (smax - smin);
                rcol = int((rInt[smax] - rInt[smin]) * factor);
                gcol = int((gInt[smax] - gInt[smin]) * factor);
                bcol = int((bInt[smax] - bInt[smin]) * factor);
                acol = int(256.0 * (1.0 - exp((-1.0 * (aInt[smax] - aInt[smin]) * factor) / 255.0)));
            } // end if
            else {
                rcol = int(colorTable[4 * smin + 0]);
                gcol = int(colorTable[4 * smin + 1]);
                bcol = int(colorTable[4 * smin + 2]);
                acol = int(colorTable[4 * smin + 3]);
            } // end else

            colormap[lookupIndex++] = clamp(rcol, 0, 255);
            colormap[lookupIndex++] = clamp(gcol, 0, 255);
            colormap[lookupIndex++] = clamp(bcol, 0, 255);
            colormap[lookupIndex++] = clamp(acol, 0, 255);
        } // end for
    } // end for
    delete[] rInt;
    delete[] gInt;
    delete[] bInt;
    delete[] aInt;
    delete[] colorTable;
} // end exportPreIntegrationColorMap()

/*
 * gaussianToggleButtonCallback
 *
 * parameter callBackData - GLMotif::ToggleButton::ValueChangedCallbackData*
 */
void RGBTransferFunction1D::gaussianToggleButtonCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData) {
    if (strcmp(callBackData->toggle->getName(), "GaussianToggleButton") == 0) {
        scalarWidget->setGaussian(callBackData->set);
    }
    Vrui::requestUpdate();
} // end gaussianToggleButtonCallback()

/*
 * greenToggleButtonCallback
 *
 * parameter callBackData - GLMotif::ToggleButton::ValueChangedCallbackData*
 */
void RGBTransferFunction1D::greenToggleButtonCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData) {
    if (strcmp(callBackData->toggle->getName(), "GreenToggleButton") == 0) {
        redToggleButton->setToggle(false);
        greenToggleButton->setToggle(true);
        blueToggleButton->setToggle(false);
        scalarWidget->setComponent(1);
        gaussianToggleButton->setToggle(scalarWidget->getGaussian());
    }
    Vrui::requestUpdate();
} // end greenToggleButtonCallback()

/*
 * getBlueChangedCallbacks
 *
 * return - Misc::CallbackList&
 */
Misc::CallbackList& RGBTransferFunction1D::getBlueChangedCallbacks(void) {
    return scalarWidget->getChangedCallbacks();
} // end getBlueChangedCallbacks()

/*
 * isDragging
 *
 * return - bool
 */
bool RGBTransferFunction1D::isDragging(void) const {
    if (scalarWidget->isDragging())
        return true;
    return false;
} // end isDragging()

/*
 * isInteractive
 *
 * return - bool
 */
bool RGBTransferFunction1D::isInteractive(void) {
    return interactive;
} // end isInteractive()

/*
 * setInteractive
 *
 * parameter interactive - bool
 */
void RGBTransferFunction1D::setInteractive(bool interactive) {
    this->interactive = interactive;
    interactiveToggleButton->setToggle(interactive);
} // end setInteractive()

/*
 * getGreenChangedCallbacks
 *
 * return - Misc::CallbackList&
 */
Misc::CallbackList& RGBTransferFunction1D::getGreenChangedCallbacks(void) {
    return scalarWidget->getChangedCallbacks();
} // end getGreenChangedCallbacks()

/*
 * getRedChangedCallbacks
 *
 * return - Misc::CallbackList&
 */
Misc::CallbackList& RGBTransferFunction1D::getRedChangedCallbacks(void) {
    return scalarWidget->getChangedCallbacks();
} // end getRedChangedCallbacks()

/*
 * initialize - Initialize the GUI for the RGBRGBTransferFunction1D class.
 *
 * parameter volume - Volume*
 */
void RGBTransferFunction1D::initialize(Volume* volume) {
    const GLMotif::StyleSheet& styleSheet = *Vrui::getWidgetManager()->getStyleSheet();
    createScalarWidgetDialog(styleSheet, volume);
}

/*
 * interactiveToggleButtonCallback
 *
 * parameter callBackData - GLMotif::ToggleButton::ValueChangedCallbackData*
 */
void RGBTransferFunction1D::interactiveToggleButtonCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData) {
    if (strcmp(callBackData->toggle->getName(), "InteractiveToggleButton") == 0) {
        if (!toirt_samhlaigh.isInteractive()) {
            interactive = callBackData->set;
        } else {
            interactive = true;
            interactiveToggleButton->setToggle(interactive);
        }
    }
    Vrui::requestUpdate();
} // end interactiveToggleButtonCallback()

/*
 * loadBlueTransferFunction1D - Load a blue component transfer function 1D.
 *
 * parameter fileName - const char*
 */
void RGBTransferFunction1D::loadBlueTransferFunction1D(const char* fileName) {
    scalarWidget->load(fileName);
} // end loadBlueTransferFunction1D()

/*
 * loadGreenTransferFunction1D - Load a green component transfer function 1D.
 *
 * parameter fileName - const char*
 */
void RGBTransferFunction1D::loadGreenTransferFunction1D(const char* fileName) {
    scalarWidget->load(fileName);
} // end loadGreenTransferFunction1D()

/*
 * loadRedTransferFunction1D - Load a red component transfer function 1D.
 *
 * parameter fileName - const char*
 */
void RGBTransferFunction1D::loadRedTransferFunction1D(const char* fileName) {
    scalarWidget->load(fileName);
} // end loadRedTransferFunction1D()

/*
 * redToggleButtonCallback
 *
 * parameter callBackData - GLMotif::ToggleButton::ValueChangedCallbackData*
 */
void RGBTransferFunction1D::redToggleButtonCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData) {
    if (strcmp(callBackData->toggle->getName(), "RedToggleButton") == 0) {
        redToggleButton->setToggle(true);
        greenToggleButton->setToggle(false);
        blueToggleButton->setToggle(false);
        scalarWidget->setComponent(0);
        gaussianToggleButton->setToggle(scalarWidget->getGaussian());
    }
    Vrui::requestUpdate();
} // end redToggleButtonCallback()

/*
 * removeScalarWidgetControlPointCallback - Remove the current scalar widget control point.
 *
 * parameter _callbackData - Misc::CallbackData*
 */
void RGBTransferFunction1D::removeScalarWidgetControlPointCallback(Misc::CallbackData* _callbackData) {
    scalarWidget->deleteControlPoint();
} // end removeScalarWidgetControlPointCallback()

/*
 * saveBlueTransferFunction1D - Save the blue transfer function 1D.
 *
 * parameter fileName - const char*
 */
void RGBTransferFunction1D::saveBlueTransferFunction1D(const char* fileName) const {
    scalarWidget->save(fileName);
} // end saveBlueTransferFunction1D()

/*
 * saveGreenTransferFunction1D - Save the green transfer function 1D.
 *
 * parameter fileName - const char*
 */
void RGBTransferFunction1D::saveGreenTransferFunction1D(const char* fileName) const {
    scalarWidget->save(fileName);
} // end saveGreenTransferFunction1D()

/*
 * saveRedTransferFunction1D - Save the red transfer function 1D.
 *
 * parameter fileName - const char*
 */
void RGBTransferFunction1D::saveRedTransferFunction1D(const char* fileName) const {
    scalarWidget->save(fileName);
} // end saveRedTransferFunction1D()

/*
 * scalarWidgetControlPointChangedCallback - Callback to select a blue control point.
 *
 * parameter _callbackData - Misc::CallbackData*
 */
void RGBTransferFunction1D::scalarWidgetControlPointChangedCallback(Misc::CallbackData* _callbackData) {
    ScalarWidgetControlPointChangedCallbackData* callbackData =
            static_cast<ScalarWidgetControlPointChangedCallbackData*> (_callbackData);
    if (callbackData->getCurrentControlPoint() != 0) {
        float value = scalarWidget->getControlPointScalar();
    }
} // end scalarWidgetControlPointChangedCallback()
