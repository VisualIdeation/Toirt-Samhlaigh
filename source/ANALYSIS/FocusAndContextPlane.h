/*
 * FocusAndContextPlane.h - Class for ...
 *
 * Author: Patrick O'Leary
 * Created: October 10, 2008
 * Copyright: 2008. All rights reserved.
 */

#ifndef FOCUSANDCONTEXT_H_
#define FOCUSANDCONTEXT_H_

/* Vrui includes */
#include <Geometry/Plane.h>
#include <Vrui/Geometry.h>

class FocusAndContextPlane {
public:
	FocusAndContextPlane(void);
	~FocusAndContextPlane(void);
	bool isActive(void);
	void setActive(bool _active);
	bool isAllocated(void);
	void setAllocated(bool _allocated);
	Vrui::Plane getPlane(void);
	void setPlane(Vrui::Plane plane);
private:
	bool active;
	bool allocated;
	Vrui::Plane plane;
};

#endif /*FOCUSANDCONTEXT_H_*/
