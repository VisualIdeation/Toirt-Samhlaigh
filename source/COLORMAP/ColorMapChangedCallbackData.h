/*
 * ColorMapChangedCallbackData.h - Class for color map changed callback data.
 * 
 * Author: Patrick O'Leary
 * Created: November 9, 2007
 * Copyright 2007. All rights reserved.
 */
#ifndef COLORMAPCHANGEDCALLBACKDATA_H_
#define COLORMAPCHANGEDCALLBACKDATA_H_

#include <COLORMAP/CallbackData.h>

// begin Forward Declarations
class ColorMap;
// end Forward Declarations

class ColorMapChangedCallbackData : public CallbackData {
public:
	ColorMapChangedCallbackData();
	ColorMapChangedCallbackData(ColorMap* sColorMap);
	~ColorMapChangedCallbackData();
};

#endif /*COLORMAPCHANGEDCALLBACKDATA_H_*/
