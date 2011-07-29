/*
 * QuadrilateralChangedCallbackData.h - Class for quadrilateral changed callback data.
 *
 * Author: Patrick O'Leary
 * Created: September 24, 2008
 * Copyright 2008. All rights reserved.
 */
#ifndef QUADRILATERALCHANGEDCALLBACKDATA_H_
#define QUADRILATERALCHANGEDCALLBACKDATA_H_

#include <GUI/HistogramWidgetCallbackData.h>

// begin Forward Declarations
class HistogramWidget;
class Quadrilateral;
// end Forward Declarations

class QuadrilateralChangedCallbackData : public HistogramWidgetCallbackData {
public:
	QuadrilateralChangedCallbackData();
	QuadrilateralChangedCallbackData(HistogramWidget * _histogramWidget, Quadrilateral* _previousQuadrilateral, Quadrilateral* _currentQuadrilateral);
	~QuadrilateralChangedCallbackData();
	Quadrilateral* getCurrentQuadrilateral(void);
private:
	Quadrilateral* previousQuadrilateral;
	Quadrilateral* currentQuadrilateral;
};

#endif /*QUADRILATERALCHANGEDCALLBACKDATA_H_*/
