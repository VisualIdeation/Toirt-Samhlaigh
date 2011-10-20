/*
 * Toirt_Samhlaigh.h - Class for volume rendering (toirt samhlaigh in Gaelic).
 *
 * Author: Patrick O'Leary
 * Created: October 21, 2007
 * Copyright: 2007
 */

#ifndef TOIRT_SAMHLAIGH_H_
#define TOIRT_SAMHLAIGH_H_

/* Vrui includes */
#include <Geometry/OrthogonalTransformation.h>
#include <GL/GLTransformationWrappers.h>
#include <GLMotif/Menu.h>
#include <GLMotif/Popup.h>
#include <GLMotif/PopupMenu.h>
#include <GLMotif/PopupWindow.h>
#include <GLMotif/RadioBox.h>
#include <GLMotif/Slider.h>
#include <GLMotif/TextField.h>
#include <GLMotif/ToggleButton.h>
#include <GLMotif/CascadeButton.h>
#include <Misc/CallbackData.h>
#include <Misc/Timer.h>
#include <Vrui/Application.h>

// begin Forward Declarations
class Animation;
class BaseLocator;
class ClippingPlane;
class DownSampling;
class FeatureEnhancement;
class FocusAndContextPlane;
class Lighting;
class Materials;
class RGBTransferFunction1D;
class Scene;
class Shading;
class Slices;
class TransferFunction1D;
class TransferFunction2_5D;
class Vector4;
class Volume;
// end Forward Declarations

