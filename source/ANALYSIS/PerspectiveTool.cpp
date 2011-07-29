#include <GL/gl.h>

/* Vrui includes */
#include <GLMotif/PopupWindow.h>
#include <GLMotif/Label.h>
#include <GLMotif/TextField.h>
#include <GLMotif/RowColumn.h>
#include <GLMotif/WidgetManager.h>
#include <GLMotif/Blind.h>


/* Vrui includes to use the Vrui interface */
#include <Vrui/Vrui.h>

#include <ANALYSIS/PerspectiveTool.h>
#include <UTILITY/Stringify.h>
#include <Toirt_Samhlaigh.h>

PerspectiveTool::PerspectiveTool(Vrui::LocatorTool* _locatorTool, Toirt_Samhlaigh* _application) :
	BaseLocator(_locatorTool, _application), drawsphere(true) {
	perspectiveDialogPopup = new GLMotif::PopupWindow("PerspectiveDialogPopup",Vrui::getWidgetManager(),"Perspective Tool");
	GLMotif::RowColumn* rowbox1 = new GLMotif::RowColumn("rowbox1",perspectiveDialogPopup,false);
	rowbox1->setOrientation(GLMotif::RowColumn::VERTICAL);
	GLMotif::RowColumn* dialog1 = new GLMotif::RowColumn("Dialog1",rowbox1,false);
	dialog1->setOrientation(GLMotif::RowColumn::HORIZONTAL);
	ClearButton = new GLMotif::Button("ClearButton",dialog1,"Clear");
	ClearButton->getSelectCallbacks().add(this, &PerspectiveTool::ClearCallback);
	GoToButton = new GLMotif::Button("GoToButton",dialog1,"Go To Current Marker");
	GoToButton->getSelectCallbacks().add(this, &PerspectiveTool::GoToCallback);
	new GLMotif::Blind("Filler",dialog1);
	new GLMotif::Blind("Filler",dialog1);
	new GLMotif::Blind("Filler",dialog1);
	new GLMotif::Blind("Filler",dialog1);
	new GLMotif::Blind("Filler",dialog1);
	new GLMotif::Blind("Filler",dialog1);
	new GLMotif::Blind("Filler",dialog1);
	new GLMotif::Blind("Filler",dialog1);
	GLMotif::RowColumn* labelbox1 = new GLMotif::RowColumn("labelbox1",rowbox1,false);
	labelbox1->setOrientation(GLMotif::RowColumn::HORIZONTAL);
	InterpLabel = new GLMotif::Label("InterpLabel",labelbox1," Marker ");
	InterpLabel2 = new GLMotif::Label("InterpLabel2",labelbox1,"  X ");
	InterpLabel4 = new GLMotif::Label("InterpLabel4",labelbox1,"           Y ");
	InterpLabel5 = new GLMotif::Label("InterpLabel5",labelbox1,"           Z ");
	new GLMotif::Blind("Filler",labelbox1);
	GLMotif::RowColumn* sliderbox1 = new GLMotif::RowColumn("sliderbox",rowbox1,false);
	sliderbox1->setOrientation(GLMotif::RowColumn::HORIZONTAL);
	FrameLabel = new GLMotif::TextField("FrameLabel",sliderbox1,6);
	FrameLabel->setFieldWidth(6);
	FrameLabel->setPrecision(1);
	Xpos = new GLMotif::TextField("Xpos",sliderbox1,6);
	Xpos->setFieldWidth(6);
	Xpos->setPrecision(4);
	Ypos = new GLMotif::TextField("Ypos",sliderbox1,6);
	Ypos->setFieldWidth(6);
	Ypos->setPrecision(4);
	Zpos = new GLMotif::TextField("Zpos",sliderbox1,6);
	Zpos->setFieldWidth(6);
	Zpos->setPrecision(4);
	SphereToggle = new GLMotif::ToggleButton("AnimationToggle",sliderbox1,"Show Spheres");
	SphereToggle->setToggle(true);
	SphereToggle->getValueChangedCallbacks().add(this, &PerspectiveTool::SpheresToggleCallback);
	GLMotif::RowColumn* labelbox21 = new GLMotif::RowColumn("labelbox21",rowbox1,false);
	labelbox21->setOrientation(GLMotif::RowColumn::HORIZONTAL);
	InterpLabel11 = new GLMotif::Label("InterpLabel11",labelbox21," Scale ");
	InterpLabel21 = new GLMotif::Label("InterpLabel21",labelbox21,"   X Angle");
	InterpLabel41 = new GLMotif::Label("InterpLabel41",labelbox21,"   Y Angle");
	InterpLabel51 = new GLMotif::Label("InterpLabel51",labelbox21,"   Z Angle");
	new GLMotif::Blind("Filler",labelbox21);
	GLMotif::RowColumn* rotationbox1 = new GLMotif::RowColumn("rotationbox1",rowbox1,false);
	rotationbox1->setOrientation(GLMotif::RowColumn::HORIZONTAL);
	scalepos = new GLMotif::TextField("scalepos",rotationbox1,6);
	scalepos->setFieldWidth(6);
	scalepos->setPrecision(3);
	xangle = new GLMotif::TextField("apos",rotationbox1,6);
	xangle->setFieldWidth(6);
	xangle->setPrecision(3);
	yangle = new GLMotif::TextField("bpos",rotationbox1,6);
	yangle->setFieldWidth(6);
	yangle->setPrecision(3);
	zangle = new GLMotif::TextField("cpos",rotationbox1,6);
	zangle->setFieldWidth(6);
	zangle->setPrecision(3);
	framesSubCascade = new GLMotif::CascadeButton("FramesSubCascade",rotationbox1,"Markers");
	framesSubCascade->setPopup(createFramesSubMenu());



	rowbox1->manageChild();
	dialog1->manageChild();
	labelbox1->manageChild();
	sliderbox1->manageChild();
	labelbox21->manageChild();
	rotationbox1->manageChild();

	Vrui::popupPrimaryWidget(perspectiveDialogPopup, Vrui::getNavigationTransformation().transform(Vrui::getDisplayCenter()));
}

