/*
 * TransferFunction2_5D.cpp - Methods for TransferFunction2_5D class.
 *
 * Author: Patrick O'Leary
 * Created: August 14, 2008
 * Copyright: 2008. All rights reserved.
 */
#include <iostream>
#include <cmath>

/* Vrui includes */
#include <GL/GLColor.h>
#include <GLMotif/Label.h>
#include <GLMotif/Button.h>
#include <GLMotif/ToggleButton.h>
#include <GLMotif/WidgetManager.h>
#include <Misc/File.h>

/* Vrui includes to use the Vrui interface */
#include <Vrui/Vrui.h>

#include <COLORMAP/RGBAColor.h>
#include <DATA/Volume.h>
#include <GUI/HistogramWidget.h>
#include <GUI/QuadrilateralChangedCallbackData.h>
#include <GUI/SliceWidget.h>
#include <GUI/SwatchesWidget.h>
#include <GUI/TransferFunction2_5D.h>
#include <Toirt_Samhlaigh.h>

/*
 * TransferFunction2_5D - Constructor for TransferFunction2_5D class.
 * 		extends GLMotif::PopupWindow
 *
 * parameter _volume - Volume *
 */
TransferFunction2_5D::TransferFunction2_5D(Volume * _volume, Toirt_Samhlaigh & _toirt_samhlaigh) :
    GLMotif::PopupWindow("TransferFunction2_5DPopup", Vrui::getWidgetManager(), "2.5D Transfer Function Editor"), colorPane(NULL),
            histogramWidget(NULL), interactive(false), sliceDirection(0), sliceSlider(NULL), sliceTextField(NULL), swatchesWidget(NULL),
            toirt_samhlaigh(_toirt_samhlaigh), volume(_volume), xyToggle(NULL), xzToggle(NULL), yzToggle(NULL) {
    colorSliders[0] = NULL;
    colorSliders[1] = NULL;
    colorSliders[2] = NULL;
    initialize();
}

/*
 * ~TransferFunction2_5D - Destructor for TransferFunction2_5D class.
 */
TransferFunction2_5D::~TransferFunction2_5D(void) {
}

/*
 * toggleSelectCallback - Adjust the gui/program state based on which toggle button changed state.
 *
 * parameter callbackData - GLMotif::ToggleButton::ValueChangedCallbackData *
 */
void TransferFunction2_5D::toggleSelectCallback(GLMotif::ToggleButton::ValueChangedCallbackData * callbackData) {
    /* Adjust gui/program state based on which toggle button changed state: */
    if (strcmp(callbackData->toggle->getName(), "XYToggle") == 0) {
        sliceDirection = XY;
        xzToggle->setToggle(false);
        yzToggle->setToggle(false);
        sliceTextField->setValue(int(volume->getDepth() / 2));
        sliceSlider->setValueRange(0, volume->getDepth() - 1, 1);
        sliceSlider->setValue(volume->getDepth() / 2);
        sliceWidget->updateSlice(sliceDirection, volume->getDepth() / 2);
        histogramWidget->updateHistogram(sliceDirection, volume->getDepth() / 2);
    } else if (strcmp(callbackData->toggle->getName(), "XZToggle") == 0) {
        sliceDirection = XZ;
        xyToggle->setToggle(false);
        yzToggle->setToggle(false);
        sliceTextField->setValue(int(volume->getHeight() / 2));
        sliceSlider->setValueRange(0, volume->getHeight() - 1, 1);
        sliceSlider->setValue(volume->getHeight() / 2);
        sliceWidget->updateSlice(sliceDirection, volume->getHeight() / 2);
        histogramWidget->updateHistogram(sliceDirection, volume->getHeight() / 2);
    } else {
        sliceDirection = YZ;
        xyToggle->setToggle(false);
        xzToggle->setToggle(false);
        sliceTextField->setValue(int(volume->getWidth() / 2));
        sliceSlider->setValueRange(0, volume->getWidth() - 1, 1);
        sliceSlider->setValue(volume->getWidth() / 2);
        sliceWidget->updateSlice(sliceDirection, volume->getWidth() / 2);
        histogramWidget->updateHistogram(sliceDirection, volume->getWidth() / 2);
    }
    histogramWidget->setDirectionChanged(true);
    histogramWidget->setSliceChanged(true);
    sliceWidget->setDirectionChanged(true);
    sliceWidget->setSliceChanged(true);
    Vrui::requestUpdate();
} // end toggleSelectCallback()

