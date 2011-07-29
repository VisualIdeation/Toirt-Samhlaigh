/*
 * QuadrilateralChangedCallbackData.cpp - Methods for QuadrilateralChangedCallbackData class.
 *
 * Author: Patrick O'Leary
 * Created: September 24, 2008
 * Copyright 2008. All rights reserved.
 */
#include <DATASTRUCTURE/Quadrilateral.h>
#include <GUI/HistogramWidget.h>
#include <GUI/QuadrilateralChangedCallbackData.h>

/*
 * QuadrilateralChangedCallbackData - Constructor for QuadrilateralChangedCallbackData.
 */
QuadrilateralChangedCallbackData::QuadrilateralChangedCallbackData() {
} // end QuadrilateralChangedCallbackData()

/*
 * QuadrilateralChangedCallbackData - Constructor for QuadrilateralChangedCallbackData.
 */
QuadrilateralChangedCallbackData::QuadrilateralChangedCallbackData(HistogramWidget * _histogramWidget,
        Quadrilateral * _previousQuadrilateral, Quadrilateral * _currentQuadrilateral) :
    HistogramWidgetCallbackData(_histogramWidget) {
    previousQuadrilateral = _previousQuadrilateral;
    currentQuadrilateral = _currentQuadrilateral;
} // end QuadrilateralChangedCallbackData()

/*
 * ~QuadrilateralChangedCallbackData - Destructor for QuadrilateralChangedCallbackData.
 */
QuadrilateralChangedCallbackData::~QuadrilateralChangedCallbackData() {
    previousQuadrilateral = 0;
    currentQuadrilateral = 0;
} // end ~QuadrilateralChangedCallbackData()

/*
 * getCurrentQuadrilateral
 *
 * return - Quadrilateral*
 */
Quadrilateral* QuadrilateralChangedCallbackData::getCurrentQuadrilateral(void) {
    return currentQuadrilateral;
} // end getCurrentQuadrilateral()
