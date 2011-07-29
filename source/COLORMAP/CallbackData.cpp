/*
 * CallbackData.cpp - Methods for CallbackData class.
 * 
 * Author: Patrick O'Leary
 * Created: November 9, 2007
 * Copyright 2007. All rights reserved.
 */
#include <COLORMAP/CallbackData.h>
#include <COLORMAP/ColorMap.h>

/*
 * CallbackData - Constructor for CallbackData.
 */
CallbackData::CallbackData() {
} // end CallbackData()

/*
 * CallbackData - Constructor for CallbackData.
 * 
 * parameter _colorMap - ColorMap
 */
CallbackData::CallbackData(ColorMap* _colorMap) {
	colorMap=_colorMap;
} //end CallbackData()

/*
 * ~CallbackData - Destructor for CallbackData.
 */
CallbackData::~CallbackData() {
	colorMap=0;
} // end ~CallbackData()
