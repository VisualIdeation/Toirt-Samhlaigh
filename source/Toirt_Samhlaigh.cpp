/*
 * Toirt_Samhlaigh.cpp - Methods for Toirt_Samhlaigh class.
 *
 * Author: Patrick O'Leary
 * Created: October 21, 2007
 * Copyright: 2007 - 2008
 */
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

#include <GL/gl.h>

/* Vrui includes */
#include <GL/GLContextData.h>
#include <GLMotif/Button.h>
#include <GLMotif/CascadeButton.h>
#include <GLMotif/SubMenu.h>
#include <GLMotif/WidgetManager.h>
#include <Misc/ThrowStdErr.h>
#include <Misc/File.h>
#include <Misc/StandardValueCoders.h>
#include <Misc/ConfigurationFile.h>
#include <Vrui/VRWindow.h>

/* Vrui includes to use the Vrui interface */
#include <Vrui/Vrui.h>

#include <ANALYSIS/BaseLocator.h>
#include <ANALYSIS/ClippingPlane.h>
#include <ANALYSIS/ClippingPlaneLocator.h>
#include <ANALYSIS/FocusAndContextPlane.h>
#include <ANALYSIS/FocusAndContextPlaneLocator.h>
#include <ANALYSIS/MeasurementLocator.h>
#include <ANALYSIS/PerspectiveTool.h>
#include <COLORMAP/ColorMap.h>
#include <DATA/Volume.h>
#include <GRAPHIC/Scene.h>
#include <GUI/Animation.h>
#include <GUI/DownSampling.h>
#include <GUI/FeatureEnhancement.h>
#include <GUI/Lighting.h>
#include <GUI/Materials.h>
#include <GUI/RGBTransferFunction1D.h>
#include <GUI/ScalarWidget.h>
#include <GUI/Shading.h>
#include <GUI/Slices.h>
#include <GUI/TransferFunction1D.h>
#include <GUI/TransferFunction2_5D.h>
#include <IO/IOHelper.h>
#include <MATH/Vector4.h>
#include <UTILITY/Stringify.h>
#include <UTILITY/StringTokenizer.h>

#include <Toirt_Samhlaigh.h>

/* Toirt_Samhlaigh - Constructor for Toirt_Samhlaigh class.
 * 		extends Vrui::Application
 *
 * parameter argc - int&
 * parameter argv - char**&
 * parameter appDefaults
 */
Toirt_Samhlaigh::Toirt_Samhlaigh(int& argc, char**& argv, char**& appDefaults) :
    Vrui::Application(argc, argv, appDefaults), alphaChanged(false), analysisTool(0), animating(false), animation(NULL),
    baseLocators(0), blueScale(1.0), blueVolumeFile(NULL), clippingPlanes(0), colorMapChanged(true), creditInformation(false), downSampling(NULL),
    firstFrame(true), focusAndContextPlanes(0), greenScale(1.0), greenVolumeFile(NULL), interactive(false),
            lighting(NULL), materials(NULL),
            mainMenu(NULL), maximumPriorityQueueSize(0), maximumPriorityQueueSizeChanged(false), maximumPriorityQueueSizeDialog(
                    NULL), maximumPriorityQueueSizeValue(NULL), maximumPriorityQueueTest(false), numberOfComponents(0),
            priorityQueueTestToggle(NULL), ratioOfVisibilityTest(false), ratioOfVisibilityThreshold(1.0),
            ratioOfVisibilityThresholdDialog(NULL), ratioOfVisibilityThresholdValue(NULL), ratioTestToggle(NULL), redScale(1.0),
            redVolumeFile(NULL), renderingCost(0.2), renderingCostFunctionDialog(NULL), renderingCostFunctionTest(true),
            renderingCostFunctionTestToggle(NULL), renderingCostValue(NULL), rgbChanged(false), rgbTransferFunction1D(NULL),
            secondFrame(false), scene(NULL), shading(NULL), showBricks(false), showOcNodes(false), showOutline(true),
            showTransferFunction1DToggle(NULL), showTransferFunction2_5DToggle(NULL), showVolume(false),
            sliceColorMapChanged(true), sliceFactorDialog(NULL), slices(NULL), sliceFactorValue(NULL), transferFunction1D(NULL),
            transferFunction2_5DChanged(false), transferFunction2_5D(NULL), typeOfComponents(0), volumeFile(NULL) {
    volume = new Volume;
    try {
        processCommandLineArguments(argc, argv);
    } catch (std::runtime_error e) {
        std::cerr << "Error: Exception " << e.what() << "!" << std::endl;
    }
    alpha = new float[256];
    alpha2_5D = new float[256 * 256];
    red = new float[256];
    green = new float[256];
    blue = new float[256];
    colormap = new unsigned char[256 * 256 * 4];
    sliceColormap = new unsigned char[256 * 256 * 4];
    initialize();
}

/*
 * ~Toirt_Samhlaigh - Destructor for Toirt_Samhlaigh class.
 */
Toirt_Samhlaigh::~Toirt_Samhlaigh(void) {
    if (animation != NULL)
        delete animation;
    delete[] alpha;
    delete[] alpha2_5D;
    delete[] red;
    delete[] green;
    delete[] blue;
    delete[] colormap;
    if (downSampling != NULL)
        delete downSampling;
    delete[] sliceColormap;
    blueVolumeFile = NULL;
    greenVolumeFile = NULL;
    redVolumeFile = NULL;
    volumeFile = NULL;
    if (mainMenu != NULL)
        delete mainMenu;
    //if (showTransferFunction1DToggle != NULL)
        //delete showTransferFunction1DToggle;
    //if (showTransferFunction2_5DToggle != NULL)
        //delete showTransferFunction2_5DToggle;
    if (lighting != NULL)
        delete lighting;
    if (materials != NULL)
        delete materials;
    if (maximumPriorityQueueSizeDialog != NULL)
        delete maximumPriorityQueueSizeDialog;
    if (ratioOfVisibilityThresholdDialog != NULL)
        delete ratioOfVisibilityThresholdDialog;
    if (renderingCostFunctionDialog != NULL)
        delete renderingCostFunctionDialog;
    if (rgbTransferFunction1D != NULL)
        delete rgbTransferFunction1D;
    if (scene != NULL)
        delete scene;
    if (shading != NULL)
        delete shading;
    if (slices != NULL)
        delete slices;
    if (sliceFactorDialog != NULL)
        delete sliceFactorDialog;
    if (transferFunction1D != NULL)
        delete transferFunction1D;
    if (transferFunction2_5D != NULL)
        delete transferFunction2_5D;
    delete[] clippingPlanes;
    for (BaseLocatorList::iterator blIt = baseLocators.begin(); blIt != baseLocators.end(); ++blIt)
        delete *blIt;
    delete volume;
}

/*
 * alphaChangedCallback - Change the alpha.
 *
 * parameter callBackData - Misc::CallbackData*
 */
void Toirt_Samhlaigh::alphaChangedCallback(Misc::CallbackData* callBackData) {
    if (scene->getPreintegrated()) {
        transferFunction1D->exportPreIntegrationColorMap(colormap);
    } else {
        transferFunction1D->exportAlpha(colormap);
    }
    for (int i = 0; i < 256; i++) {
        alpha[i] = float(colormap[4 * (i * 256 + i) + 3]) / 255.0f;
    }
    if (!interactive) {
        if (transferFunction1D->isDragging() && transferFunction1D->isInteractive()) {
            scene->setInteractive(true);
        } else {
            scene->setInteractive(false);
            scene->updateAlpha(alpha);
            alphaChanged = true;
        }
    }
    scene->updateColorMap(colormap);
    colorMapChanged = true;
    Vrui::requestUpdate();
} // end alphaChangedCallback()

/*
 * assignCreditInformation - Assign Credit Information to VRindow
 */
void Toirt_Samhlaigh::assignCreditInformation(void) const
	{

	if (creditInformation)
		{
		std::cout << "hey" << std::endl;

		/* Assign Credit Information */
		for(int i=0;i<Vrui::getNumWindows();i++)
			{
			Vrui::VRWindow * window = Vrui::getWindow(i);
			window->setCreditTitle((creditTitle).c_str());
			window->setCreditData((creditData).c_str());
			window->setCreditGraphics((creditGraphics).c_str());
			}
		}
	} // end assignCreditInformation()

/*
 * blueChangedCallback - Change the blue.
 *
 * parameter callBackData - Misc::CallbackData*
 */
void Toirt_Samhlaigh::blueChangedCallback(Misc::CallbackData* callBackData) {
    if (scene->getPreintegrated()) {
        rgbTransferFunction1D->exportPreIntegrationColorMap(colormap);
    } else {
        rgbTransferFunction1D->exportBlue(colormap);
    }
    for (int i = 0; i < 256; i++) {
        blue[i] = blueScale * float(colormap[4 * (i * 256 + i) + 2]) / 255.0f;
    }
    if (!interactive) {
        if (rgbTransferFunction1D->isDragging() && rgbTransferFunction1D->isInteractive()) {
            scene->setInteractive(true);
        } else {
            scene->setInteractive(false);
            scene->updateBlue(blue);
            rgbChanged = true;
        }
    }
    scene->updateColorMap(colormap);
    colorMapChanged = true;
    Vrui::requestUpdate();
} // end blueChangedCallback()

/*
 * calculateNumberOfLeafNodesInOctree
 *
 * n = 8^d
 *
 * Find the maximum number of octree leaf nodes, n, where d is
 * the depth of the octree
 *
 * d = log(s/c)/log(2)
 *
 * s is the maximum texture size and c is the maximum leaf
 * size.
 *
 * return - float
 */
float Toirt_Samhlaigh::calculateNumberOfLeafNodesInOctree(void) {
    float result = float(pow(8, floor(log(double(volume->getTextureSize()) / double(volume->getLeafSize()) - 1.0f) / log(2))));
    return result;
}
/*
 * centerDisplayCallback - Center the display for world coordinates of the volume.
 *
 * parameter callBackData - Misc::CallbackData*
 */
void Toirt_Samhlaigh::centerDisplayCallback(Misc::CallbackData* callBackData) {
    Vrui::Point center = Geometry::mid(volume->getMinimum(), volume->getMaximum());
    Vrui::Scalar radius = Geometry::dist(volume->getMinimum(), volume->getMaximum());
    Vrui::setNavigationTransformation(center, radius);
}

/*
 * changeAlphaCallback
 *
 * parameter callBackData - GLMotif::RadioBox::ValueChangedCallbackData*
 */
