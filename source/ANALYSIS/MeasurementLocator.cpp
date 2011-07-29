
#include <GL/gl.h>

/* Vrui includes */
#include <GLMotif/PopupWindow.h>
#include <GLMotif/Label.h>
#include <GLMotif/TextField.h>
#include <GLMotif/RowColumn.h>
#include <GLMotif/WidgetManager.h>

/* Vrui includes to use the Vrui interface */
#include <Vrui/Vrui.h>

#include <ANALYSIS/MeasurementLocator.h>
#include <Toirt_Samhlaigh.h>

MeasurementLocator::MeasurementLocator(Vrui::LocatorTool* _locatorTool, Toirt_Samhlaigh* _application) :
	BaseLocator(_locatorTool, _application) {
	numberOfPoints = 0;
	dragging = false;
	measurementDialogPopup=new GLMotif::PopupWindow("MeasurementDialogPopup",Vrui::getWidgetManager(),"Measurement Dialog");
	GLMotif::RowColumn* measurementDialog=new GLMotif::RowColumn("MeasurementDialog",measurementDialogPopup,false);
	measurementDialog->setNumMinorWidgets(2);
	GLMotif::Label* startPositionLabel=new GLMotif::Label("StartPositionLabel",measurementDialog,"Start");
	GLMotif::RowColumn* startPositionBox=new GLMotif::RowColumn("StartPositionBox",measurementDialog,false);
	startPositionBox->setOrientation(GLMotif::RowColumn::HORIZONTAL);
	startPositionBox->setPacking(GLMotif::RowColumn::PACK_GRID);
	for (int i=0; i<3; ++i) {
		char labelName[40];
		snprintf(labelName, sizeof(labelName), "SartPosition-%d", i+1);
		startPosition[i]=new GLMotif::TextField(labelName,startPositionBox,8);
		startPosition[i]->setFieldWidth(8);
		startPosition[i]->setPrecision(4);
	}
	startPositionBox->manageChild();
	GLMotif::Label* endPositionLabel=new GLMotif::Label("EndPositionLabel",measurementDialog,"End");
	GLMotif::RowColumn* endPositionBox=new GLMotif::RowColumn("EndPositionBox",measurementDialog,false);
	endPositionBox->setOrientation(GLMotif::RowColumn::HORIZONTAL);
	endPositionBox->setPacking(GLMotif::RowColumn::PACK_GRID);
	for (int i=0; i<3; ++i) {
		char labelName[40];
		snprintf(labelName, sizeof(labelName), "EndPosition-%d", i+1);
		endPosition[i]=new GLMotif::TextField(labelName,endPositionBox,8);
		endPosition[i]->setFieldWidth(8);
		endPosition[i]->setPrecision(4);
	}
	endPositionBox->manageChild();
	GLMotif::Label* distanceLabel=new GLMotif::Label("DistLabel",measurementDialog,"Distance");
	distance=new GLMotif::TextField("Dist",measurementDialog,8);
	distance->setFieldWidth(8);
	distance->setPrecision(4);
	measurementDialog->manageChild();
	Vrui::popupPrimaryWidget(measurementDialogPopup, Vrui::getNavigationTransformation().transform(Vrui::getDisplayCenter()));
}

MeasurementLocator::~MeasurementLocator(void) {
	Vrui::popdownPrimaryWidget(measurementDialogPopup);
	delete measurementDialogPopup;
}

void MeasurementLocator::motionCallback(Vrui::LocatorTool::MotionCallbackData* callbackData) {
	if (dragging) {
		/* Update the measurement display dialog: */
		if (numberOfPoints==1)
			startPoint=callbackData->currentTransformation.getOrigin();
		for (int i=0; i<3; ++i)
			startPosition[i]->setValue(double(startPoint[i]));
		if (numberOfPoints==2) {
			endPoint=callbackData->currentTransformation.getOrigin();
			for (int i=0; i<3; ++i)
				endPosition[i]->setValue(double(endPoint[i]));
			float _distance=Geometry::dist(startPoint, endPoint);
			distance->setValue(double(_distance));
		}
	}
}

void MeasurementLocator::buttonPressCallback(Vrui::LocatorTool::ButtonPressCallbackData* callbackData) {
	if (numberOfPoints==2) {
		numberOfPoints=0;
		for (int i=0; i<3; ++i)
			endPosition[i]->setLabel("");
		distance->setLabel("");
	}
	++numberOfPoints;
	dragging=true;
}

void MeasurementLocator::buttonReleaseCallback(Vrui::LocatorTool::ButtonReleaseCallbackData* callbackData) {
	dragging=false;
}

void MeasurementLocator::highlightLocator(GLContextData& glContextData) const {
	GLboolean lightingEnabled=glIsEnabled(GL_LIGHTING);
	if (lightingEnabled)
		glDisable(GL_LIGHTING);
	GLfloat lineWidth;
	glGetFloatv(GL_LINE_WIDTH,&lineWidth);
	glLineWidth(1.0f);
	Vrui::Scalar markerSize=Vrui::getUiSize()/Vrui::getNavigationTransformation().getScaling();
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);
	if (numberOfPoints > 0) {
		glVertex3f(startPoint[0]-markerSize, startPoint[1], startPoint[2]);
		glVertex3f(startPoint[0]+markerSize, startPoint[1], startPoint[2]);
		glVertex3f(startPoint[0], startPoint[1]-markerSize, startPoint[2]);
		glVertex3f(startPoint[0], startPoint[1]+markerSize, startPoint[2]);
		glVertex3f(startPoint[0], startPoint[1], startPoint[2]-markerSize);
		glVertex3f(startPoint[0], startPoint[1], startPoint[2]+markerSize);
	}
	if (numberOfPoints==2) {
		glVertex3f(endPoint[0]-markerSize, endPoint[1], endPoint[2]);
		glVertex3f(endPoint[0]+markerSize, endPoint[1], endPoint[2]);
		glVertex3f(endPoint[0], endPoint[1]-markerSize, endPoint[2]);
		glVertex3f(endPoint[0], endPoint[1]+markerSize, endPoint[2]);
		glVertex3f(endPoint[0], endPoint[1], endPoint[2]-markerSize);
		glVertex3f(endPoint[0], endPoint[1], endPoint[2]+markerSize);
		glVertex3f(startPoint[0], startPoint[1], startPoint[2]);
		glVertex3f(endPoint[0], endPoint[1], endPoint[2]);
	}
	glEnd();
	glLineWidth(lineWidth);
	if (lightingEnabled)
		glEnable(GL_LIGHTING);
}
