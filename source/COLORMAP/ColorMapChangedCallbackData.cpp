/*
 * CallbackData.cpp - Methods for ColorMapChangedCallbackData class.
 * 
 * Author: Patrick O'Leary
 * Created: November 9, 2007
 * Copyright 2007. All rights reserved.
 */
#include <COLORMAP/ColorMap.h>
#include <COLORMAP/ColorMapChangedCallbackData.h>

/*
 * ColorMapChangedCallbackData - Constructor for ColorMapChangedCallbackData.
 */
ColorMapChangedCallbackData::ColorMapChangedCallbackData() {
} // end ColorMapChangedCallbackData()

/*
 * ColorMapChangedCallbackData - Constructor for ColorMapChangedCallbackData.
 * 
 * parameter _colorMap - ColorMap*
 */
ColorMapChangedCallbackData::ColorMapChangedCallbackData(ColorMap* _colorMap) :
	CallbackData(_colorMap) {
} // end ColorMapChangedCallbackData()

/*
 * ~ColorMapChangedCallbackData - Destructor for ColorMapChangedCallbackData.
 */
ColorMapChangedCallbackData::~ColorMapChangedCallbackData() {
} // end ~ColorMapChangedCallbackData()