void Toirt_Samhlaigh::changeAlphaCallback(GLMotif::RadioBox::ValueChangedCallbackData* callBackData) {
    /* Set the new alpha ramp */
    int value = callBackData->radioBox->getToggleIndex(callBackData->newSelectedToggle);
    transferFunction1D->changeAlpha(value);
    if (scene->getPreintegrated()) {
        transferFunction1D->exportPreIntegrationColorMap(colormap);
    } else {
        transferFunction1D->exportAlpha(colormap);
    }
    for (int i = 0; i < 256; i++) {
        alpha[i] = float(colormap[4 * (i * 256 + i) + 3]) / 255.0f;
    }
    scene->updateAlpha(alpha);
    alphaChanged = true;
    scene->updateColorMap(colormap);
    colorMapChanged = true;
    Vrui::requestUpdate();
} // end changeAlphaCallback()

/*
 * changeAnalysisToolsCallback
 *
 * parameter callbackData - GLMotif::RadioBox::ValueChangedCallbackData *
 */
void Toirt_Samhlaigh::changeAnalysisToolsCallback(GLMotif::RadioBox::ValueChangedCallbackData * callbackData) {
    /* Set the new analysis tool: */
    analysisTool = callbackData->radioBox->getToggleIndex(callbackData->newSelectedToggle);
} // end changeAnalysisToolsCallback()

/*
 * changeBlueCallback
 *
 * parameter callBackData - GLMotif::RadioBox::ValueChangedCallbackData*
 */
void Toirt_Samhlaigh::changeBlueCallback(GLMotif::RadioBox::ValueChangedCallbackData* callBackData) {
    int value = callBackData->radioBox->getToggleIndex(callBackData->newSelectedToggle);
    rgbTransferFunction1D->changeBlue(value);
    if (scene->getPreintegrated()) {
        rgbTransferFunction1D->exportPreIntegrationColorMap(colormap);
    } else {
        rgbTransferFunction1D->exportBlue(colormap);
    }
    scene->updateColorMap(colormap);
    for (int i = 0; i < 255; i++) {
        blue[i] = blueScale * float(colormap[4 * (i * 256 + i) + 2]) / 255.0f;
    }
    scene->updateBlue(blue);
    rgbChanged = true;
    scene->updateColorMap(colormap);
    colorMapChanged = true;
    Vrui::requestUpdate();
} // end changeBlueCallback()

/*
 * changeColorMapCallback
 *
 * parameter callBackData - GLMotif::RadioBox::ValueChangedCallbackData*
 */
void Toirt_Samhlaigh::changeColorMapCallback(GLMotif::RadioBox::ValueChangedCallbackData* callBackData) {
    /* Set the new color map */
    int value = callBackData->radioBox->getToggleIndex(callBackData->newSelectedToggle);
    transferFunction1D->changeColorMap(value);
    if (scene->getPreintegrated()) {
        transferFunction1D->exportPreIntegrationColorMap(colormap);
    } else {
        transferFunction1D->exportColorMap(colormap);
    }
    scene->updateColorMap(colormap);
    colorMapChanged = true;
    Vrui::requestUpdate();
} // end changeColorMapCallback()

/*
 * changeGreenCallback
 *
 * parameter callBackData - GLMotif::RadioBox::ValueChangedCallbackData*
 */
void Toirt_Samhlaigh::changeGreenCallback(GLMotif::RadioBox::ValueChangedCallbackData* callBackData) {
    int value = callBackData->radioBox->getToggleIndex(callBackData->newSelectedToggle);
    rgbTransferFunction1D->changeGreen(value);
    if (scene->getPreintegrated()) {
        rgbTransferFunction1D->exportPreIntegrationColorMap(colormap);
    } else {
        rgbTransferFunction1D->exportGreen(colormap);
    }
    scene->updateColorMap(colormap);
    for (int i = 0; i < 255; i++) {
        green[i] = greenScale * float(colormap[4 * (i * 256 + i) + 1]) / 255.0f;
    }
    scene->updateGreen(green);
    rgbChanged = true;
    scene->updateColorMap(colormap);
    colorMapChanged = true;
    Vrui::requestUpdate();
} // end changeGreenCallback()

/*
 * changeRedCallback
 *
 * parameter callBackData - GLMotif::RadioBox::ValueChangedCallbackData*
 */
void Toirt_Samhlaigh::changeRedCallback(GLMotif::RadioBox::ValueChangedCallbackData* callBackData) {
    int value = callBackData->radioBox->getToggleIndex(callBackData->newSelectedToggle);
    rgbTransferFunction1D->changeRed(value);
    if (scene->getPreintegrated()) {
        rgbTransferFunction1D->exportPreIntegrationColorMap(colormap);
    } else {
        rgbTransferFunction1D->exportRed(colormap);
    }
    scene->updateColorMap(colormap);
    for (int i = 0; i < 255; i++) {
        red[i] = redScale * float(colormap[4 * (i * 256 + i) + 0]) / 255.0f;
    }
    scene->updateRed(red);
    rgbChanged = true;
    scene->updateColorMap(colormap);
    colorMapChanged = true;
    Vrui::requestUpdate();
} // end changeRedCallback()

/*
 * colorMapChangedCallback - Change the ColorMap.
 *
 * parameter callBackData - Misc::CallbackData*
 */
void Toirt_Samhlaigh::colorMapChangedCallback(Misc::CallbackData* callBackData) {
    if (scene->getPreintegrated()) {
        transferFunction1D->exportPreIntegrationColorMap(colormap);
    } else {
        transferFunction1D->exportColorMap(colormap);
    }
    scene->updateColorMap(colormap);
    colorMapChanged = true;
    Vrui::requestUpdate();
} // end colorMapChangedCallback()

/*
 * colorMap2_5DChangedCallback - Change the ColorMap.
 *
 * parameter callBackData - Misc::CallbackData*
 */
void Toirt_Samhlaigh::colorMap2_5DChangedCallback(Misc::CallbackData* callBackData) {
    transferFunction2_5D->exportColorMap(colormap);
    scene->updateColorMap(colormap);
    if (!interactive) {
        if (transferFunction2_5D->isDragging() && transferFunction2_5D->isInteractive()) {
            scene->setInteractive(true);
        } else {
            scene->setInteractive(false);
            for (int i = 0; i < 256 * 256; i++) {
                alpha2_5D[i] = colormap[4 * i + 3];
            }
            scene->updateAlpha2_5D(alpha2_5D);
            alphaChanged = true;
        }
    }
    colorMapChanged = true;
    Vrui::requestUpdate();
} // end colorMap2_5DChangedCallback()

/*
 * createAlphaSubMenu - Creates a submenu of the alpha ramps.
 *
 * parameter mainMenu - GLMotif::Menu*
 */
void Toirt_Samhlaigh::createAlphaSubMenu(GLMotif::Menu* mainMenu) {
    if (volume->getNumberOfComponents() == 0) {
        GLMotif::CascadeButton * alphaSubCascade = new GLMotif::CascadeButton("AlphaSubCascade", mainMenu, "Opacity Ramp");
        GLMotif::Popup * alphaSubMenuPopup = new GLMotif::Popup("AlphaSubMenuPopup", Vrui::getWidgetManager());
        GLMotif::RadioBox* alphas = new GLMotif::RadioBox("Alphas", alphaSubMenuPopup, false);
        alphas->setSelectionMode(GLMotif::RadioBox::ALWAYS_ONE);
        alphas->addToggle("Up");
        alphas->addToggle("Down");
        alphas->addToggle("Constant");
        alphas->addToggle("Seismic");
        alphas->setSelectedToggle(0);
        alphas->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::changeAlphaCallback);
        alphas->manageChild();
        alphaSubCascade->setPopup(alphaSubMenuPopup);
    } else {
        GLMotif::CascadeButton * opacitySubCascade = new GLMotif::CascadeButton("OpacitySubCascade", mainMenu, "Opacity Ramps");
        GLMotif::Popup * opacitySubMenuPopup = new GLMotif::Popup("OpacitySubMenuPopup", Vrui::getWidgetManager());
        GLMotif::Menu* opacityMenu = new GLMotif::Menu("OpacityMenu", opacitySubMenuPopup, false);
        GLMotif::CascadeButton * blueSubCascade =
                new GLMotif::CascadeButton("BlueSubMenuCascade", opacityMenu, "Blue Opacity Ramp");
        blueSubCascade->setPopup(createBlueSubMenu());
        GLMotif::CascadeButton * greenSubCascade = new GLMotif::CascadeButton("GreenSubMenuCascade", opacityMenu,
                "Green Opacity Ramp");
        greenSubCascade->setPopup(createGreenSubMenu());
        GLMotif::CascadeButton * redSubCascade = new GLMotif::CascadeButton("RedSubMenuCascade", opacityMenu, "Red Opacity Ramp");
        redSubCascade->setPopup(createRedSubMenu());
        opacityMenu->manageChild();
        opacitySubCascade->setPopup(opacitySubMenuPopup);
    }
} // end createAlphaSubMenu()

/*
 * createAnalysisToolsSubMenu
 *
 * return - GLMotif::Popup *
 */
GLMotif::Popup * Toirt_Samhlaigh::createAnalysisToolsSubMenu(void) {
    GLMotif::Popup * analysisToolsMenuPopup = new GLMotif::Popup("analysisToolsMenuPopup", Vrui::getWidgetManager());
    GLMotif::RadioBox * analysisTools = new GLMotif::RadioBox("analysisTools", analysisToolsMenuPopup, false);
    analysisTools->setSelectionMode(GLMotif::RadioBox::ALWAYS_ONE);

    /* Add the clipping plane analysisTool: */
    int analysisToolIndex = 0;
    analysisTools->addToggle("Clipping Plane");
    ++analysisToolIndex;

    analysisTools->addToggle("Measurement Tool");
    ++analysisToolIndex;

    analysisTools->addToggle("Perspective Tool");
    ++analysisToolIndex;

    analysisTools->setSelectedToggle(analysisTool);
    analysisTools->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::changeAnalysisToolsCallback);

    analysisTools->manageChild();

    return analysisToolsMenuPopup;
} // end createAnalysisToolsSubMenu()

/*
 * createBlueSubMenu - Creates a submenu of the blue alpha ramps.
 *
 * return - GLMotif::Popup*
 */