/*
 * alphaSliderCallback - Callback of change to color slider value.
 *
 * parameter callbackData - Misc::CallbackData*
 */
void TransferFunction2_5D::alphaSliderCallback(Misc::CallbackData* callbackData) {
    RGBAColor* rgbaColor = new RGBAColor(0.0f, 0.0f, 0.0f, 0.0f);
    float * color = histogramWidget->getQuadrilateralColor();
    for (int i = 0; i < 3; ++i) {
        rgbaColor->setValues(i, float(color[i]));
    }
    rgbaColor->setValues(3, float(alphaSlider->getValue()));
    alphaTextField->setValue(alphaSlider->getValue());
    histogramWidget->setQuadrilateralColor(rgbaColor->getValues());
} // end alphaSliderCallback()

/*
 * colorSliderCallback - Callback of change to color slider value.
 *
 * parameter callbackData - Misc::CallbackData*
 */
void TransferFunction2_5D::colorSliderCallback(Misc::CallbackData* callbackData) {
    RGBAColor* rgbaColor = new RGBAColor(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 3; ++i) {
        rgbaColor->setValues(i, float(colorSliders[i]->getValue()));
    }
    rgbaColor->setValues(3, 0.1f);
    colorPane->setBackgroundColor(rgbaColor->getValues());
    histogramWidget->setQuadrilateralColor(rgbaColor->getValues());
}

/*
 * colorSwatchesWidgetCallback
 *
 * parameter callbackData - Misc::CallbackData*
 */
void TransferFunction2_5D::colorSwatchesWidgetCallback(Misc::CallbackData* callbackData) {
    float* _color = swatchesWidget->getCurrentColor();
    RGBAColor* rgbaColor = new RGBAColor(_color[0], _color[1], _color[2], 0.1f);
    for (int i = 0; i < 3; ++i) {
        colorSliders[i]->setValue(_color[i]);
    }
    colorPane->setBackgroundColor(rgbaColor->getValues());
    histogramWidget->setQuadrilateralColor(rgbaColor->getValues());
} // end colorSwatchesWidgetCallback()

/*
 * createAlphaSlider - Create color panel.
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter transferFunction2_5DDialog - GLMotif::RowColumn*&
 */
void TransferFunction2_5D::createAlphaSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& transferFunction2_5DDialog) {
    GLMotif::RowColumn* alphaSliderRowColumn = new GLMotif::RowColumn("AlphaSliderRowColumn", transferFunction2_5DDialog, false);
    alphaSliderRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    alphaSliderRowColumn->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    alphaTextField = new GLMotif::TextField("AlphaTextField", alphaSliderRowColumn, 6);
    alphaTextField->setFieldWidth(4);
    alphaTextField->setPrecision(3);
    alphaTextField->setValue(0.1f);
    alphaSlider = new GLMotif::Slider("AlphaSlider", alphaSliderRowColumn, GLMotif::Slider::HORIZONTAL, styleSheet.fontHeight
            * 5.0f);
    alphaSlider->setSliderColor(GLMotif::Color(0.5f, 0.5f, 0.5f));
    alphaSlider->setValueRange(0.0f, 1.0f, 0.01f);
    alphaSlider->setValue(0.1f);
    alphaSlider->getValueChangedCallbacks().add(this, &TransferFunction2_5D::alphaSliderCallback);
    alphaSliderRowColumn->manageChild();
} // end createAlphaSlider()

/*
 * createButtonBox - Create a box to hold buttons.
 *
 * parameter transferFunction2_5DDialog - GLMotif::RowColumn*&
 * return - GLMotif::RowColumn*
 */