class Toirt_Samhlaigh: public Vrui::Application {
public:
    typedef std::vector<BaseLocator*> BaseLocatorList;
    typedef float Scalar;
    Toirt_Samhlaigh(int& argc, char**& argv, char**& appDefaults);
    virtual ~Toirt_Samhlaigh(void);
    void alphaChangedCallback(Misc::CallbackData* callBackData);
    void blueChangedCallback(Misc::CallbackData* callBackData);
    float calculateNumberOfLeafNodesInOctree(void);
    void centerDisplayCallback(Misc::CallbackData* callBackData);
    void changeAlphaCallback(GLMotif::RadioBox::ValueChangedCallbackData* callBackData);
    void changeBlueCallback(GLMotif::RadioBox::ValueChangedCallbackData* callBackData);
    void changeColorMapCallback(GLMotif::RadioBox::ValueChangedCallbackData* callBackData);
    void changeGreenCallback(GLMotif::RadioBox::ValueChangedCallbackData* callBackData);
    void changeRedCallback(GLMotif::RadioBox::ValueChangedCallbackData* callBackData);
    void changeShadingCallback(GLMotif::RadioBox::ValueChangedCallbackData* callBackData);
    void colorMapChangedCallback(Misc::CallbackData* callBackData);
    void colorMap2_5DChangedCallback(Misc::CallbackData* callBackData);
    virtual void display(GLContextData& contextData) const;
    void frame(void);
    ClippingPlane * getClippingPlanes(void);
    FocusAndContextPlane * getFocusAndContextPlanes(void);
    void setDownSamplingChanged(bool downSamplingChanged);
    bool isInteractive(void);
    int getNumberOfClippingPlanes(void);
    int getNumberOfFocusAndContextPlanes(void);
    void greenChangedCallback(Misc::CallbackData* callBackData);
    void setSliceColorMapChanged(bool _sliceColorMapChanged);
    void menuToggleSelectCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
    void redChangedCallback(Misc::CallbackData* callBackData);
    void set1D(void);
    void set2D(void);
    void setAnimating(bool _animating);
    void setPreintegrationColormap(void);
    void setTransferFunction2_5DChanged(bool transferFunction2_5DChanged);
    void showTransferFunction1DCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
    void showTransferFunction2_5DCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
    void sliderCallback(GLMotif::Slider::ValueChangedCallbackData* callBackData);
private:
    float * alpha;
    float * alpha2_5D;
    bool alphaChanged;
    int analysisTool;
    bool animating;
    Animation * animation;
    BaseLocatorList baseLocators;
    float* blue;
    float blueScale;
    const char* blueVolumeFile;
    ClippingPlane * clippingPlanes;
    unsigned char* colormap;
    bool colorMapChanged;
    DownSampling * downSampling;
    bool downSamplingChanged;
    FeatureEnhancement * featureEnhancement;
    bool firstFrame;
    FocusAndContextPlane * focusAndContextPlanes;
    float* green;
    float greenScale;
    const char* greenVolumeFile;
    bool interactive;
    Lighting * lighting;
    GLMotif::PopupMenu* mainMenu;
    Materials * materials;
    bool maximumPriorityQueueTest;
    int maximumPriorityQueueSize;
    bool maximumPriorityQueueSizeChanged;
    GLMotif::PopupWindow* maximumPriorityQueueSizeDialog;
    GLMotif::ToggleButton* priorityQueueTestToggle;
    GLMotif::TextField* maximumPriorityQueueSizeValue;
    int numberOfClippingPlanes;
    int numberOfComponents;
    int numberOfFocusAndContextPlanes;
    bool ratioOfVisibilityTest;
    float ratioOfVisibilityThreshold;
    GLMotif::PopupWindow* ratioOfVisibilityThresholdDialog;
    GLMotif::ToggleButton* ratioTestToggle;
    GLMotif::TextField* ratioOfVisibilityThresholdValue;
    float* red;
    float redScale;
    const char* redVolumeFile;
    float renderingCost;
    GLMotif::PopupWindow* renderingCostFunctionDialog;
    bool renderingCostFunctionTest;
    GLMotif::ToggleButton* renderingCostFunctionTestToggle;
    GLMotif::TextField* renderingCostValue;
    bool rgbChanged;
    RGBTransferFunction1D* rgbTransferFunction1D;
    Scene* scene;
    bool secondFrame;
    Shading * shading;
    bool showBricks;
    bool showOcNodes;
    bool showOutline;
    bool showVolume;
    GLMotif::ToggleButton * showTransferFunction1DToggle;
    GLMotif::ToggleButton * showTransferFunction2_5DToggle;
    Slices * slices;
    unsigned char* sliceColormap;
    bool sliceColorMapChanged;
    GLMotif::PopupWindow* sliceFactorDialog;
    GLMotif::TextField* sliceFactorValue;
    TransferFunction1D * transferFunction1D;
    TransferFunction2_5D * transferFunction2_5D;
    bool transferFunction2_5DChanged;
    int typeOfComponents;
     Volume* volume;
    const char* volumeFile;
    void changeAnalysisToolsCallback(GLMotif::RadioBox::ValueChangedCallbackData* cbData);
    void createAlphaSubMenu(GLMotif::Menu* mainMenu);
    GLMotif::Popup* createAnalysisToolsSubMenu(void);
    GLMotif::Popup* createBlueSubMenu(void);
    GLMotif::Popup* createColorMapSubMenu(void);
    GLMotif::Popup* createComponentSubMenu(void);
    GLMotif::Popup* createGreenSubMenu(void);
    GLMotif::Popup* createGridSubMenu(void);
    GLMotif::PopupMenu* createMainMenu(void);
    GLMotif::PopupWindow* createMaximumPriorityQueueSizeDialog(void);
    GLMotif::PopupWindow* createMotionInterpolationDialog(void);
    GLMotif::PopupWindow* createRatioOfVisibilityThresholdDialog(void);
    GLMotif::Popup* createRedSubMenu(void);
    GLMotif::PopupWindow* createRenderingCostFunctionDialog(void);
    GLMotif::Popup* createSettingsSubMenu(void);
    GLMotif::PopupWindow* createSliceFactorDialog(void);
    void initialize(void);
    void processCommandLineArguments(int& argc, char**& argv) throw (std::runtime_error);
    virtual void toolCreationCallback(Vrui::ToolManager::ToolCreationCallbackData* cbData);
    virtual void toolDestructionCallback(Vrui::ToolManager::ToolDestructionCallbackData* cbData);

	/* Credit Information - Elements and Methods */
	bool creditInformation;
	std::string creditTitle;
	std::string creditData;
	std::string creditGraphics;
	void assignCreditInformation(void) const;
	void readCreditFile(const char * creditFileName);
};

#endif
