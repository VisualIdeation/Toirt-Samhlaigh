/*
 * TransferFunction1D.cpp - Methods for TransferFunction1D class.
 *
 * Author: Patrick O'Leary
 * Created: October 28, 2007
 * Copyright: 2007
 */
#include <iostream>
#include <cmath>

/* Vrui includes */
#include <GL/GLColor.h>
#include <GLMotif/Label.h>
#include <GLMotif/Button.h>
#include <GLMotif/WidgetManager.h>
#include <Misc/File.h>

/* Vrui includes to use the Vrui interface */
#include <Vrui/Vrui.h>

#include <COLORMAP/ColorMap.h>
#include <COLORMAP/ControlPointChangedCallbackData.h>
#include <COLORMAP/RGBAColor.h>
#include <COLORMAP/Storage.h>
#include <DATA/Volume.h>
#include <GUI/ScalarWidget.h>
#include <GUI/ScalarWidgetControlPointChangedCallbackData.h>
#include <GUI/SwatchesWidget.h>
#include <GUI/TransferFunction1D.h>
#include <Toirt_Samhlaigh.h>

/*
 * TransferFunction1D - Constructor for TransferFunction1D class.
 * 		extends GLMotif::PopupWindow
 *
 * parameter volume - Volume*
 */
TransferFunction1D::TransferFunction1D(Toirt_Samhlaigh & _toirt_samhlaigh, Volume * volume) :
    GLMotif::PopupWindow("TransferFunction1DPopup", Vrui::getWidgetManager(), "1D Transfer Function Editor"), interactive(false), toirt_samhlaigh(_toirt_samhlaigh) {
    initialize(volume);
}

/*
 * ~TransferFunction1D - Destructor for TransferFunction1D class.
 */
TransferFunction1D::~TransferFunction1D(void) {
}

/*
 * changeAlpha - Change the alpha ramp.
 *
 * parameter ramp - int
 */
void TransferFunction1D::changeAlpha(int ramp) const {
    alphaComponent->create(ramp);
} // end changeAlpha()

/*
 * changeColorMap - Change the color map.
 *
 * parameter colormap - int
 */
void TransferFunction1D::changeColorMap(int colormap) const {
    colorMap->createColorMap(colormap);
} // end changeColorMap()

/*
 * clamp
 *
 * parameter _value - int
 * parameter _minimum - int
 * parameter _maximum - int
 *
 * return - unsigned char
 */
unsigned char TransferFunction1D::clamp(int _value, int _minimum, int _maximum) {
    if (_value > _maximum)
        _value = _maximum;
    if (_value < _minimum)
        _value = _minimum;
    return (unsigned char) _value;
} // end clamp()

/*
 * colorSliderCallback - Callback of change to color slider value.
 *
 * parameter _callbackData - Misc::CallbackData*
 */
void TransferFunction1D::colorSliderCallback(Misc::CallbackData* _callbackData) {
    RGBAColor* rgbaColor = new RGBAColor(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 3; ++i) {
        rgbaColor->setValues(i, float(colorSliders[i]->getValue()));
    }
    rgbaColor->setValues(3, 1.0f);
    colorPane->setBackgroundColor(rgbaColor->getValues());
    colorMap->setControlPointColor(*rgbaColor);
}

/*
 * colorSwatchesWidgetCallback
 *
 * parameter _callbackData - Misc::CallbackData*
 */
void TransferFunction1D::colorSwatchesWidgetCallback(Misc::CallbackData* _callbackData) {
    float* _color = swatchesWidget->getCurrentColor();
    RGBAColor* rgbaColor = new RGBAColor(_color[0], _color[1], _color[2], 1.0f);
    for (int i = 0; i < 3; ++i) {
        colorSliders[i]->setValue(_color[i]);
    }
    colorPane->setBackgroundColor(rgbaColor->getValues());
    colorMap->setControlPointColor(*rgbaColor);
} // end colorSwatchesWidgetCallback()

/*
 * createAlphaComponent - Create alpha component.
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter colorMapDialog - GLMotif::RowColumn*&
 * parameter volume - Volume*
 */
