/*
 * HistogramWidgetCallbackData.h - Class for histogram widget callback data.
 *
 * Author: Patrick O'Leary
 * Created: September 15, 2008
 * Copyright 2008. All rights reserved.
 */
#ifndef HISTOGRAMWIDGETCALLBACKDATA_H_
#define HISTOGRAMWIDGETCALLBACKDATA_H_

#include <Misc/CallbackData.h>

// begin Forward Declarations
class HistogramWidget;
// end Forward Declarations

class HistogramWidgetCallbackData : public Misc::CallbackData {
public:
    HistogramWidget * histogramWidget;
    HistogramWidgetCallbackData();
    HistogramWidgetCallbackData(HistogramWidget * _histogramWidget);
	~HistogramWidgetCallbackData();
};

#endif /*HISTOGRAMWIDGETCALLBACKDATA_H_*/
