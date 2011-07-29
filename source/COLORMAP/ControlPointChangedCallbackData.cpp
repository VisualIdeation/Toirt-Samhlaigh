/*
 * ControlPointChangedCallbackData.cpp - Methods for ControlPointChangedCallbackData class.
 * 
 * Author: Patrick O'Leary
 * Created: November 9, 2007
 * Copyright 2007. All rights reserved.
 */
#include <COLORMAP/ColorMap.h>
#include <COLORMAP/ControlPoint.h>
#include <COLORMAP/ControlPointChangedCallbackData.h>

/*
 * ControlPointChangedCallbackData - Constructor for ControlPointChangedCallbackData.
 */
ControlPointChangedCallbackData::ControlPointChangedCallbackData() {
} // end ControlPointChangedCallbackData()

/*
 * ControlPointChangedCallbackData - Constructor for ControlPointChangedCallbackData.
 */
ControlPointChangedCallbackData::ControlPointChangedCallbackData(ColorMap* _colorMap, ControlPoint* _previousControlPoint,
		ControlPoint* _currentControlPoint) :
	CallbackData(_colorMap) {
	previousControlPoint=_previousControlPoint;
	currentControlPoint=_currentControlPoint;
} // end ControlPointChangedCallbackData()

/*
 * ~ControlPointChangedCallbackData - Destructor for ControlPointChangedCallbackData.
 */
ControlPointChangedCallbackData::~ControlPointChangedCallbackData() {
	previousControlPoint=0;
	currentControlPoint=0;
} // end ~ControlPointChangedCallbackData()

/*
 * getCurrentControlPoint
 * 
 * return - ControlPoint*
 */
ControlPoint* ControlPointChangedCallbackData::getCurrentControlPoint(void) {
	return currentControlPoint;
} // end getCurrentControlPoint()
