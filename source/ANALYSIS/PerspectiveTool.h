
// This locator tool allows the user to place a marker with the wand and then shift perspective to the location of the marker.
// Peter McKee 8/4/2008

#ifndef PERSPECTIVETOOL_H_
#define PERSPECTIVETOOL_H_

/* Vrui includes */

#include <Vrui/Geometry.h>
#include <deque>
#include <GLMotif/ToggleButton.h>
#include <GLMotif/Button.h>
#include <GLMotif/Popup.h>
#include <GLMotif/CascadeButton.h>
#include <Geometry/OrthogonalTransformation.h>

#include <ANALYSIS/BaseLocator.h>

using namespace Vrui;
#include <GL/gl.h>

/* Vrui includes */
#include <GLMotif/Menu.h>
#include <GLMotif/Popup.h>
#include <GLMotif/PopupMenu.h>
#include <GLMotif/PopupWindow.h>
#include <GLMotif/RadioBox.h>
#include <GLMotif/SubMenu.h>
#include <GLMotif/TextField.h>
#include <GLMotif/ToggleButton.h>
#include <GLMotif/CascadeButton.h>
#include <Misc/CallbackData.h>
#include <GL/GLTransformationWrappers.h>


// begin Forward Declarations
class Toirt_Samhlaigh;
// end Forward Declarations

class PerspectiveTool : public BaseLocator {
public:
	PerspectiveTool(Vrui::LocatorTool* _locatorTool, Toirt_Samhlaigh* _application);
	~PerspectiveTool(void);
	virtual void buttonPressCallback(Vrui::LocatorTool::ButtonPressCallbackData* cbData);
	void GoToCallback(Misc::CallbackData* _callbackData);
	void ClearCallback(Misc::CallbackData* _callbackData);
	void FrameStatsUpdate(void);
	void FrameButtonsCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
	void SpheresToggleCallback(GLMotif::ToggleButton::ValueChangedCallbackData* callBackData);
	std::deque<NavTransform> Frames;
	bool drawsphere;
private:
	GLMotif::ToggleButton* SphereToggle;
	GLMotif::Popup* createFramesSubMenu(void);
	GLMotif::CascadeButton* framesSubCascade;
	std::deque<GLMotif::ToggleButton*> FrameButtons;
	GLMotif::PopupWindow* perspectiveDialogPopup;
	GLMotif::Button* ClearButton;
	GLMotif::Button* GoToButton;
	GLMotif::Label* InterpLabel;
	GLMotif::Label* InterpLabel2;
	GLMotif::Label* InterpLabel4;
	GLMotif::Label* InterpLabel5;
	GLMotif::Label* InterpLabel11;
	GLMotif::Label* InterpLabel21;
	GLMotif::Label* InterpLabel51;
	GLMotif::Label* InterpLabel41;
	GLMotif::TextField* FrameLabel;
	GLMotif::TextField* scalepos;
	GLMotif::TextField* Xpos;
	GLMotif::TextField* Ypos;
	GLMotif::TextField* Zpos;
	GLMotif::TextField* xangle;
	GLMotif::TextField* yangle;
	GLMotif::TextField* zangle;
	int CurrentFrame;
};


#endif /*PERSPECTIVETOOL_H_*/
