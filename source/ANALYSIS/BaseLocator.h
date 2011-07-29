/*
 * BaseLocator
 *
 * Author: Patrick O'Leary
 * Created: February 8, 2008
 * Copyright 2008. All rights reserved.
 */
#ifndef BASELOCATOR_H_
#define BASELOCATOR_H_

/* Vrui includes */
#include <Vrui/LocatorToolAdapter.h>

// begin Forward Declarations
namespace Vrui {
class LocatorTool;
}
class Toirt_Samhlaigh;
// end Forward Declarations
class BaseLocator : public Vrui::LocatorToolAdapter {
public:
	BaseLocator(Vrui::LocatorTool* _locatorTool, Toirt_Samhlaigh* _application);
	~BaseLocator();
	virtual void highlightLocator(GLContextData& contextData) const;
	virtual void glRenderAction(GLContextData& contextData) const;
	virtual void glRenderActionTransparent(GLContextData& contextData) const;
private:
	Toirt_Samhlaigh* application;
};

#endif /*BASELOCATOR_H_*/