GLMotif::Popup* Toirt_Samhlaigh::createBlueSubMenu(void) {
    GLMotif::Popup* blueSubMenuPopup = new GLMotif::Popup("BlueSubMenuPopup", Vrui::getWidgetManager());
    GLMotif::RadioBox* blues = new GLMotif::RadioBox("Blues", blueSubMenuPopup, false);
    blues->setSelectionMode(GLMotif::RadioBox::ALWAYS_ONE);
    blues->addToggle("Up");
    blues->addToggle("Down");
    blues->addToggle("Constant");
    blues->setSelectedToggle(0);
    blues->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::changeBlueCallback);
    blues->manageChild();
    return blueSubMenuPopup;
} // end createBlueSubMenu()

/*
 * createColorMapSubMenu - Creates a submenu of the available color maps.
 *
 * return - GLMotif::Popup*
 */
GLMotif::Popup* Toirt_Samhlaigh::createColorMapSubMenu(void) {
    GLMotif::Popup * colorMapSubMenuPopup = new GLMotif::Popup("ColorMapSubMenuPopup", Vrui::getWidgetManager());
    GLMotif::RadioBox* colorMaps = new GLMotif::RadioBox("ColorMaps", colorMapSubMenuPopup, false);
    colorMaps->setSelectionMode(GLMotif::RadioBox::ALWAYS_ONE);
    colorMaps->addToggle("Full Rainbow");
    colorMaps->addToggle("Inverse Full Rainbow");
    colorMaps->addToggle("Rainbow");
    colorMaps->addToggle("Inverse Rainbow");
    colorMaps->addToggle("Cold to Hot");
    colorMaps->addToggle("Hot to Cold");
    colorMaps->addToggle("Black to White");
    colorMaps->addToggle("White to Black");
    colorMaps->addToggle("HSB Hues");
    colorMaps->addToggle("Inverse HSB Hues");
    colorMaps->addToggle("Davinci");
    colorMaps->addToggle("Inverse Davinci");
    colorMaps->addToggle("Seismic");
    colorMaps->addToggle("Inverse Seismic");
    colorMaps->setSelectedToggle(3);
    colorMaps->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::changeColorMapCallback);
    colorMaps->manageChild();
    return colorMapSubMenuPopup;
} // end createColorMapSubMenu()

/*
 * createComponentSubMenu - Creates a submenu of the available Component Modes.
 *
 * return - GLMotif::Popup*
 */
GLMotif::Popup* Toirt_Samhlaigh::createComponentSubMenu(void) {
    GLMotif::Popup * componentSubMenuPopup = new GLMotif::Popup("ComponentSubMenuPopup", Vrui::getWidgetManager());
    GLMotif::SubMenu * componentSubMenu = new GLMotif::SubMenu("ComponentSubMenu", componentSubMenuPopup, false);
    if (typeOfComponents == 7) {
        GLMotif::ToggleButton * showRedToggle = new GLMotif::ToggleButton("ShowRedToggle", componentSubMenu, "Show Red Component");
        showRedToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
        showRedToggle->setToggle(true);
        GLMotif::ToggleButton * showGreenToggle = new GLMotif::ToggleButton("ShowGreenToggle", componentSubMenu,
                "Show Green Component");
        showGreenToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
        showGreenToggle->setToggle(true);
        GLMotif::ToggleButton * showBlueToggle = new GLMotif::ToggleButton("ShowBlueToggle", componentSubMenu,
                "Show Blue Component");
        showBlueToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
        showBlueToggle->setToggle(true);
    } else if (typeOfComponents == 6) {
        GLMotif::ToggleButton * showRedToggle = new GLMotif::ToggleButton("ShowRedToggle", componentSubMenu, "Show Red Component");
        showRedToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
        showRedToggle->setToggle(true);
        GLMotif::ToggleButton * showGreenToggle = new GLMotif::ToggleButton("ShowGreenToggle", componentSubMenu,
                "Show Green Component");
        showGreenToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
        showGreenToggle->setToggle(true);
    } else if (typeOfComponents == 5) {
        GLMotif::ToggleButton * showRedToggle = new GLMotif::ToggleButton("ShowRedToggle", componentSubMenu, "Show Red Component");
        showRedToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
        showRedToggle->setToggle(true);
        GLMotif::ToggleButton * showBlueToggle = new GLMotif::ToggleButton("ShowBlueToggle", componentSubMenu,
                "Show Blue Component");
        showBlueToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
        showBlueToggle->setToggle(true);
    } else if (typeOfComponents == 4) {
        GLMotif::ToggleButton * showRedToggle = new GLMotif::ToggleButton("ShowRedToggle", componentSubMenu, "Show Red Component");
        showRedToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
        showRedToggle->setToggle(true);
    } else if (typeOfComponents == 3) {
        GLMotif::ToggleButton * showGreenToggle = new GLMotif::ToggleButton("ShowGreenToggle", componentSubMenu,
                "Show Green Component");
        showGreenToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
        showGreenToggle->setToggle(true);
        GLMotif::ToggleButton * showBlueToggle = new GLMotif::ToggleButton("ShowBlueToggle", componentSubMenu,
                "Show Blue Component");
        showBlueToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
        showBlueToggle->setToggle(true);
    } else if (typeOfComponents == 2) {
        GLMotif::ToggleButton * showGreenToggle = new GLMotif::ToggleButton("ShowGreenToggle", componentSubMenu,
                "Show Green Component");
        showGreenToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
        showGreenToggle->setToggle(true);
    } else if (typeOfComponents == 1) {
        GLMotif::ToggleButton * showBlueToggle = new GLMotif::ToggleButton("ShowBlueToggle", componentSubMenu,
                "Show Blue Component");
        showBlueToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
        showBlueToggle->setToggle(true);
    }
    componentSubMenu->manageChild();
    return componentSubMenuPopup;
} // end createComponentSubMenu()

/*
 * createGreenSubMenu - Creates a submenu of the green alpha ramps.
 *
 * return - GLMotif::Popup*
 */
GLMotif::Popup* Toirt_Samhlaigh::createGreenSubMenu(void) {
    GLMotif::Popup* greenSubMenuPopup = new GLMotif::Popup("GreenSubMenuPopup", Vrui::getWidgetManager());
    GLMotif::RadioBox* greens = new GLMotif::RadioBox("Greens", greenSubMenuPopup, false);
    greens->setSelectionMode(GLMotif::RadioBox::ALWAYS_ONE);
    greens->addToggle("Up");
    greens->addToggle("Down");
    greens->addToggle("Constant");
    greens->setSelectedToggle(0);
    greens->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::changeGreenCallback);
    greens->manageChild();
    return greenSubMenuPopup;
} // end createGreenSubMenu()

/*
 * createGridSubMenu - Creates a submenu of the available Grid Modes.
 *
 * return - GLMotif::Popup*
 */
GLMotif::Popup* Toirt_Samhlaigh::createGridSubMenu(void) {
    GLMotif::Popup* gridSubMenuPopup = new GLMotif::Popup("GridSubMenuPopup", Vrui::getWidgetManager());
    GLMotif::SubMenu* gridSubMenu = new GLMotif::SubMenu("GridSubMenu", gridSubMenuPopup, false);
    GLMotif::ToggleButton * showOutlineToggle = new GLMotif::ToggleButton("ShowOutlineToggle", gridSubMenu, "Show Outline");
    showOutlineToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
    showOutlineToggle->setToggle(true);
    GLMotif::ToggleButton * showBricksToggle = new GLMotif::ToggleButton("ShowBricksToggle", gridSubMenu, "Show Bricks");
    showBricksToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
    GLMotif::ToggleButton * showOcNodesToggle = new GLMotif::ToggleButton("ShowOcNodesToggle", gridSubMenu, "Show OcNodes");
    showOcNodesToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
    gridSubMenu->manageChild();
    return gridSubMenuPopup;
} // end createGridSubMenu()

/*
 * createMainMenu - Creates a main menu for the Toirt_Samhlaigh application.
 *
 * return - GLMotif::PopupMenu*
 */
GLMotif::PopupMenu* Toirt_Samhlaigh::createMainMenu(void) {
    GLMotif::PopupMenu* mainMenuPopup = new GLMotif::PopupMenu("MainMenuPopup", Vrui::getWidgetManager());
    mainMenuPopup->setTitle("Toirt Samhlaigh");
    GLMotif::Menu* mainMenu = new GLMotif::Menu("MainMenu", mainMenuPopup, false);
    GLMotif::ToggleButton * showVolumeToggle = new GLMotif::ToggleButton("ShowVolumeToggle", mainMenu, "Show Volume");
    showVolumeToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
    showVolumeToggle->setToggle(showVolume);
    GLMotif::ToggleButton * interactiveToggle = new GLMotif::ToggleButton("InteractiveToggle", mainMenu, "Interactive");
    interactiveToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
    interactiveToggle->setToggle(interactive);
    GLMotif::ToggleButton * downSamplingToggle = new GLMotif::ToggleButton("DownSamplingToggle", mainMenu, "Down Sampling");
    downSamplingToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
    downSamplingToggle->setToggle(false);
    GLMotif::CascadeButton * gridSubCascade = new GLMotif::CascadeButton("GridSubCascade", mainMenu, "Grid Modes");
    gridSubCascade->setPopup(createGridSubMenu());
    GLMotif::CascadeButton * analysisToolsCascade = new GLMotif::CascadeButton("AnalysisToolsCascade", mainMenu, "Analysis Tools");
    analysisToolsCascade->setPopup(createAnalysisToolsSubMenu());
    if (numberOfComponents != 0) {
        GLMotif::CascadeButton * componentSubCascade = new GLMotif::CascadeButton("ComponentSubCascade", mainMenu,
                "Component Modes");
        componentSubCascade->setPopup(createComponentSubMenu());
    } else {
        GLMotif::CascadeButton * colorMapSubCascade = new GLMotif::CascadeButton("ColorMapSubCascade", mainMenu, "Color Map");
        colorMapSubCascade->setPopup(createColorMapSubMenu());
    }
    createAlphaSubMenu(mainMenu);
    GLMotif::CascadeButton * settingsSubCascade = new GLMotif::CascadeButton("SettingsSubCascade", mainMenu, "Settings");
    settingsSubCascade->setPopup(createSettingsSubMenu());
    GLMotif::ToggleButton * ShowAnimationDialogToggle = new GLMotif::ToggleButton("ShowAnimationDialogToggle", mainMenu,
            "Animation");
    ShowAnimationDialogToggle->setToggle(false);
    ShowAnimationDialogToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
    GLMotif::Button * centerDisplayButton = new GLMotif::Button("CenterDisplayButton", mainMenu, "Center Display");
    centerDisplayButton->getSelectCallbacks().add(this, &Toirt_Samhlaigh::centerDisplayCallback);
    showTransferFunction1DToggle = new GLMotif::ToggleButton("ShowTransferFunction1DToggle", mainMenu,
            "1D Transfer Function Editor");
    showTransferFunction1DToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::showTransferFunction1DCallback);
    if (volume->getNumberOfComponents() == 0) {
        showTransferFunction2_5DToggle = new GLMotif::ToggleButton("ShowTransferFunction2_5DToggle", mainMenu,
                "2.5D Transfer Function Editor");
        showTransferFunction2_5DToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::showTransferFunction2_5DCallback);
    }
    GLMotif::ToggleButton * showSlicesToggle = new GLMotif::ToggleButton("ShowSlicesToggle", mainMenu, "Slices");
    showSlicesToggle->setToggle(false);
    showSlicesToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
    GLMotif::ToggleButton * showShadingToggle = new GLMotif::ToggleButton("ShowShadingToggle", mainMenu, "Shading");
    showShadingToggle->setToggle(false);
    showShadingToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
    GLMotif::ToggleButton * showFeatureEnhancementToggle = new GLMotif::ToggleButton("ShowFeatureEnhancementToggle", mainMenu,
            "Feature Enhancement");
    showFeatureEnhancementToggle->setToggle(false);
    showFeatureEnhancementToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
    GLMotif::ToggleButton * showLightingToggle = new GLMotif::ToggleButton("ShowLightingToggle", mainMenu, "Lighting");
    showLightingToggle->setToggle(false);
    showLightingToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
    GLMotif::ToggleButton * showMaterialsToggle = new GLMotif::ToggleButton("ShowMaterialsToggle", mainMenu, "Materials");
    showMaterialsToggle->setToggle(false);
    showMaterialsToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);

    mainMenu->manageChild();
    return mainMenuPopup;
}