GLMotif::RowColumn* TransferFunction2_5D::createButtonBox(GLMotif::RowColumn*& transferFunction2_5DDialog) {
    GLMotif::RowColumn* buttonBox = new GLMotif::RowColumn("ButtonBox", transferFunction2_5DDialog, false);
    buttonBox->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    buttonBox->setPacking(GLMotif::RowColumn::PACK_GRID);
    GLMotif::Button* removeQuadrilateralButton =
            new GLMotif::Button("RemoveQuadrilateralButton", buttonBox, "Remove Quadrilateral");
    removeQuadrilateralButton->getSelectCallbacks().add(this, &TransferFunction2_5D::removeQuadrilateralCallback);
    interactiveToggleButton = new GLMotif::ToggleButton("InteractiveToggleButton", buttonBox, "Interactive");
    interactiveToggleButton->setToggle(false);
    interactiveToggleButton->getValueChangedCallbacks().add(this, &TransferFunction2_5D::interactiveToggleButtonCallback);
    new GLMotif::Blind("Filler", buttonBox);
    return buttonBox;
}

/*
 * createColorEditor - Create color editor.
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter transferFunction2_5DDialog - GLMotif::RowColumn*&
 * return - GLMotif::RowColumn*
 */
GLMotif::RowColumn* TransferFunction2_5D::createColorEditor(const GLMotif::StyleSheet& styleSheet,
        GLMotif::RowColumn*& transferFunction2_5DDialog) {
    GLMotif::RowColumn* colorEditor = new GLMotif::RowColumn("ColorEditor", transferFunction2_5DDialog, false);
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
}

/*
 * createColorSwatchesWidget
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter colorEditor - GLMotif::RowColumn*&
 */