void TransferFunction1D::createAlphaComponent(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& colorMapDialog,
        Volume* volume) {
    alphaComponent = new ScalarWidget("AlphaComponent", colorMapDialog, volume, 3);
    alphaComponent->setBorderWidth(styleSheet.size * 0.5f);
    alphaComponent->setBorderType(GLMotif::Widget::LOWERED);
    alphaComponent->setForegroundColor(GLMotif::Color(0.0f, 1.0f, 0.0f));
    alphaComponent->setMarginWidth(styleSheet.size);
    alphaComponent->setPreferredSize(GLMotif::Vector(styleSheet.fontHeight * 20.0, styleSheet.fontHeight * 10.0, 0.0f));
    alphaComponent->setControlPointSize(styleSheet.size);
    alphaComponent->setControlPointScalar(1.0f);
    alphaComponent->getControlPointChangedCallbacks().add(this, &TransferFunction1D::alphaControlPointChangedCallback);
} // end createAlphaComponent()

/*
 * createButtonBox - Create a box to hold buttons.
 *
 * parameter colorMapDialog - GLMotif::RowColumn*&
 * return - GLMotif::RowColumn*
 */
GLMotif::RowColumn* TransferFunction1D::createButtonBox(GLMotif::RowColumn*& colorMapDialog) {
    GLMotif::RowColumn* buttonBox = new GLMotif::RowColumn("ButtonBox", colorMapDialog, false);
    buttonBox->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    buttonBox->setPacking(GLMotif::RowColumn::PACK_GRID);
    GLMotif::Button* removeControlPointButton = new GLMotif::Button("RemoveControlPointButton", buttonBox, "Remove RGB Point");
    removeControlPointButton->getSelectCallbacks().add(this, &TransferFunction1D::removeControlPointCallback);
    GLMotif::Button* removeAlphaControlPointButton = new GLMotif::Button("RemoveAlphaControlPointButton", buttonBox,
            "Remove Alpha Point");
    removeAlphaControlPointButton->getSelectCallbacks().add(this, &TransferFunction1D::removeAlphaControlPointCallback);
    GLMotif::ToggleButton* guassianToggleButton = new GLMotif::ToggleButton("GuassianToggleButton", buttonBox, "Gaussian");
    guassianToggleButton->setToggle(false);
    guassianToggleButton->getValueChangedCallbacks().add(this, &TransferFunction1D::gaussianToggleButtonCallback);
    interactiveToggleButton = new GLMotif::ToggleButton("InteractiveToggleButton", buttonBox, "Interactive");
    interactiveToggleButton->setToggle(false);
    interactiveToggleButton->getValueChangedCallbacks().add(this, &TransferFunction1D::interactiveToggleButtonCallback);
    return buttonBox;
}

/*
 * createColorEditor - Create color editor.
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter colorMapDialog - GLMotif::RowColumn*&
 * return - GLMotif::RowColumn*
 */
GLMotif::RowColumn* TransferFunction1D::createColorEditor(const GLMotif::StyleSheet& styleSheet,
        GLMotif::RowColumn*& colorMapDialog) {
    GLMotif::RowColumn* colorEditor = new GLMotif::RowColumn("ColorEditor", colorMapDialog, false);
    colorEditor->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    createColorSwatchesWidget(styleSheet, colorEditor);
    GLMotif::RowColumn* colorSlidersBox = createColorSliderBox(styleSheet, colorEditor);
    colorSlidersBox->manageChild();
    new GLMotif::Blind("Filler", colorEditor);
    return colorEditor;
}

/*
 * createColorMap - Create color map.
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter colorMapDialog - GLMotif::RowColumn*&
 */