/*
 * createMaximumPriorityQueueSizeDialog
 *
 * return - GLMotif::PopupWindow*
 */
GLMotif::PopupWindow* Toirt_Samhlaigh::createMaximumPriorityQueueSizeDialog(void) {
    const GLMotif::StyleSheet& ss = *Vrui::getWidgetManager()->getStyleSheet();
    GLMotif::PopupWindow * dialogPopup = new GLMotif::PopupWindow("MaximumPriorityQueueSizeDialogPopup", Vrui::getWidgetManager(),
            "Maximum Priority Queue Size");
    GLMotif::RowColumn * dialog = new GLMotif::RowColumn("MaximumPriorityQueueSizeDialog", dialogPopup, false);
    dialog->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    priorityQueueTestToggle = new GLMotif::ToggleButton("PriorityQueueTestToggle", dialog, "ON");
    priorityQueueTestToggle->setToggle(maximumPriorityQueueTest);
    priorityQueueTestToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
    maximumPriorityQueueSizeValue = new GLMotif::TextField("MaximumPriorityQueueSizeValue", dialog, 6);
    maximumPriorityQueueSizeValue->setFieldWidth(6);
    maximumPriorityQueueSizeValue->setPrecision(0);
    maximumPriorityQueueSizeValue->setValue(0);
    GLMotif::Slider * maximumPriorityQueueSizeSlider = new GLMotif::Slider("MaximumPriorityQueueSizeSlider", dialog,
            GLMotif::Slider::HORIZONTAL, ss.fontHeight * 10.0f);
    maximumPriorityQueueSizeSlider->setValueRange(0, calculateNumberOfLeafNodesInOctree(), 1);
    maximumPriorityQueueSizeSlider->setValue(0);
    maximumPriorityQueueSizeSlider->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::sliderCallback);
    dialog->manageChild();
    return dialogPopup;
}

/*
 * createRatioOfVisibilityThresholdDialog
 *
 * return - GLMotif::PopupWindow*
 */
GLMotif::PopupWindow* Toirt_Samhlaigh::createRatioOfVisibilityThresholdDialog(void) {
    const GLMotif::StyleSheet& ss = *Vrui::getWidgetManager()->getStyleSheet();
    GLMotif::PopupWindow * dialogPopup = new GLMotif::PopupWindow("RatioOfVisibilityThresholdDialogPopup",
            Vrui::getWidgetManager(), "Ratio Of Visibility Threshold");
    GLMotif::RowColumn * dialog = new GLMotif::RowColumn("RatioOfVisibilityThresholdDialog", dialogPopup, false);
    dialog->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    ratioTestToggle = new GLMotif::ToggleButton("RatioTestToggle", dialog, "ON");
    ratioTestToggle->setToggle(ratioOfVisibilityTest);
    ratioTestToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
    ratioOfVisibilityThresholdValue = new GLMotif::TextField("RatioOfVisibilityThresholdValue", dialog, 6);
    ratioOfVisibilityThresholdValue->setFieldWidth(6);
    ratioOfVisibilityThresholdValue->setPrecision(3);
    ratioOfVisibilityThresholdValue->setValue(volume->getRatioOfVisibilityThreshold());
    GLMotif::Slider * ratioOfVisibilityThresholdSlider = new GLMotif::Slider("RatioOfVisibilityThresholdSlider", dialog,
            GLMotif::Slider::HORIZONTAL, ss.fontHeight * 10.0f);
    ratioOfVisibilityThresholdSlider->setValueRange(0.01, 1.0, 0.01);
    ratioOfVisibilityThresholdSlider->setValue(volume->getRatioOfVisibilityThreshold());
    ratioOfVisibilityThresholdSlider->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::sliderCallback);
    dialog->manageChild();
    return dialogPopup;
}

/*
 * createRedSubMenu - Creates a submenu of the red alpha ramps.
 *
 * return - GLMotif::Popup*
 */
GLMotif::Popup* Toirt_Samhlaigh::createRedSubMenu(void) {
    GLMotif::Popup* redSubMenuPopup = new GLMotif::Popup("RedSubMenuPopup", Vrui::getWidgetManager());
    GLMotif::RadioBox* reds = new GLMotif::RadioBox("Reds", redSubMenuPopup, false);
    reds->setSelectionMode(GLMotif::RadioBox::ALWAYS_ONE);
    reds->addToggle("Up");
    reds->addToggle("Down");
    reds->addToggle("Constant");
    reds->setSelectedToggle(0);
    reds->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::changeRedCallback);
    reds->manageChild();
    return redSubMenuPopup;
} // end createRedSubMenu()

/*
 * createRenderingCostFunctionDialog
 *
 * return - GLMotif::PopupWindow*
 */
GLMotif::PopupWindow* Toirt_Samhlaigh::createRenderingCostFunctionDialog(void) {
    const GLMotif::StyleSheet& ss = *Vrui::getWidgetManager()->getStyleSheet();
    GLMotif::PopupWindow * dialogPopup = new GLMotif::PopupWindow("RenderingCostFunctionDialogPopup", Vrui::getWidgetManager(),
            "Rendering Cost Function");
    GLMotif::RowColumn * dialog = new GLMotif::RowColumn("RenderingCostFunctionDialog", dialogPopup, false);
    dialog->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    renderingCostFunctionTestToggle = new GLMotif::ToggleButton("RenderingCostFunctionTestToggle", dialog, "ON");
    renderingCostFunctionTestToggle->setToggle(renderingCostFunctionTest);
    renderingCostFunctionTestToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
    renderingCostValue = new GLMotif::TextField("RenderingCostValue", dialog, 6);
    renderingCostValue->setFieldWidth(6);
    renderingCostValue->setPrecision(3);
    renderingCostValue->setValue(renderingCost);
    GLMotif::Slider * renderingCostSlider = new GLMotif::Slider("RenderingCostSlider", dialog, GLMotif::Slider::HORIZONTAL,
            ss.fontHeight * 10.0f);
    renderingCostSlider->setValueRange(0.0, 0.5, 0.01);
    renderingCostSlider->setValue(renderingCost);
    renderingCostSlider->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::sliderCallback);
    dialog->manageChild();
    return dialogPopup;
} // end createRenderingCostFunctionDialog()

/*
 * createSettingsSubMenu - Creates a submenu of the application settings.
 *
 * return - GLMotif::Popup*
 */
GLMotif::Popup* Toirt_Samhlaigh::createSettingsSubMenu(void) {
    GLMotif::Popup * settingsSubMenuPopup = new GLMotif::Popup("SettingsSubMenuPopup", Vrui::getWidgetManager());
    GLMotif::SubMenu * settingsSubMenu = new GLMotif::SubMenu("SettingsSubMenu", settingsSubMenuPopup, false);
    GLMotif::ToggleButton * showRatioOfVisibilityThresholdDialogToggle = new GLMotif::ToggleButton(
            "ShowRatioOfVisibilityThresholdDialogToggle", settingsSubMenu, "Show Ratio of Visibility Dialog");
    showRatioOfVisibilityThresholdDialogToggle->setToggle(false);
    showRatioOfVisibilityThresholdDialogToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
    GLMotif::ToggleButton * showRenderingCostFunctionDialogToggle = new GLMotif::ToggleButton(
            "ShowRenderingCostFunctionDialogToggle", settingsSubMenu, "Show Rendering Cost Function Dialog");
    showRenderingCostFunctionDialogToggle->setToggle(false);
    showRenderingCostFunctionDialogToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
    GLMotif::ToggleButton * showMaximumPriorityQueueSizeDialogToggle = new GLMotif::ToggleButton(
            "ShowMaximumPriorityQueueSizeDialogToggle", settingsSubMenu, "Show Maximum Priority Queue Size Dialog");
    showMaximumPriorityQueueSizeDialogToggle->setToggle(false);
    showMaximumPriorityQueueSizeDialogToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
    GLMotif::ToggleButton * showSliceFactorDialogToggle = new GLMotif::ToggleButton("ShowSliceFactorDialogToggle", settingsSubMenu,
            "Show Slice Factor Dialog");
    showSliceFactorDialogToggle->setToggle(false);
    showSliceFactorDialogToggle->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::menuToggleSelectCallback);
    settingsSubMenu->manageChild();
    return settingsSubMenuPopup;
} // end createSettingsSubMenu()

/*
 * createSliceFactorDialog
 *
 * return - GLMotif::PopupWindow*
 */
