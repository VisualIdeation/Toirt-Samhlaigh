/*
 * ScalarWidgetCallbackData.cpp - Methods for ScalarWidgetCallbackData class.
 *
 * Author: Patrick O'Leary
 * Created: January 19, 2008
 * Copyright 2008. All rights reserved.
 */
#include <GUI/ScalarWidget.h>
#include <GUI/ScalarWidgetCallbackData.h>

/*
 * ScalarWidgetCallbackData - Constructor for ScalarWidgetCallbackData.
 */
ScalarWidgetCallbackData::ScalarWidgetCallbackData(void) {
} // end ScalarWidgetCallbackData()

/*
 * ScalarWidgetCallbackData - Constructor for ScalarWidgetCallbackData.
 *
 * parameter _scalarWidget - ScalarWidget*
 */
ScalarWidgetCallbackData::ScalarWidgetCallbackData(ScalarWidget* _scalarWidget) {
	scalarWidget=_scalarWidget;
} //end ScalarWidgetCallbackData()

/*
 * ~ScalarWidgetCallbackData - Destructor for ScalarWidgetCallbackData.
 */
ScalarWidgetCallbackData::~ScalarWidgetCallbackData(void) {
	scalarWidget=0;
} // end ~ScalarWidgetCallbackData()