void TransferFunction1D::createColorMap(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& colorMapDialog) {
    colorMap = new ColorMap("ColorMap", colorMapDialog);
    colorMap->setBorderWidth(styleSheet.size * 0.5f);
    colorMap->setBorderType(GLMotif::Widget::LOWERED);
    colorMap->setForegroundColor(GLMotif::Color(0.0f, 1.0f, 0.0f));
    colorMap->setMarginWidth(styleSheet.size);
    colorMap->setPreferredSize(GLMotif::Vector(styleSheet.fontHeight * 5.0, styleSheet.fontHeight * 5.0, 0.0f));
    colorMap->setControlPointSize(styleSheet.size);
    RGBAColor* rgbaColor = new RGBAColor(1.0f, 0.0f, 0.0f, 1.0f);
    colorMap->setControlPointColor(rgbaColor);
    colorMap->getControlPointChangedCallbacks().add(this, &TransferFunction1D::controlPointChangedCallback);
}

/*
 * createColorMapDialog - Create color map dialog.
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter volume - Volume*
 */
void TransferFunction1D::createColorMapDialog(const GLMotif::StyleSheet& styleSheet, Volume* volume) {
    GLMotif::RowColumn* colorMapDialog = new GLMotif::RowColumn("ColorMapDialog", this, false);
    createColorMap(styleSheet, colorMapDialog);
    GLMotif::RowColumn* buttonBox = createButtonBox(colorMapDialog);
    buttonBox->manageChild();
    createAlphaComponent(styleSheet, colorMapDialog, volume);
    createColorPanel(styleSheet, colorMapDialog);
    GLMotif::RowColumn* colorEditor = createColorEditor(styleSheet, colorMapDialog);
    colorEditor->manageChild();
    colorMapDialog->manageChild();
}

/*
 * createColorPanel - Create color panel.
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter colorMapDialog - GLMotif::RowColumn*&
 */
void TransferFunction1D::createColorPanel(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& colorMapDialog) {
    GLMotif::RowColumn* colorPanel = new GLMotif::RowColumn("ColorPanel", colorMapDialog, false);
    colorPanel->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    colorPanel->setMarginWidth(styleSheet.size);
    colorPane = new GLMotif::Blind("ColorPane", colorPanel);
    colorPane->setBorderWidth(styleSheet.size * 0.5f);
    colorPane->setBorderType(GLMotif::Widget::LOWERED);
    colorPane->setBackgroundColor(GLMotif::Color(0.5f, 0.5f, 0.5f));
    colorPane->setPreferredSize(GLMotif::Vector(styleSheet.fontHeight * 20.0f, styleSheet.fontHeight * 1.0f, 0.0f));
    colorPanel->manageChild();
}

/*
 * createColorSwatchesWidget
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter colorEditor - GLMotif::RowColumn*&
 */