GLMotif::PopupWindow* Toirt_Samhlaigh::createSliceFactorDialog(void) {
    const GLMotif::StyleSheet& ss = *Vrui::getWidgetManager()->getStyleSheet();
    GLMotif::PopupWindow * dialogPopup = new GLMotif::PopupWindow("SliceFactorDialogPopup", Vrui::getWidgetManager(),
            "Slice Factor");
    GLMotif::RowColumn* dialog = new GLMotif::RowColumn("SliceFactorDialog", dialogPopup, false);
    dialog->setOrientation(GLMotif::RowColumn::HORIZONTAL);
    sliceFactorValue = new GLMotif::TextField("SliceFactorValue", dialog, 6);
    sliceFactorValue->setFieldWidth(6);
    sliceFactorValue->setPrecision(3);
    sliceFactorValue->setValue(volume->getSliceFactor());
    GLMotif::Slider * sliceFactorSlider = new GLMotif::Slider("SliceFactorSlider", dialog, GLMotif::Slider::HORIZONTAL,
            ss.fontHeight * 10.0f);
    sliceFactorSlider->setValueRange(0.05, 4.0, 0.05);
    sliceFactorSlider->setValue(volume->getSliceFactor());
    sliceFactorSlider->getValueChangedCallbacks().add(this, &Toirt_Samhlaigh::sliderCallback);
    dialog->manageChild();
    return dialogPopup;
} // end createSliceFactorDialog()

/*
 * display - Main rendering method.
 *
 * parameter glContextData - GLContextData
 */
void Toirt_Samhlaigh::display(GLContextData& glContextData) const {
    /* Enable all clipping planes: */
    int numberOfSupportedClippingPlanes;
    glGetIntegerv(GL_MAX_CLIP_PLANES, &numberOfSupportedClippingPlanes);
    int clippingPlaneIndex = 0;
    for (int i = 0; i < numberOfClippingPlanes && clippingPlaneIndex < numberOfSupportedClippingPlanes; ++i) {
        if (clippingPlanes[i].isActive()) {
            /* Enable the clipping plane: */
            glEnable(GL_CLIP_PLANE0 + clippingPlaneIndex);
            GLdouble clippingPlane[4];
            for (int j = 0; j < 3; ++j)
                clippingPlane[j] = clippingPlanes[i].getPlane().getNormal()[j];
            clippingPlane[3] = -clippingPlanes[i].getPlane().getOffset();
            glClipPlane(GL_CLIP_PLANE0 + clippingPlaneIndex, clippingPlane);
            /* Go to the next clipping plane: */
            ++clippingPlaneIndex;
        }
    }
    scene->display(glContextData); // draw the scene
    clippingPlaneIndex = 0;
    for (int i = 0; i < numberOfClippingPlanes && clippingPlaneIndex < numberOfSupportedClippingPlanes; ++i) {
        if (clippingPlanes[i].isActive()) {
            /* Disable the clipping plane: */
            glDisable(GL_CLIP_PLANE0 + clippingPlaneIndex);
            /* Go to the next clipping plane: */
            ++clippingPlaneIndex;
        }
    }
    for (int i = baseLocators.size() - 1; i > -1; --i) {
        MeasurementLocator* ml = dynamic_cast<MeasurementLocator*> (baseLocators[i]);
        if (ml) {
            ml->highlightLocator(glContextData);
        }
    }
    for (int i = baseLocators.size() - 1; i > -1; --i) {
        PerspectiveTool* pt = dynamic_cast<PerspectiveTool*> (baseLocators[i]);
        if (pt) {
            scene->clearSpheres();
            scene->setPerspective(true);
            if (pt->drawsphere) {
                for (int x = 0; x < pt->Frames.size(); x++) {
                    scene->setSphere(Geometry::invert(pt->Frames[x]).getOrigin());
                }
            }
        }
    }
}

/*
 * frame - called once per frame Vrui::Application virtual function
 */
void Toirt_Samhlaigh::frame(void) {
    if (firstFrame) {
    	assignCreditInformation();
        firstFrame = false;
        secondFrame = true;
        scene->initialize();
        scene->setShowDisplay(true);
    } else if (secondFrame) {
        secondFrame = false;
    } else {
        if (downSamplingChanged) {
            scene->setDownSamplingChanged(downSamplingChanged);
            downSamplingChanged = false;
        } else {
            scene->setDownSamplingChanged(downSamplingChanged);
        }
        if (transferFunction2_5DChanged) {
            if (volume->getNumberOfComponents() == 0) {
                transferFunction2_5D->setSliceChanged(transferFunction2_5DChanged);
            }
            transferFunction2_5DChanged = false;
        } else {
            if (volume->getNumberOfComponents() == 0) {
                transferFunction2_5D->setSliceChanged(transferFunction2_5DChanged);
            }
        }
        if (colorMapChanged) {
            scene->setColorMapChanged(colorMapChanged);
            colorMapChanged = false;
        } else {
            scene->setColorMapChanged(colorMapChanged);
        }
        if (alphaChanged) {
            scene->setAlphaChanged(alphaChanged);
            alphaChanged = false;
        } else {
            scene->setAlphaChanged(alphaChanged);
        }
        if (rgbChanged) {
            scene->setRGBChanged(rgbChanged);
            rgbChanged = false;
        } else {
            scene->setRGBChanged(rgbChanged);
        }
        if (sliceColorMapChanged) {
            scene->setSliceColorMapChanged(sliceColorMapChanged);
            sliceColorMapChanged = false;
        } else {
            scene->setSliceColorMapChanged(sliceColorMapChanged);
        }
        if (maximumPriorityQueueSizeChanged) {
            scene->setMaximumPriorityQueueSizeChanged(maximumPriorityQueueSizeChanged);
            maximumPriorityQueueSizeChanged = false;
        } else {
            scene->setMaximumPriorityQueueSizeChanged(maximumPriorityQueueSizeChanged);
        }
        if (animating) {
            animation->setCurrentNavigationTransformation();
        }
    }
    // This requestUpdate() is necessary for the animation and for some unknown reason must be outside of the if statement
    requestUpdate();
}

/*
 * getClippingPlanes
 *
 * return - ClippingPlanes *
 */
ClippingPlane * Toirt_Samhlaigh::getClippingPlanes(void) {
    return clippingPlanes;
} // end getClippingPlanes()

/*
 * getFocusAndContextPlanes
 *
 * return - FocusAndContextPlanes *
 */
FocusAndContextPlane * Toirt_Samhlaigh::getFocusAndContextPlanes(void) {
    return focusAndContextPlanes;
} // end getFocusAndContextPlanes()

/*
 * setDownSamplingChanged
 *
 * parameter downSamplingChanged - bool
 */
void Toirt_Samhlaigh::setDownSamplingChanged(bool downSamplingChanged) {
    this->downSamplingChanged = downSamplingChanged;
} // end setDownSamplingChanged()

/*
 * isInteractive
 *
 * return - bool
 */
bool Toirt_Samhlaigh::isInteractive(void) {
    return interactive;
} // end isInteractive()

/*
 * getNumberOfClippingPlanes
 *
 * return - int
 */
int Toirt_Samhlaigh::getNumberOfClippingPlanes(void) {
    return numberOfClippingPlanes;
} // end getNumberOfClippingPlanes()

/*
 * getNumberOfFocusAndContextPlanes
 *
 * return - int
 */
int Toirt_Samhlaigh::getNumberOfFocusAndContextPlanes(void) {
    return numberOfFocusAndContextPlanes;
} // end getNumberOfFocusAndContextPlanes()

/*
 * setSliceColorMapChanged
 *
 * parameter _sliceColorMapChanged - bool
 */
void Toirt_Samhlaigh::setSliceColorMapChanged(bool _sliceColorMapChanged) {
    sliceColorMapChanged = _sliceColorMapChanged;
} // end setSliceColorMapChanged()

/*
 * greenChangedCallback - Change the green.
 *
 * parameter callBackData - Misc::CallbackData*
 */
void Toirt_Samhlaigh::greenChangedCallback(Misc::CallbackData* callBackData) {
    if (scene->getPreintegrated()) {
        rgbTransferFunction1D->exportPreIntegrationColorMap(colormap);
    } else {
        rgbTransferFunction1D->exportGreen(colormap);
    }
    for (int i = 0; i < 256; i++) {
        green[i] = greenScale * float(colormap[4 * (i * 256 + i) + 1]) / 255.0f;
    }
    if (!interactive) {
        if (rgbTransferFunction1D->isDragging() && rgbTransferFunction1D->isInteractive()) {
            scene->setInteractive(true);
        } else {
            scene->setInteractive(false);
            scene->updateGreen(green);
            rgbChanged = true;
        }
    }
    scene->updateColorMap(colormap);
    colorMapChanged = true;
    Vrui::requestUpdate();
} // end greenChangedCallback()

/*
 * initialize - Initialize the GUI portion of the application.
 *
 * parameter volumeFile - const char*& volumeFile
 */