PerspectiveTool::~PerspectiveTool(void) {
	Vrui::popdownPrimaryWidget(perspectiveDialogPopup);
	delete perspectiveDialogPopup;
}

GLMotif::Popup* PerspectiveTool::createFramesSubMenu(void) {

	GLMotif::Popup* framesSubMenuPopup = new GLMotif::Popup("FramesSubMenuPopup",Vrui::getWidgetManager());
		GLMotif::SubMenu* framesSubMenu = new GLMotif::SubMenu("FramesSubMenu",framesSubMenuPopup,false);
		if (!Frames.empty()) {
			FrameButtons.clear();
		for (int i=0; i<Frames.size(); i++) {
			FrameButtons.push_back(new GLMotif::ToggleButton(Stringify::toString(i).c_str(),framesSubMenu,Stringify::toString(i).c_str()));
			FrameButtons[i]->getValueChangedCallbacks().add(this, &PerspectiveTool::FrameButtonsCallback);}
		}
		else {
			new GLMotif::Label("Test",framesSubMenu,"No Markers");
		}
		framesSubMenu->manageChild();

		return framesSubMenuPopup;
}

void PerspectiveTool::FrameButtonsCallback(
		GLMotif::ToggleButton::ValueChangedCallbackData* callBackData) {
		CurrentFrame=Stringify::toInt(callBackData->toggle->getName());
		for (int i=0; i<FrameButtons.size(); i++){
			if (i!=CurrentFrame) {
				FrameButtons[i]->setToggle(false);
			}
		}
		FrameStatsUpdate();
}

void PerspectiveTool::SpheresToggleCallback(
		GLMotif::ToggleButton::ValueChangedCallbackData* callBackData) {
		drawsphere=!drawsphere;
}

void PerspectiveTool::FrameStatsUpdate(void) {
	Vector trans= Frames[CurrentFrame].getTranslation();
	const double* q;
	q=Frames[CurrentFrame].getRotation().getQuaternion();
	Xpos->setValue(trans[0]);
	Ypos->setValue(trans[1]);
	Zpos->setValue(trans[2]);
	zangle->setValue(asin(2*q[0]*q[1] + 2*q[2]*q[3]));
	yangle->setValue(atan2(2*q[1]*q[3]-2*q[0]*q[2] , 1 - 2*q[1]*q[1] - 2*q[2]*q[2]));
	xangle->setValue(atan2(2*q[0]*q[3]-2*q[1]*q[2] , 1 - 2*q[0]*q[0] - 2*q[2]*q[2]));
	scalepos->setValue(Frames[CurrentFrame].getScaling());
	FrameLabel->setValue(CurrentFrame);
	requestUpdate();
}

void PerspectiveTool::GoToCallback(Misc::CallbackData* _callbackData) {
	 setNavigationTransformation(Frames[CurrentFrame]);
	 Vector error= getNavigationTransformation().transform(Vrui::getHeadPosition())-Geometry::invert(Frames[CurrentFrame]).getOrigin();
	 setNavigationTransformation(getNavigationTransformation().translate(error));
}

void PerspectiveTool::ClearCallback(Misc::CallbackData* _callbackData) {
	Frames.clear();
	Xpos->setLabel("");
	Ypos->setLabel("");
	Zpos->setLabel("");
	xangle->setLabel("");
	yangle->setLabel("");
	zangle->setLabel("");
	scalepos->setLabel("");
	FrameLabel->setLabel("");
	FrameButtons.clear();
	framesSubCascade->setPopup(createFramesSubMenu());

}


void PerspectiveTool::buttonPressCallback(Vrui::LocatorTool::ButtonPressCallbackData* callbackData) {
		Frames.push_back(Geometry::invert(callbackData->currentTransformation));
		CurrentFrame=Frames.size()-1;
		framesSubCascade->setPopup(createFramesSubMenu());
		FrameStatsUpdate();
}

