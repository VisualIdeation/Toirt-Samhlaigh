/*
 * HistogramWidgetChangedCallbackData.h - Class for histogram widget changed callback data.
 *
 * Author: Patrick O'Leary
 * Created: September 15, 2008
 * Copyright 2008. All rights reserved.
 */
#ifndef HISTOGRAMWIDGETCHANGEDCALLBACKDATA_H_
#define HISTOGRAMWIDGETCHANGEDCALLBACKDATA_H_

#include <GUI/HistogramWidgetCallbackData.h>

// begin Forward Declarations
class HistogramWidget;
// end Forward Declarations

class HistogramWidgetChangedCallbackData : public HistogramWidgetCallbackData {
public:
	HistogramWidgetChangedCallbackData();
	HistogramWidgetChangedCallbackData(HistogramWidget * _histogramWidget);
	~HistogramWidgetChangedCallbackData();
};

#endif /*HISTOGRAMWIDGETCHANGEDCALLBACKDATA_H_*/