void Toirt_Samhlaigh::initialize(void) {
    if (volumeFile != 0) {
        IOHelper::readVolume(volumeFile, volume);
    } else if (numberOfComponents != 0) {
        if (typeOfComponents == 7) {
            IOHelper::readVolume(redVolumeFile, greenVolumeFile, blueVolumeFile, volume);
        } else if (typeOfComponents == 6) {
            IOHelper::readVolume(redVolumeFile, greenVolumeFile, volume, typeOfComponents);
        } else if (typeOfComponents == 5) {
            IOHelper::readVolume(redVolumeFile, blueVolumeFile, volume, typeOfComponents);
        } else if (typeOfComponents == 4) {
            IOHelper::readVolume(redVolumeFile, volume, typeOfComponents);
        } else if (typeOfComponents == 3) {
            IOHelper::readVolume(greenVolumeFile, blueVolumeFile, volume, typeOfComponents);
        } else if (typeOfComponents == 2) {
            IOHelper::readVolume(greenVolumeFile, volume, typeOfComponents);
        } else if (typeOfComponents == 1) {
            IOHelper::readVolume(blueVolumeFile, volume, typeOfComponents);
        }
    } else {
        IOHelper::readVolume("../data/C60.vol", volume);
    }
    if (volume->getNumberOfComponents() == 0) {
        alphaChanged = true;
        rgbChanged = false;
        transferFunction1D = new TransferFunction1D(*this, volume);
        transferFunction1D->createTransferFunction1D(CINVERSE_RAINBOW, UP_RAMP, 0.0, 1.0);
        transferFunction1D->getColorMapChangedCallbacks().add(this, &Toirt_Samhlaigh::colorMapChangedCallback);
        transferFunction1D->getAlphaChangedCallbacks().add(this, &Toirt_Samhlaigh::alphaChangedCallback);
        transferFunction1D->exportAlpha(colormap);
        transferFunction1D->exportColorMap(colormap);
        for (int i = 0; i < 256; i++) {
            alpha[i] = float(colormap[4 * (i * 256 + i) + 3]) / 255.0f;
        }
        volume->calculateGradient();
        transferFunction2_5D = new TransferFunction2_5D(volume, *this);
        transferFunction2_5D->getHistogramWidgetChangedCallbacks().add(this, &Toirt_Samhlaigh::colorMap2_5DChangedCallback);
    } else {
        alphaChanged = false;
        rgbChanged = true;
        rgbTransferFunction1D = new RGBTransferFunction1D(*this, volume);
        rgbTransferFunction1D->createRGBTransferFunction1D(UP_RAMP, UP_RAMP, UP_RAMP, 0.0, 1.0);
        rgbTransferFunction1D->getRedChangedCallbacks().add(this, &Toirt_Samhlaigh::redChangedCallback);
        rgbTransferFunction1D->getGreenChangedCallbacks().add(this, &Toirt_Samhlaigh::greenChangedCallback);
        rgbTransferFunction1D->getBlueChangedCallbacks().add(this, &Toirt_Samhlaigh::blueChangedCallback);
        rgbTransferFunction1D->exportRed(colormap);
        rgbTransferFunction1D->exportGreen(colormap);
        rgbTransferFunction1D->exportBlue(colormap);
        for (int i = 0; i < 256; i++) {
            red[i] = redScale * float(colormap[4 * (i * 256 + i) + 0]) / 255.0f;
            green[i] = greenScale * float(colormap[4 * (i * 256 + i) + 1]) / 255.0f;
            blue[i] = blueScale * float(colormap[4 * (i * 256 + i) + 2]) / 255.0f;
        }
    }
    scene = new Scene(volume, colormap, alpha, alpha2_5D, red, green, blue, sliceColormap);
    downSampling = new DownSampling(scene, *this, 4);
    float * extent = new float[3];
    extent[0] = volume->getExtent(0);
    extent[1] = volume->getExtent(1);
    extent[2] = volume->getExtent(2);
    float * origin = new float[3];
    origin[0] = volume->getOrigin(0);
    origin[1] = volume->getOrigin(1);
    origin[2] = volume->getOrigin(2);
    lighting = new Lighting(scene, *this, extent, origin);
    materials = new Materials(scene, *this);
    shading = new Shading(scene, *this);
    featureEnhancement = new FeatureEnhancement(scene, *this);
    slices = new Slices(scene, sliceColormap, *this, volume);
    slices->setSlicesColorMap(CINVERSE_RAINBOW, 0.0, 1.0);
    slices->exportSlicesColorMap(sliceColormap);
    scene->updateSliceColorMap(sliceColormap);
    numberOfClippingPlanes = 6;
    clippingPlanes = new ClippingPlane[numberOfClippingPlanes];
    for (int i = 0; i < numberOfClippingPlanes; ++i) {
        clippingPlanes[i].setAllocated(false);
        clippingPlanes[i].setActive(false);
    }
    numberOfFocusAndContextPlanes = 1;
    focusAndContextPlanes = new FocusAndContextPlane[numberOfFocusAndContextPlanes];
    for (int i = 0; i < numberOfFocusAndContextPlanes; ++i) {
        focusAndContextPlanes[i].setAllocated(false);
        focusAndContextPlanes[i].setActive(false);
    }
    mainMenu = createMainMenu();
    Vrui::setMainMenu(mainMenu);
    centerDisplayCallback(0);
    animation = new Animation(*this, scene);
    sliceFactorDialog = createSliceFactorDialog();
    maximumPriorityQueueSizeDialog = createMaximumPriorityQueueSizeDialog();
    ratioOfVisibilityThresholdDialog = createRatioOfVisibilityThresholdDialog();
    renderingCostFunctionDialog = createRenderingCostFunctionDialog();
}

/*
 * menuToggleSelectCallback - Adjust the program state based on which toggle button changed state.
 *
 * parameter callBackData - GLMotif::ToggleButton::ValueChangedCallbackData*
 */
void Toirt_Samhlaigh::menuToggleSelectCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData) {
    /* Adjust program state based on which toggle button changed state: */
    if (strcmp(callBackData->toggle->getName(), "ShowVolumeToggle") == 0) {
        showVolume = callBackData->set;
        scene->setShowVolume(showVolume);
    } else if (strcmp(callBackData->toggle->getName(), "InteractiveToggle") == 0) {
        interactive = callBackData->set;
        if (volume->getNumberOfComponents() == 0) {
            transferFunction1D->setInteractive(interactive);
            transferFunction2_5D->setInteractive(interactive);
        } else {
            rgbTransferFunction1D->setInteractive(interactive);
        }
        scene->setInteractive(interactive);
    } else if (strcmp(callBackData->toggle->getName(), "ShowOutlineToggle") == 0) {
        showOutline = callBackData->set;
        scene->setShowOutline(showOutline);
    } else if (strcmp(callBackData->toggle->getName(), "ShowBricksToggle") == 0) {
        showBricks = callBackData->set;
        scene->setShowBricks(showBricks);
    } else if (strcmp(callBackData->toggle->getName(), "ShowOcNodesToggle") == 0) {
        showOcNodes = callBackData->set;
        scene->setShowOcNodes(showOcNodes);
    } else if (strcmp(callBackData->toggle->getName(), "ShowSliceFactorDialogToggle") == 0) {
        if (callBackData->set) {
            /* Open the slice factor dialog at the same position as the main menu: */
            Vrui::getWidgetManager()->popupPrimaryWidget(sliceFactorDialog, Vrui::getWidgetManager()->calcWidgetTransformation(
                    mainMenu));
        } else {
            /* Close the slice factor dialog: */
            Vrui::popdownPrimaryWidget(sliceFactorDialog);
        }
    } else if (strcmp(callBackData->toggle->getName(), "ShowRatioOfVisibilityThresholdDialogToggle") == 0) {
        if (callBackData->set) {
            /* Open the ratio of visibility threshold dialog at the same position as the main menu: */
            Vrui::getWidgetManager()->popupPrimaryWidget(ratioOfVisibilityThresholdDialog,
                    Vrui::getWidgetManager()->calcWidgetTransformation(mainMenu));
        } else {
            /* Close the ratio of visibility threshold dialog: */
            Vrui::popdownPrimaryWidget(ratioOfVisibilityThresholdDialog);
        }
    } else if (strcmp(callBackData->toggle->getName(), "ShowRenderingCostFunctionDialogToggle") == 0) {
        if (callBackData->set) {
            /* Open the rendering cost function dialog at the same position as the main menu: */
            Vrui::getWidgetManager()->popupPrimaryWidget(renderingCostFunctionDialog,
                    Vrui::getWidgetManager()->calcWidgetTransformation(mainMenu));
        } else {
            /* Close the rendering cost function dialog: */
            Vrui::popdownPrimaryWidget(renderingCostFunctionDialog);
        }
    } else if (strcmp(callBackData->toggle->getName(), "RenderingCostFunctionTestToggle") == 0) {
        renderingCostFunctionTestToggle->setToggle(callBackData->set);
        renderingCostFunctionTest = callBackData->set;
        ratioOfVisibilityTest = !callBackData->set;
        maximumPriorityQueueTest = false;
        ratioTestToggle->setToggle(!callBackData->set);
        priorityQueueTestToggle->setToggle(false);
        scene->setRenderingCostFunctionTest(callBackData->set);
        scene->setRenderingCost(renderingCost);
        scene->setRatioOfVisibilityTest(!callBackData->set);
        scene->setMaximumPriorityQueueTest(false);
        alphaChanged = true;
    } else if (strcmp(callBackData->toggle->getName(), "RatioTestToggle") == 0) {
        ratioTestToggle->setToggle(callBackData->set);
        ratioOfVisibilityTest = callBackData->set;
        renderingCostFunctionTest = !callBackData->set;
        maximumPriorityQueueTest = false;
        priorityQueueTestToggle->setToggle(false);
        renderingCostFunctionTestToggle->setToggle(!callBackData->set);
        scene->setRatioOfVisibilityTest(callBackData->set);
        scene->setRatioOfVisibilityThreshold(ratioOfVisibilityThreshold);
        scene->setMaximumPriorityQueueTest(false);
        alphaChanged = true;
    } else if (strcmp(callBackData->toggle->getName(), "PriorityQueueTestToggle") == 0) {
        priorityQueueTestToggle->setToggle(callBackData->set);
        ratioTestToggle->setToggle(!callBackData->set);
        renderingCostFunctionTestToggle->setToggle(false);
        maximumPriorityQueueTest = callBackData->set;
        ratioOfVisibilityTest = !callBackData->set;
        renderingCostFunctionTest = false;
        scene->setMaximumPriorityQueueTest(callBackData->set);
        scene->setMaximumPriorityQueueSize(maximumPriorityQueueSize);
        scene->setRatioOfVisibilityTest(!callBackData->set);
        scene->setRenderingCostFunctionTest(false);
        alphaChanged = true;
    } else if (strcmp(callBackData->toggle->getName(), "ShowMaximumPriorityQueueSizeDialogToggle") == 0) {
        if (callBackData->set) {
            /* Open the ratio of visibility threshold dialog at the same position as the main menu: */
            Vrui::getWidgetManager()->popupPrimaryWidget(maximumPriorityQueueSizeDialog,
                    Vrui::getWidgetManager()->calcWidgetTransformation(mainMenu));
        } else {
            /* Close the ratio of visibility threshold dialog: */
            Vrui::popdownPrimaryWidget(maximumPriorityQueueSizeDialog);
        }
    } else if (strcmp(callBackData->toggle->getName(), "ShowAnimationDialogToggle") == 0) {
        if (callBackData->set) {
            /* Open the Animation dialog at the same position as the main menu: */
            Vrui::getWidgetManager()->popupPrimaryWidget(animation, Vrui::getWidgetManager()->calcWidgetTransformation(mainMenu));
            scene->setPerspective(false);
        } else {
            /* Close the Animation dialog */
            Vrui::popdownPrimaryWidget(animation);
        }
    } else if (strcmp(callBackData->toggle->getName(), "ShowRedToggle") == 0) {
        if (callBackData->set) {
            redScale = 1.0f;
            scene->setRedScale(redScale);
        } else {
            redScale = 0.0f;
            scene->setRedScale(redScale);
        }
    } else if (strcmp(callBackData->toggle->getName(), "ShowGreenToggle") == 0) {
        if (callBackData->set) {
            greenScale = 1.0f;
            scene->setGreenScale(greenScale);
        } else {
            greenScale = 0.0f;
            scene->setGreenScale(greenScale);
        }
    } else if (strcmp(callBackData->toggle->getName(), "ShowBlueToggle") == 0) {
        if (callBackData->set) {
            blueScale = 1.0f;
            scene->setBlueScale(blueScale);
        } else {
            blueScale = 0.0f;
            scene->setBlueScale(blueScale);
        }
    } else if (strcmp(callBackData->toggle->getName(), "ShowSlicesToggle") == 0) {
        if (callBackData->set) {
            /* Open the slices dialog at the same position as the main menu: */
            Vrui::getWidgetManager()->popupPrimaryWidget(slices, Vrui::getWidgetManager()->calcWidgetTransformation(mainMenu));
        } else {
            /* Close the slices dialog: */
            Vrui::popdownPrimaryWidget(slices);
        }
    } else if (strcmp(callBackData->toggle->getName(), "ShowShadingToggle") == 0) {
        if (callBackData->set) {
            /* Open the shading dialog at the same position as the main menu: */
            Vrui::getWidgetManager()->popupPrimaryWidget(shading, Vrui::getWidgetManager()->calcWidgetTransformation(mainMenu));
        } else {
            /* Close the shading dialog: */
            Vrui::popdownPrimaryWidget(shading);
        }
    } else if (strcmp(callBackData->toggle->getName(), "ShowFeatureEnhancementToggle") == 0) {
        if (callBackData->set) {
            /* Open the feature enhancement dialog at the same position as the main menu: */
            Vrui::getWidgetManager()->popupPrimaryWidget(featureEnhancement, Vrui::getWidgetManager()->calcWidgetTransformation(
                    mainMenu));
        } else {
            /* Close the feature enhancement dialog: */
            Vrui::popdownPrimaryWidget(featureEnhancement);
        }
    } else if (strcmp(callBackData->toggle->getName(), "ShowLightingToggle") == 0) {
        if (callBackData->set) {
            /* Open the lighting dialog at the same position as the main menu: */
            Vrui::getWidgetManager()->popupPrimaryWidget(lighting, Vrui::getWidgetManager()->calcWidgetTransformation(mainMenu));
        } else {
            /* Close the lighting dialog: */
            Vrui::popdownPrimaryWidget(lighting);
        }
    } else if (strcmp(callBackData->toggle->getName(), "ShowMaterialsToggle") == 0) {
        if (callBackData->set) {
            /* Open the materials dialog at the same position as the main menu: */
            Vrui::getWidgetManager()->popupPrimaryWidget(materials, Vrui::getWidgetManager()->calcWidgetTransformation(mainMenu));
        } else {
            /* Close the materials dialog: */
            Vrui::popdownPrimaryWidget(materials);
        }
    } else if (strcmp(callBackData->toggle->getName(), "DownSamplingToggle") == 0) {
        if (callBackData->set) {
            /* Open the down sampling dialog at the same position as the main menu: */
            Vrui::getWidgetManager()->popupPrimaryWidget(downSampling, Vrui::getWidgetManager()->calcWidgetTransformation(mainMenu));
        } else {
            /* Close the down sampling dialog: */
            Vrui::popdownPrimaryWidget(downSampling);
        }
    }
    Vrui::requestUpdate();
}

