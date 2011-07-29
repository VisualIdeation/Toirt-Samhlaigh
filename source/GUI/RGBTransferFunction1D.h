/*
 * RGBTransferFunction1D.h - Class for editing RGB component 1D transfer function.
 *
 * Author: Patrick O'Leary
 * Created: January 21, 2008
 * Copyright: 2008. All rights reserved.
 */
#ifndef RGBTRASFERFUNCTION1D_INCLUDED
#define RGBTRASFERFUNCTION1D_INCLUDED

/* Vrui includes */
#include <GL/GLColorMap.h>
#include <GLMotif/Blind.h>
#include <GLMotif/PopupWindow.h>
#include <GLMotif/RowColumn.h>
#include <GLMotif/StyleSheet.h>
#include <GLMotif/ToggleButton.h>
#include <Misc/CallbackData.h>
#include <Misc/CallbackList.h>

// begin Forward Declarations
class ScalarWidget;
class Toirt_Samhlaigh;
class Volume;
// end Forward Declarations

class RGBTransferFunction1D: public GLMotif::PopupWindow {
public:
    RGBTransferFunction1D(Toirt_Samhlaigh & _toirt_samhlaigh, Volume * volume);
    virtual ~RGBTransferFunction1D(void);
    void changeBlue(int ramp) const;
    void changeGreen(int ramp) const;
    void changeRed(int ramp) const;
    void createRGBTransferFunction1D(int redRampCreationType, int greenRampCreationType, int blueRampCreationType, double _minimum,
            double _maximum);
    void exportBlue(unsigned char* colormap) const;
    void exportGreen(unsigned char* colormap) const;
    void exportRed(unsigned char* colormap) const;
    void exportPreIntegrationColorMap(unsigned char* colormap);
    Misc::CallbackList& getBlueChangedCallbacks(void);
    bool isDragging(void) const;
    Misc::CallbackList& getGreenChangedCallbacks(void);
    bool isInteractive(void);
    void setInteractive(bool interactive);
    Misc::CallbackList& getRedChangedCallbacks(void);
    void loadBlueTransferFunction1D(const char* fileName);
    void loadGreenTransferFunction1D(const char* fileName);
    void loadRedTransferFunction1D(const char* fileName);
    void saveBlueTransferFunction1D(const char* fileName) const;
    void saveGreenTransferFunction1D(const char* fileName) const;
    void saveRedTransferFunction1D(const char* fileName) const;
private:
    GLMotif::ToggleButton* blueToggleButton;
    GLMotif::ToggleButton* gaussianToggleButton;
    GLMotif::ToggleButton* greenToggleButton;
    bool interactive;
    GLMotif::ToggleButton* interactiveToggleButton;
    GLMotif::ToggleButton* redToggleButton;
    ScalarWidget* scalarWidget;
    Toirt_Samhlaigh & toirt_samhlaigh;
    void blueToggleButtonCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
    unsigned char clamp(int _value, int _minimum, int _maximum);
    GLMotif::RowColumn* createButtonBox(GLMotif::RowColumn*& colorMapDialog);
    GLMotif::RowColumn* createRGBButtonBox(GLMotif::RowColumn*& colorMapDialog);
    void createScalarWidget(const GLMotif::StyleSheet& styleSheet, GLMotif::RowColumn*& scalarComponentDialog, Volume* volume);
    void createScalarWidgetDialog(const GLMotif::StyleSheet& styleSheet, Volume* volume);
    void gaussianToggleButtonCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
    void greenToggleButtonCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
    void initialize(Volume* volume);
    void interactiveToggleButtonCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
    void redToggleButtonCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
    void removeScalarWidgetControlPointCallback(Misc::CallbackData* _callbackData);
    void scalarWidgetControlPointChangedCallback(Misc::CallbackData* _callbackData);
};

#endif
