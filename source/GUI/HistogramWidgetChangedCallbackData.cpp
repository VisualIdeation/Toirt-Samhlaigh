/*
 * HistogramWidgetChangedCallbackData.cpp - Methods for HistogramWidgetChangedCallbackData class.
 *
 * Author: Patrick O'Leary
 * Created: September 15, 2008
 * Copyright 2008. All rights reserved.
 */
#include <GUI/HistogramWidget.h>
#include <GUI/HistogramWidgetChangedCallbackData.h>

/*
 * HistogramWidgetChangedCallbackData - Constructor for HistogramWidgetChangedCallbackData.
 */
HistogramWidgetChangedCallbackData::HistogramWidgetChangedCallbackData() {
} // end HistogramWidgetChangedCallbackData()

/*
 * HistogramWidgetChangedCallbackData - Constructor for HistogramWidgetChangedCallbackData.
 *
 * parameter _histogramWidget - HistogramWidget *
 */
HistogramWidgetChangedCallbackData::HistogramWidgetChangedCallbackData(HistogramWidget * _histogramWidget) :
	HistogramWidgetCallbackData(_histogramWidget) {
} // end HistogramWidgetChangedCallbackData()

/*
 * ~HistogramWidgetChangedCallbackData - Destructor for HistogramWidgetChangedCallbackData.
 */
HistogramWidgetChangedCallbackData::~HistogramWidgetChangedCallbackData() {
} // end ~HistogramWidgetChangedCallbackData()
