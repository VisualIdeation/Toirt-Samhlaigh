/*
 * CallbackData.h - Class for callback data.
 * 
 * Author: Patrick O'Leary
 * Created: November 9, 2007
 * Copyright 2007. All rights reserved.
 */
#ifndef CALLBACKDATA_H_
#define CALLBACKDATA_H_

#include <Misc/CallbackData.h>

// begin Forward Declarations
class ColorMap;
// end Forward Declarations

class CallbackData : public Misc::CallbackData {
public:
	ColorMap* colorMap;
	CallbackData();
	CallbackData(ColorMap* _colorMap);
	~CallbackData();
};

#endif /*CALLBACKDATA_H_*/
