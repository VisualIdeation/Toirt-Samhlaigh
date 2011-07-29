/*
 * FocusAndContextPlane.cpp - Methods for the FocusAndContextPlane class
 *
 * Author: Patrick O'Leary
 * Created: October 10, 2008
 * Copyright: 2008. All rights reserved.
 */

#include <ANALYSIS/FocusAndContextPlane.h>

/*
 * FocusAndContextPlane - Constructor for FocusAndContextPlane class.
 */
FocusAndContextPlane::FocusAndContextPlane(void) {
} // end FocusAndContextPlane()

/*
 * ~FocusAndContextPlane - Destructor for FocusAndContextPlane class.
 */
FocusAndContextPlane::~FocusAndContextPlane(void) {
} // end ~FocusAndContextPlane()

/*
 * isActive
 *
 * return - bool
 */
bool FocusAndContextPlane::isActive(void) {
	return active;
} // end isActive()

/*
 * setActive
 *
 * parameter _active - bool
 */
void FocusAndContextPlane::setActive(bool _active) {
	active = _active;
} // end setActive()

/*
 * isAllocated
 *
 * return - bool
 */
bool FocusAndContextPlane::isAllocated(void) {
	return allocated;
} // end isAllocated()

/*
 * setAllocated
 *
 * parameter _allocated - bool
 */
void FocusAndContextPlane::setAllocated(bool _allocated) {
	allocated = _allocated;
} // end setAllocated()

/*
 * getPlane
 *
 * return - Vrui::Plane
 */
Vrui::Plane FocusAndContextPlane::getPlane(void) {
	return plane;
} // end getPlane()

/*
 * setPlane
 *
 * parameter _plane - Vrui::Plane
 */
void FocusAndContextPlane::setPlane(Vrui::Plane _plane) {
	plane = _plane;
} // end setPlane()
