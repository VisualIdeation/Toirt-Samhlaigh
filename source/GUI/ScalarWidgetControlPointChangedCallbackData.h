/*
 * ScalarWidgetControlPointChangedCallbackData.h - Class for scalar widget control point changed callback data.
 *
 * Author: Patrick O'Leary
 * Created: January 19, 2008
 * Copyright 2008. All rights reserved.
 */
#ifndef SCALARWIDGETCONTROLPOINTCHANGEDCALLBACKDATA_H_
#define SCALARWIDGETCONTROLPOINTCHANGEDCALLBACKDATA_H_

#include <GUI/ScalarWidgetCallbackData.h>

// begin Forward Declarations
class ScalarWidget;
class ScalarWidgetControlPoint;
// end Forward Declarations

class ScalarWidgetControlPointChangedCallbackData : public ScalarWidgetCallbackData {
public:
	ScalarWidgetControlPointChangedCallbackData(void);
	ScalarWidgetControlPointChangedCallbackData(ScalarWidget * _scalarWidget, ScalarWidgetControlPoint * _previousControlPoint, ScalarWidgetControlPoint * _currentControlPoint);
	~ScalarWidgetControlPointChangedCallbackData(void);
	ScalarWidgetControlPoint* getCurrentControlPoint(void);
private:
    ScalarWidgetControlPoint* previousControlPoint;
    ScalarWidgetControlPoint* currentControlPoint;
};

#endif /*SCALARWIDGETCONTROLPOINTCHANGEDCALLBACKDATA_H_*/