void TransferFunction1D::createColorSwatchesWidget(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& colorEditor) {
    swatchesWidget = new SwatchesWidget("SwatchesWidget", colorEditor);
    swatchesWidget->setBorderWidth(styleSheet.size * 0.5f);
    swatchesWidget->setBorderType(GLMotif::Widget::LOWERED);
    swatchesWidget->setForegroundColor(GLMotif::Color(0.0f, 1.0f, 0.0f));
    swatchesWidget->setMarginWidth(styleSheet.size);
    swatchesWidget->setPreferredSize(GLMotif::Vector(styleSheet.fontHeight * 15.0, styleSheet.fontHeight * 5.0, 0.0f));
    swatchesWidget->getColorChangedCallbacks().add(this, &TransferFunction1D::colorSwatchesWidgetCallback);
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
GLMotif::Slider* TransferFunction1D::createColorSlider(const char* title, GLMotif::Color color,
        const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn* colorSlidersBox) {
    GLMotif::Slider* colorSlider = new GLMotif::Slider(title, colorSlidersBox, GLMotif::Slider::VERTICAL, styleSheet.fontHeight
            * 5.0f);
    colorSlider->setSliderColor(color);
    colorSlider->setValueRange(0.0f, 1.0f, 0.01f);
    colorSlider->setValue(0.5f);
    colorSlider->getValueChangedCallbacks().add(this, &TransferFunction1D::colorSliderCallback);
    return colorSlider;
}

/*
 * createColorSliderBox - Create box to contain color slider bars.
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter colorEditor - GLMotif::RowColumn*
 * return - GLMotif::RowColumn*
 */
GLMotif::RowColumn* TransferFunction1D::createColorSliderBox(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn* colorEditor) {
    GLMotif::RowColumn* colorSlidersBox = new GLMotif::RowColumn("ColorSliders", colorEditor, false);
    colorSlidersBox->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    colorSlidersBox->setPacking(GLMotif::RowColumn::PACK_GRID);
    createColorSliders(styleSheet, colorSlidersBox);
    return colorSlidersBox;
}

/*
 * createColorSliders - Create RGB color sliders.
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter colorSliderBox - GLMotif::RowColumn*
 */
void TransferFunction1D::createColorSliders(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn* colorSlidersBox) {
    colorSliders[0] = createColorSlider("RedSlider", GLMotif::Color(1.0f, 0.0f, 0.0f), styleSheet, colorSlidersBox);
    colorSliders[1] = createColorSlider("GreenSlider", GLMotif::Color(0.0f, 1.0f, 0.0f), styleSheet, colorSlidersBox);
    colorSliders[2] = createColorSlider("BlueSlider", GLMotif::Color(0.0f, 0.0f, 1.0f), styleSheet, colorSlidersBox);
}

/*
 * createTransferFunction1D - Create the transfer function 1D.
 *
 * parameter colorMapCreationType - int
 * parameter rampCreationType - int
 * parameter _minimum - double
 * parameter _maximum - double
 */
void TransferFunction1D::createTransferFunction1D(int colorMapCreationType, int rampCreationType, double _minimum, double _maximum) {
    colorMap->createColorMap(colorMapCreationType, _minimum, _maximum);
    alphaComponent->create(rampCreationType, _minimum, _maximum);
}

/*
 * exportAlpha - Export the alpha.
 *
 * parameter colormap - unsigned char*
 */
void TransferFunction1D::exportAlpha(unsigned char* colormap) const {
    alphaComponent->exportScalar(colormap);
} // end exportAlpha()

/*
 * exportColorMap - Export the color map.
 *
 * parameter colormap - unsigned char*
 */
void TransferFunction1D::exportColorMap(unsigned char* colormap) const {
    colorMap->exportColorMap(colormap);
}

/*
 * exportPreIntegrationColorMap
 *
 * parameter colormap - unsigned char*
 */
void TransferFunction1D::exportPreIntegrationColorMap(unsigned char* colormap) {
    colorMap->exportColorMap(colormap);
    alphaComponent->exportScalar(colormap);
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
        colorTable[4 * i + 3] = colormap[4 * (i * 256 + i) + 3];
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
void TransferFunction1D::gaussianToggleButtonCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData) {
    if (strcmp(callBackData->toggle->getName(), "GuassianToggleButton") == 0) {
        alphaComponent->setGaussian(callBackData->set);
    }
    Vrui::requestUpdate();
} // end gaussianToggleButtonCallback()

/*
 * getAlphaChangedCallbacks
 *
 * return - Misc::CallbackList&
 */
Misc::CallbackList& TransferFunction1D::getAlphaChangedCallbacks(void) {
    return alphaComponent->getChangedCallbacks();
} // end getAlphaChangedCallbacks()

/*
 * getColorMap
 *
 * return const ColorMap*
 */
const ColorMap* TransferFunction1D::getColorMap(void) const {
    return colorMap;
} // end getColorMap()

/*
 * getColorMap
 *
 * return - ColorMap*
 */
ColorMap* TransferFunction1D::getColorMap(void) {
    return colorMap;
} // end getColorMap()

/*
 * getColorMapChangedCallbacks
 *
 * return - Misc::CallbackList&
 */
Misc::CallbackList& TransferFunction1D::getColorMapChangedCallbacks(void) {
    return colorMap->getColorMapChangedCallbacks();
} // end getColorMapChangedCallbacks()

/*
 * isDragging
 *
 * return - bool
 */
bool TransferFunction1D::isDragging(void) const {
    return alphaComponent->isDragging();
} // end isDragging()

/*
 * isInteractive
 *
 * return - bool
 */
bool TransferFunction1D::isInteractive(void) {
    return interactive;
} // end isInteractive()

/*
 * setInteractive
 *
 * parameter interactive - bool
 */
void TransferFunction1D::setInteractive(bool interactive) {
    this->interactive = interactive;
    interactiveToggleButton->setToggle(interactive);
} // end setInteractive()

/*
 * getTransferFunction1D - Get the transfer function 1D.
 *
 * return - Storage*
 */
Storage* TransferFunction1D::getTransferFunction1D(void) const {
    return colorMap->getColorMap();
}

/*
 * setTransferFunction1D - Set the transfer function 1D.
 *
 * parameter storage - Storage*
 */
void TransferFunction1D::setTransferFunction1D(Storage* storage) {
    colorMap->setColorMap(storage);
}

/*
 * initialize - Initialize the GUI for the TransferFunction1D class.
 *
 * parameter volume - Volume*
 */
void TransferFunction1D::initialize(Volume* volume) {
    const GLMotif::StyleSheet& styleSheet = *Vrui::getWidgetManager()->getStyleSheet();
    createColorMapDialog(styleSheet, volume);
}

/*
 * interactiveToggleButtonCallback
 *
 * parameter callBackData - GLMotif::ToggleButton::ValueChangedCallbackData*
 */
void TransferFunction1D::interactiveToggleButtonCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData) {
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
 * loadTransferFunction1D - Load a transfer function 1D.
 *
 * parameter fileName - const char*
 */
void TransferFunction1D::loadTransferFunction1D(const char* fileName) {
    colorMap->loadColorMap(fileName);
}

/*
 * removeAlphaControlPointCallback - Remove the current alpha control point.
 *
 * parameter _callbackData - Misc::CallbackData*
 */
void TransferFunction1D::removeAlphaControlPointCallback(Misc::CallbackData* _callbackData) {
    alphaComponent->deleteControlPoint();
} // end removeAlphaControlPointCallback()

/*
 * removeControlPointCallback - Remove the current control point.
 *
 * parameter _callbackData - Misc::CallbackData*
 */
void TransferFunction1D::removeControlPointCallback(Misc::CallbackData* _callbackData) {
    colorMap->deleteControlPoint();
} // end removeControlPointCallback()

/*
 * saveTransferFunction1D - Save the transfer function 1D.
 *
 * parameter fileName - const char*
 */
void TransferFunction1D::saveTransferFunction1D(const char* fileName) const {
    colorMap->saveColorMap(fileName);
}

/*
 * alphaControlPointChangedCallback - Callback to select a control point.
 *
 * parameter _callbackData - Misc::CallbackData*
 */
void TransferFunction1D::alphaControlPointChangedCallback(Misc::CallbackData* _callbackData) {
    ScalarWidgetControlPointChangedCallbackData* callbackData =
            static_cast<ScalarWidgetControlPointChangedCallbackData*> (_callbackData);
    if (callbackData->getCurrentControlPoint() != 0) {
        float _alpha = alphaComponent->getControlPointScalar();
    }
} // end alphaControlPointChangedCallback()

/*
 * controlPointChangedCallback - Callback to select a control point.
 *
 * parameter _callbackData - Misc::CallbackData*
 */
void TransferFunction1D::controlPointChangedCallback(Misc::CallbackData* _callbackData) {
    ControlPointChangedCallbackData* callbackData = static_cast<ControlPointChangedCallbackData*> (_callbackData);
    if (callbackData->getCurrentControlPoint() != 0) {
        RGBAColor* rgbaColor = colorMap->getControlPointColor();
        colorPane->setBackgroundColor(rgbaColor->getValues());
        for (int i = 0; i < 3; ++i)
            colorSliders[i]->setValue(rgbaColor->getValues(i));
    } else {
        colorPane->setBackgroundColor(GLMotif::Color(0.5f, 0.5f, 0.5f));
        for (int i = 0; i < 3; ++i)
            colorSliders[i]->setValue(0.5);
    }
}