/*
 * processCommandLineArguments - Process the command line arguments.
 *
 * parameter argc - int&
 * parameter argv - char**&
 * parameter volume - Volume*
 * return - const char* (Volume File Name)
 */
void Toirt_Samhlaigh::processCommandLineArguments(int& argc, char**& argv) throw (std::runtime_error) {
    const char* leafSizeString = 0;
    const char* ratioOfVisibilityThresholdString = 0;
    const char* maximumPriorityQueueSizeString = 0;
    const char* scaleString = 0;
    const char* sizeString = 0;
    const char* sliceFactorString = 0;
    const char* textureSizeString = 0;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            if (strcasecmp(argv[i] + 1, "scale") == 0) {
                ++i;
                scaleString = argv[i];
                StringTokenizer stringTokenizer(scaleString, "x");
                Scalar scale[3];
                if (stringTokenizer.getNumberOfTokens() == 3) {
                    scale[1] = stringTokenizer.nextFloatToken();
                    scale[2] = stringTokenizer.nextFloatToken();
                    scale[0] = stringTokenizer.nextFloatToken();
                    volume->setScale(scale);
                } else
                    Misc::throwStdErr("Toirt_Samhlaigh::parseCommandLineArguments: Invalid volume scale command-line syntax");
            } else if (strcasecmp(argv[i] + 1, "size") == 0) {
                ++i;
                sizeString = argv[i];
                StringTokenizer stringTokenizer(sizeString, "x");
                int size[3];
                if (stringTokenizer.getNumberOfTokens() == 3) {
                    size[1] = stringTokenizer.nextIntToken();
                    size[2] = stringTokenizer.nextIntToken();
                    size[0] = stringTokenizer.nextIntToken();
                    volume->setSize(size);
                } else
                    Misc::throwStdErr("Toirt_Samhlaigh::parseCommandLineArguments: Invalid volume size command-line syntax");
            } else if (strcasecmp(argv[i] + 1, "textureSize") == 0) {
                ++i;
                textureSizeString = argv[i];
                int _textureSize = Stringify::toInt(textureSizeString);
                if (_textureSize != 0) {
                    volume->setTextureSize(_textureSize);
                } else
                    Misc::throwStdErr("Toirt_Samhlaigh::parseCommandLineArguments: Unrecognized texture size command-line argument");
            } else if (strcasecmp(argv[i] + 1, "leafSize") == 0) {
                ++i;
                leafSizeString = argv[i];
                int _leafSize = Stringify::toInt(leafSizeString);
                if (_leafSize != 0) {
                    volume->setLeafSize(_leafSize);
                } else
                    Misc::throwStdErr("Toirt_Samhlaigh::parseCommandLineArguments: Unrecognized leaf size command-line argument");
            } else if (strcasecmp(argv[i] + 1, "file") == 0) {
                ++i;
                volumeFile = argv[i];
            } else if (strcasecmp(argv[i] + 1, "redFile") == 0) {
                ++i;
                redVolumeFile = argv[i];
                numberOfComponents++;
                typeOfComponents += 4;
            } else if (strcasecmp(argv[i] + 1, "greenFile") == 0) {
                ++i;
                greenVolumeFile = argv[i];
                numberOfComponents++;
                typeOfComponents += 2;
            } else if (strcasecmp(argv[i] + 1, "blueFile") == 0) {
                ++i;
                blueVolumeFile = argv[i];
                numberOfComponents++;
                typeOfComponents++;
            } else if (strcasecmp(argv[i] + 1, "ratioOfVisibilityThreshold") == 0) {
                ++i;
                ratioOfVisibilityThresholdString = argv[i];
                ratioOfVisibilityThreshold = Stringify::toFloat(ratioOfVisibilityThresholdString);
                if (ratioOfVisibilityThreshold != 0) {
                    volume->setRatioOfVisibilityThreshold(ratioOfVisibilityThreshold);
                } else
                    Misc::throwStdErr(
                            "Toirt_Samhlaigh::parseCommandLineArguments: Unrecognized ratio of visibility threshold command-line argument");
            } else if (strcasecmp(argv[i] + 1, "maximumPriorityQueueSize") == 0) {
                ++i;
                maximumPriorityQueueSizeString = argv[i];
                maximumPriorityQueueSize = Stringify::toInt(maximumPriorityQueueSizeString);
                if (maximumPriorityQueueSize >= 0) {
                    volume->setMaximumPriorityQueueSize(maximumPriorityQueueSize);
                } else
                    Misc::throwStdErr(
                            "Toirt_Samhlaigh::parseCommandLineArguments: Unrecognized maximum priority queue size command-line argument");
            } else if (strcasecmp(argv[i] + 1, "sliceFactor") == 0) {
                ++i;
                sliceFactorString = argv[i];
                float _sliceFactor = Stringify::toFloat(sliceFactorString);
                if (_sliceFactor != 0) {
                    volume->setSliceFactor(_sliceFactor);
                } else
                    Misc::throwStdErr("Toirt_Samhlaigh::parseCommandLineArguments: Unrecognized slice factor command-line argument");
            } else if(strcasecmp(argv[i] + 1,"creditFile")==0) {
            	++i;
            	readCreditFile(argv[i]);
			} else
                Misc::throwStdErr("Toirt_Samhlaigh::parseCommandLineArguments: Unrecognized command-line argument");
        } else {
            Misc::throwStdErr("Toirt_Samhlaigh::parseCommandLineArguments: Unrecognized command-line argument");
        }
    }
}

/*
 * readCreditFile - Read the credit file
 *
 * parameter creditFileName - const char*
 */
void Toirt_Samhlaigh::readCreditFile(const char* creditFileName)
	{
	try
		{
		std::string str(creditFileName);
		/* Open credit file: */
		Misc::ConfigurationFile creditFile((str).c_str());
		Misc::ConfigurationFileSection credit=creditFile.getSection("/Credit");
		/* Override program settings from credit file: */
		creditTitle=credit.retrieveValue<std::string>("./title","title");
		creditData=credit.retrieveValue<std::string>("./data","data authors");
		creditGraphics=credit.retrieveValue<std::string>("./graphics","graphics authors");
		creditInformation=true;
		}
	catch(std::runtime_error err)
		{
		std::cerr<<"Caught exception "<<err.what()<<" in ShowEarthModel::readCreditFile"<<std::endl;
		}
	} // end readCreditFile()

/*
 * redChangedCallback - Change the red.
 *
 * parameter callBackData - Misc::CallbackData*
 */
void Toirt_Samhlaigh::redChangedCallback(Misc::CallbackData* callBackData) {
    if (scene->getPreintegrated()) {
        rgbTransferFunction1D->exportPreIntegrationColorMap(colormap);
    } else {
        rgbTransferFunction1D->exportRed(colormap);
    }
    for (int i = 0; i < 256; i++) {
        red[i] = redScale * float(colormap[4 * (i * 256 + i) + 0]) / 255.0f;
    }
    if (!interactive) {
        if (rgbTransferFunction1D->isDragging() && rgbTransferFunction1D->isInteractive()) {
            scene->setInteractive(true);
        } else {
            scene->setInteractive(false);
            scene->updateRed(red);
            rgbChanged = true;
        }
    }
    scene->updateColorMap(colormap);
    colorMapChanged = true;
    Vrui::requestUpdate();
} // end redChangedCallback()