void TransferFunction2_5D::createColorSwatchesWidget(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& colorEditor) {
    swatchesWidget = new SwatchesWidget("SwatchesWidget", colorEditor);
    swatchesWidget->setBorderWidth(styleSheet.size * 0.5f);
    swatchesWidget->setBorderType(GLMotif::Widget::LOWERED);
    swatchesWidget->setForegroundColor(GLMotif::Color(0.0f, 1.0f, 0.0f));
    swatchesWidget->setMarginWidth(styleSheet.size);
    swatchesWidget->setPreferredSize(GLMotif::Vector(styleSheet.fontHeight * 15.0, styleSheet.fontHeight * 5.0, 0.0f));
    swatchesWidget->getColorChangedCallbacks().add(this, &TransferFunction2_5D::colorSwatchesWidgetCallback);
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
GLMotif::Slider* TransferFunction2_5D::createColorSlider(const char* title, GLMotif::Color color,
        const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn* colorSlidersBox) {
    GLMotif::Slider* colorSlider = new GLMotif::Slider(title, colorSlidersBox, GLMotif::Slider::VERTICAL, styleSheet.fontHeight
            * 5.0f);
    colorSlider->setSliderColor(color);
    colorSlider->setValueRange(0.0f, 1.0f, 0.01f);
    colorSlider->setValue(0.5f);
    colorSlider->getValueChangedCallbacks().add(this, &TransferFunction2_5D::colorSliderCallback);
    return colorSlider;
}

/*
 * createColorSliderBox - Create box to contain color slider bars.
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter colorEditor - GLMotif::RowColumn*
 * return - GLMotif::RowColumn*
 */
GLMotif::RowColumn* TransferFunction2_5D::createColorSliderBox(const GLMotif::StyleSheet& styleSheet,
        GLMotif::RowColumn* colorEditor) {
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
void TransferFunction2_5D::createColorSliders(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn* colorSlidersBox) {
    colorSliders[0] = createColorSlider("RedSlider", GLMotif::Color(1.0f, 0.0f, 0.0f), styleSheet, colorSlidersBox);
    colorSliders[1] = createColorSlider("GreenSlider", GLMotif::Color(0.0f, 1.0f, 0.0f), styleSheet, colorSlidersBox);
    colorSliders[2] = createColorSlider("BlueSlider", GLMotif::Color(0.0f, 0.0f, 1.0f), styleSheet, colorSlidersBox);
}

/*
 * createHistogramWidget
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter sliceAndHistogramRowColumn - GLMotif::RowColumn*
 */
void TransferFunction2_5D::createHistogramWidget(const GLMotif::StyleSheet & styleSheet,
        GLMotif::RowColumn * & sliceAndHistogramRowColumn) {
    histogramWidget = new HistogramWidget("HistogramWidget", sliceAndHistogramRowColumn, volume);
    histogramWidget->setBorderWidth(styleSheet.size * 0.5f);
    histogramWidget->setBorderType(GLMotif::Widget::LOWERED);
    histogramWidget->setForegroundColor(GLMotif::Color(0.0f, 1.0f, 0.0f));
    histogramWidget->setMarginWidth(styleSheet.size);
    histogramWidget->setPreferredSize(GLMotif::Vector(styleSheet.fontHeight * 15.0, styleSheet.fontHeight * 15.0, 0.0f));
    histogramWidget->setControlPointSize(styleSheet.size);
    histogramWidget->getQuadrilateralChangedCallbacks().add(this, &TransferFunction2_5D::quadrilateralChangedCallback);
} // end createHistogramWidget()

/*
 * createSliceAndHistogramRowColumn
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter transferFunction2_5DDialog - GLMotif::RowColumn*
 */
void TransferFunction2_5D::createSliceAndHistogramRowColumn(const GLMotif::StyleSheet& styleSheet,
        GLMotif::RowColumn * transferFunction2_5DDialog) {
    GLMotif::RowColumn * sliceAndHistogramRowColumn =
            new GLMotif::RowColumn("SliceAndHistogramRowColumn", transferFunction2_5DDialog, false);
    sliceAndHistogramRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    sliceAndHistogramRowColumn->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    createSliceWidget(styleSheet, sliceAndHistogramRowColumn);
    createHistogramWidget(styleSheet, sliceAndHistogramRowColumn);
    sliceAndHistogramRowColumn->manageChild();
} // end createSliceAndHistogramRowColumn()

/*
 * createSliceWidget
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter sliceAndHistogramRowColumn - GLMotif::RowColumn*
 */
void TransferFunction2_5D::createSliceWidget(const GLMotif::StyleSheet & styleSheet,
        GLMotif::RowColumn * & sliceAndHistogramRowColumn) {
    sliceWidget = new SliceWidget("SliceWidget", sliceAndHistogramRowColumn, volume);
    sliceWidget->setBorderWidth(styleSheet.size * 0.5f);
    sliceWidget->setBorderType(GLMotif::Widget::LOWERED);
    sliceWidget->setForegroundColor(GLMotif::Color(0.0f, 1.0f, 0.0f));
    sliceWidget->setMarginWidth(styleSheet.size);
    sliceWidget->setPreferredSize(GLMotif::Vector(styleSheet.fontHeight * 15.0, styleSheet.fontHeight * 15.0, 0.0f));
    // how sliceWidget->getColorChangedCallbacks().add(this, &TransferFunction2_5D::colorSwatchesCallback);
} // end createSliceWidget()

/*
 * createSliceDirection
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter sliceRowColumn - GLMotif::RowColumn*
 */
void TransferFunction2_5D::createSliceDirection(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn * sliceRowColumn) {
    GLMotif::RowColumn * sliceDirectionRowColumn = new GLMotif::RowColumn("SliceDirectionRadioBoxRowColumn", sliceRowColumn, false);
    sliceDirectionRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    sliceDirectionRowColumn->setPacking(GLMotif::RowColumn::PACK_GRID);
    xyToggle = new GLMotif::ToggleButton("XYToggle", sliceDirectionRowColumn, "X-Y");
    xyToggle->setToggle(true);
    xyToggle->getValueChangedCallbacks().add(this, &TransferFunction2_5D::toggleSelectCallback);
    xzToggle = new GLMotif::ToggleButton("XZToggle", sliceDirectionRowColumn, "X-Z");
    xzToggle->setToggle(false);
    xzToggle->getValueChangedCallbacks().add(this, &TransferFunction2_5D::toggleSelectCallback);
    yzToggle = new GLMotif::ToggleButton("YZToggle", sliceDirectionRowColumn, "Y-Z");
    yzToggle->setToggle(false);
    yzToggle->getValueChangedCallbacks().add(this, &TransferFunction2_5D::toggleSelectCallback);
    sliceDirectionRowColumn->manageChild();
} // end createSliceDirection()

/*
 * createSliceRowColumn
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter transferFunction2_5DDialog - GLMotif::RowColumn*
 */
void TransferFunction2_5D::createSliceRowColumn(const GLMotif::StyleSheet& styleSheet,
        GLMotif::RowColumn * transferFunction2_5DDialog) {
    GLMotif::RowColumn * sliceRowColumn = new GLMotif::RowColumn("SliceRowColumn", transferFunction2_5DDialog, false);
    sliceRowColumn->setOrientation(GLMotif::RowColumn::VERTICAL);
    sliceRowColumn->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    createSliceDirection(styleSheet, sliceRowColumn);
    createSliceSlider(styleSheet, sliceRowColumn);
    sliceRowColumn->manageChild();
} // end createSliceRowColumn()

/*
 * createSliceSlider - Create slice slider.
 *
 * parameter styleSheet - const GLMotif::StyleSheet&
 * parameter sliceRowColumn - GLMotif::RowColumn*
 */
void TransferFunction2_5D::createSliceSlider(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn * sliceRowColumn) {
    GLMotif::RowColumn * sliceSliderRowColumn = new GLMotif::RowColumn("SliceSliderRowColumn", sliceRowColumn, false);
    sliceSliderRowColumn->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    sliceSliderRowColumn->setPacking(GLMotif::RowColumn::PACK_TIGHT);
    sliceTextField = new GLMotif::TextField("SliceTextField", sliceSliderRowColumn, 6);
    sliceTextField->setFieldWidth(4);
    sliceTextField->setPrecision(0);
    sliceTextField->setValue(int(volume->getDepth() / 2));
    sliceSlider = new GLMotif::Slider("SliceSlider", sliceSliderRowColumn, GLMotif::Slider::HORIZONTAL, styleSheet.fontHeight
            * 5.0f);
    sliceSlider->setValueRange(0, volume->getDepth() - 1, 1);
    sliceSlider->setValue(volume->getDepth() / 2);
    sliceSlider->getValueChangedCallbacks().add(this, &TransferFunction2_5D::sliceSliderCallback);

    sliceSliderRowColumn->manageChild();
}

/*
 * exportColorMap - Export the color map.
 *
 * parameter colormap - unsigned char *
 */
void TransferFunction2_5D::exportColorMap(unsigned char * colormap) const {
    histogramWidget->exportColorMap(colormap);
}

/*
 * getHistogramWidgetChangedCallbacks
 *
 * return - Misc::CallbackList&
 */
Misc::CallbackList& TransferFunction2_5D::getHistogramWidgetChangedCallbacks(void) {
    return histogramWidget->getHistogramWidgetChangedCallbacks();
} // end getHistogramWidgetChangedCallbacks()

/*
 * isDragging
 *
 * return - bool
 */
bool TransferFunction2_5D::isDragging(void) const {
    return histogramWidget->isDragging();
} // end isDragging()

/*
 * isInteractive
 *
 * return - bool
 */
bool TransferFunction2_5D::isInteractive(void) {
    return interactive;
} // end isInteractive()

/*
 * setInteractive
 *
 * parameter interactive - bool
 */
void TransferFunction2_5D::setInteractive(bool interactive) {
    this->interactive = interactive;
    interactiveToggleButton->setToggle(interactive);
} // end setInteractive()

/*
 * setSliceChanged
 *
 * parameter sliceChanged - bool
 */
void TransferFunction2_5D::setSliceChanged(bool sliceChanged) {
    return sliceWidget->setSliceChanged(sliceChanged);
    return histogramWidget->setSliceChanged(sliceChanged);
} // end setSliceChanged()

/*
 * initialize - Initialize the GUI for the TransferFunction2_5D class.
 */
void TransferFunction2_5D::initialize(void) {
    const GLMotif::StyleSheet& styleSheet = *Vrui::getWidgetManager()->getStyleSheet();
    GLMotif::RowColumn* transferFunction2_5DDialog = new GLMotif::RowColumn("TransferFunction2_5DDialog", this, false);
    GLMotif::RowColumn* buttonBox = createButtonBox(transferFunction2_5DDialog);
    buttonBox->manageChild();
    createSliceAndHistogramRowColumn(styleSheet, transferFunction2_5DDialog);
    createSliceRowColumn(styleSheet, transferFunction2_5DDialog);
    GLMotif::RowColumn* colorEditor = createColorEditor(styleSheet, transferFunction2_5DDialog);
    colorEditor->manageChild();
    createAlphaSlider(styleSheet, transferFunction2_5DDialog);
    transferFunction2_5DDialog->manageChild();
}

/*
 * interactiveToggleButtonCallback
 *
 * parameter callBackData - GLMotif::ToggleButton::ValueChangedCallbackData*
 */
void TransferFunction2_5D::interactiveToggleButtonCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData) {
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
 * loadTransferFunction2_5D - Load a transfer function 2.5D.
 *
 * parameter fileName - const char*
 */
void TransferFunction2_5D::loadTransferFunction2_5D(const char* fileName) {
    // how
}

/*
 * quadrilateralChangedCallback - Callback to select a quadrilateral.
 *
 * parameter _callbackData - Misc::CallbackData*
 */
void TransferFunction2_5D::quadrilateralChangedCallback(Misc::CallbackData* _callbackData) {
    QuadrilateralChangedCallbackData* callbackData = static_cast<QuadrilateralChangedCallbackData*> (_callbackData);
    if (callbackData->getCurrentQuadrilateral() != 0) {
        RGBAColor * rgbaColor = new RGBAColor(histogramWidget->getQuadrilateralColor());
        colorPane->setBackgroundColor(rgbaColor->getValues());
        for (int i = 0; i < 3; ++i)
            colorSliders[i]->setValue(rgbaColor->getValues(i));
        alphaSlider->setValue(rgbaColor->getValues(3));
    } else {
        colorPane->setBackgroundColor(GLMotif::Color(0.5f, 0.5f, 0.5f));
        for (int i = 0; i < 3; ++i)
            colorSliders[i]->setValue(0.5);
        alphaSlider->setValue(0.1);
    }
} // end quadrilateralChangedCallback()

/*
 * removeQuadrilateralCallback - Remove the current quadrilateral.
 *
 * parameter _callbackData - Misc::CallbackData*
 */
void TransferFunction2_5D::removeQuadrilateralCallback(Misc::CallbackData* _callbackData) {
    histogramWidget->deleteQuadrilateral();
} // end removeQuadrilateralCallback()

/*
 * saveTransferFunction2_5D - Save the transfer function 2.5D.
 *
 * parameter fileName - const char*
 */
void TransferFunction2_5D::saveTransferFunction2_5D(const char* fileName) const {
    // how
}

/*
 * sliceSliderCallback - Callback of change to slice slider value.
 *
 * parameter callbackData - Misc::CallbackData*
 */
void TransferFunction2_5D::sliceSliderCallback(Misc::CallbackData* callbackData) {
    sliceTextField->setValue(int(sliceSlider->getValue()));
    histogramWidget->updateHistogram(sliceDirection, int(sliceSlider->getValue()));
    histogramWidget->setSliceChanged(true);
    sliceWidget->updateSlice(sliceDirection, int(sliceSlider->getValue()));
    toirt_samhlaigh.setTransferFunction2_5DChanged(true);
    Vrui::requestUpdate();
} // end sliceSliderCallback()
