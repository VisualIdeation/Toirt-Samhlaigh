/*
 * HistogramWidgetCallbackData.cpp - Methods for HistogramWidgetCallbackData class.
 *
 * Author: Patrick O'Leary
 * Created: September 15, 2008
 * Copyright 2008. All rights reserved.
 */
#include <GUI/HistogramWidgetCallbackData.h>
#include <GUI/HistogramWidget.h>

/*
 * HistogramWidgetCallbackData - Constructor for HistogramWidgetCallbackData.
 */
HistogramWidgetCallbackData::HistogramWidgetCallbackData(void) {
} // end HistogramWidgetCallbackData()

/*
 * HistogramWidgetCallbackData - Constructor for HistogramWidgetCallbackData.
 *
 * parameter _histogramWidget - HistogramWidget
 */
HistogramWidgetCallbackData::HistogramWidgetCallbackData(HistogramWidget * _histogramWidget) {
    histogramWidget=_histogramWidget;
} //end HistogramWidgetCallbackData()

/*
 * ~HistogramWidgetCallbackData - Destructor for HistogramWidgetCallbackData.
 */
HistogramWidgetCallbackData::~HistogramWidgetCallbackData() {
	histogramWidget=0;
} // end ~HistogramWidgetCallbackData()