/*
 * set1D
 */
void Toirt_Samhlaigh::set1D(void) {
    for (int i = 0; i < 256 * 256 * 4; i++) {
        colormap[i] = 0;
    }
    if (volume->getNumberOfComponents() == 0) {
        if (scene->getPreintegrated()) {
            transferFunction1D->exportPreIntegrationColorMap(colormap);
        } else {
            transferFunction1D->exportAlpha(colormap);
            transferFunction1D->exportColorMap(colormap);
        }
        for (int i = 0; i < 256; i++) {
            alpha[i] = float(colormap[4 * (i * 256 + i) + 3]) / 255.0f;
        }
        scene->updateAlpha(alpha);
        alphaChanged = true;
    } else {
        rgbTransferFunction1D->exportBlue(colormap);
        rgbTransferFunction1D->exportGreen(colormap);
        rgbTransferFunction1D->exportRed(colormap);
        for (int i = 0; i < 255; i++) {
            red[i] = redScale * float(colormap[4 * (i * 256 + i) + 0]) / 255.0f;
            green[i] = greenScale * float(colormap[4 * (i * 256 + i) + 1]) / 255.0f;
            blue[i] = blueScale * float(colormap[4 * (i * 256 + i) + 2]) / 255.0f;
        }
        scene->updateRed(red);
        scene->updateGreen(green);
        scene->updateBlue(blue);
        rgbChanged = true;
    }
    scene->updateColorMap(colormap);
    colorMapChanged = true;
    scene->setPreintegrated(false);
    scene->setDimension(1);
} // end set1D()

/*
 * set2D
 */
void Toirt_Samhlaigh::set2D(void) {
    for (int i = 0; i < 256 * 256 * 4; i++) {
        colormap[i] = 0;
    }
    transferFunction2_5D->exportColorMap(colormap);
    for (int i = 0; i < 256 * 256; i++) {
        alpha2_5D[i] = colormap[4 * i + 3];
    }
    scene->updateAlpha2_5D(alpha2_5D);
    alphaChanged = true;
    scene->updateColorMap(colormap);
    colorMapChanged = true;
    scene->setPreintegrated(false);
    scene->setDimension(2);
} // end set2D()

/*
 * setAnimating
 *
 * parameter _animating - bool
 */
void Toirt_Samhlaigh::setAnimating(bool _animating) {
    animating = _animating;
} // end setAnimating()

/*
 * setPreintegrationColormap
 */
void Toirt_Samhlaigh::setPreintegrationColormap(void) {
    for (int i = 0; i < 256 * 256 * 4; i++) {
        colormap[i] = 0;
    }
    if (volume->getNumberOfComponents() == 0) {
        transferFunction1D->exportPreIntegrationColorMap(colormap);
    } else {
        rgbTransferFunction1D->exportPreIntegrationColorMap(colormap);
    }
    for (int i = 0; i < 256; i++) {
        alpha[i] = float(colormap[4 * (i * 256 + i) + 3]) / 255.0f;
    }
    scene->updateAlpha(alpha);
    alphaChanged = true;
    scene->updateColorMap(colormap);
    colorMapChanged = true;
} // end setPreintegrationColormap()

/*
 * setTransferFunction2_5DChanged
 *
 * parameter transferFunction2_5DChanged - bool
 */
void Toirt_Samhlaigh::setTransferFunction2_5DChanged(bool transferFunction2_5DChanged) {
    this->transferFunction2_5DChanged = transferFunction2_5DChanged;
} // end setTransferFunction2_5DChanged()

/*
 * showTransferFunction1DCallback - Display or hide the 1D transfer function editor for the color map.
 *
 * parameter callBackData - GLMotif::ToggleButton::ValueChangedCallbackData*
 */
void Toirt_Samhlaigh::showTransferFunction1DCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData) {
    if (callBackData->set) {
        if (volume->getNumberOfComponents() == 0) {
            Vrui::popdownPrimaryWidget(transferFunction2_5D);
            showTransferFunction2_5DToggle->setToggle(false);
            Vrui::getWidgetManager()->popupPrimaryWidget(transferFunction1D, Vrui::getWidgetManager()->calcWidgetTransformation(
                    mainMenu));
            set1D();
        } else {
            Vrui::getWidgetManager()->popupPrimaryWidget(rgbTransferFunction1D, Vrui::getWidgetManager()->calcWidgetTransformation(
                    mainMenu));
        }
    } else {
        if (volume->getNumberOfComponents() == 0) {
            Vrui::popdownPrimaryWidget(transferFunction1D);
        } else {
            Vrui::popdownPrimaryWidget(rgbTransferFunction1D);
        }
    }
} // end showTransferFunction1DCallback()

/*
 * showTransferFunction2_5DCallback - Display or hide the 2.5D transfer function editor for the color map.
 *
 * parameter callBackData - GLMotif::ToggleButton::ValueChangedCallbackData*
 */
void Toirt_Samhlaigh::showTransferFunction2_5DCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData) {
    if (callBackData->set) {
        Vrui::popdownPrimaryWidget(transferFunction1D);
        showTransferFunction1DToggle->setToggle(false);
        Vrui::getWidgetManager()->popupPrimaryWidget(transferFunction2_5D, Vrui::getWidgetManager()->calcWidgetTransformation(
                mainMenu));
        set2D();
    } else {
        Vrui::popdownPrimaryWidget(transferFunction2_5D);
    }
} // end showTransferFunction2_5DCallback()

/*
 * sliderCallback
 *
 * parameter callBackData - GLMotif::Slider::ValueChangedCallbackData*
 */
void Toirt_Samhlaigh::sliderCallback(GLMotif::Slider::ValueChangedCallbackData* callBackData) {
    if (strcmp(callBackData->slider->getName(), "SliceFactorSlider") == 0) {
        sliceFactorValue->setValue(callBackData->value);
        scene->setSliceFactor(callBackData->value);
        Vrui::requestUpdate();
    }
    if (strcmp(callBackData->slider->getName(), "RatioOfVisibilityThresholdSlider") == 0) {
        ratioOfVisibilityThreshold = callBackData->value;
        ratioOfVisibilityThresholdValue->setValue(ratioOfVisibilityThreshold);
        if (ratioOfVisibilityTest) {
            scene->setRatioOfVisibilityThreshold(ratioOfVisibilityThreshold);
        }
        Vrui::requestUpdate();
    }
    if (strcmp(callBackData->slider->getName(), "RenderingCostSlider") == 0) {
        renderingCost = callBackData->value;
        renderingCostValue->setValue(renderingCost);
        if (renderingCostFunctionTest) {
            scene->setRenderingCost(renderingCost);
            alphaChanged = true;
        }
        Vrui::requestUpdate();
    }
    if (strcmp(callBackData->slider->getName(), "MaximumPriorityQueueSizeSlider") == 0) {
        maximumPriorityQueueSize = int(callBackData->value);
        maximumPriorityQueueSizeValue->setValue(maximumPriorityQueueSize);
        if (!ratioOfVisibilityTest) {
            scene->setMaximumPriorityQueueSize(int(maximumPriorityQueueSize));
            maximumPriorityQueueSizeChanged = true;
        }
        Vrui::requestUpdate();
    }
} // end sliderCallback()

/*
 * toolCreationCallback
 *
 * parameter callbackData - Vrui::ToolManager::ToolCreationCallbackData *
 */
void Toirt_Samhlaigh::toolCreationCallback(Vrui::ToolManager::ToolCreationCallbackData * callbackData) {
    /* Check if the new tool is a locator tool: */
    Vrui::LocatorTool* locatorTool = dynamic_cast<Vrui::LocatorTool*> (callbackData->tool);
    if (locatorTool != 0) {
        BaseLocator* newLocator;
        if (analysisTool == 0) {
            /* Create a clipping plane locator object and associate it with the new tool: */
            newLocator = new ClippingPlaneLocator(locatorTool, this);
        }
        if (analysisTool == 1) {
            /* Create a focus and context  locator object and associate it with the new tool: */
            newLocator = new FocusAndContextPlaneLocator(locatorTool, this);
        }
        if (analysisTool == 2) {
            /* Create a Measurement Tool locator object and associate it with the new tool: */
            newLocator = new MeasurementLocator(locatorTool, this);
        }
        if (analysisTool == 2) {
            /* Create a Perspective Tool locator object and associate it with the new tool: */
            newLocator = new PerspectiveTool(locatorTool, this);
        }

        /* Add new locator to list: */
        baseLocators.push_back(newLocator);
    }
} // end toolCreationCallback()

/*
 * toolDestructionCallback
 *
 * parameter callbackData - Vrui::ToolManager::ToolDestructionCallbackData *
 */
void Toirt_Samhlaigh::toolDestructionCallback(Vrui::ToolManager::ToolDestructionCallbackData * callbackData) {
    /* Check if the to-be-destroyed tool is a locator tool: */
    Vrui::LocatorTool* locatorTool = dynamic_cast<Vrui::LocatorTool*> (callbackData->tool);
    if (locatorTool != 0) {
        /* Find the data locator associated with the tool in the list: */
        for (BaseLocatorList::iterator blIt = baseLocators.begin(); blIt != baseLocators.end(); ++blIt) {

            if ((*blIt)->getTool() == locatorTool) {
                FocusAndContextPlaneLocator* focusAndContextPlaneLocator = dynamic_cast<FocusAndContextPlaneLocator*> (*blIt);
                if (focusAndContextPlaneLocator) {
                    // Clean up
                }
                PerspectiveTool* perspectiveTool = dynamic_cast<PerspectiveTool*> (*blIt);
                if (perspectiveTool) {
                    scene->clearSpheres();
                    scene->setPerspective(false);
                }
                /* Remove the locator: */
                delete *blIt;
                baseLocators.erase(blIt);
                break;
            }
        }
    }
} // end toolDestructionCallback()

/*
 * main - The application main method.
 *
 * parameter argc - int
 * parameter argv - char**
 */
int main(int argc, char* argv[]) {
    try {
        char** applicationDefaults = 0;
        Toirt_Samhlaigh application(argc, argv, applicationDefaults);
        application.run();
        return 0;
    } catch (std::runtime_error e) {
        std::cerr << "Error: Exception " << e.what() << "!" << std::endl;
        return 1;
    }
}
