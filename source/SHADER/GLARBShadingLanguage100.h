/*
 * GLARBShadingLanguage100.h - Class for determining if GLSL shading is available.
 * 
 * Author: Patrick O'Leary
 * Created: November 12, 2007
 * Copyright 2007. All rights reserved.
 */
#ifndef GLARBSHADINGLANGUAGE100_H_
#define GLARBSHADINGLANGUAGE100_H_

/* Vrui includes */
#include <GL/GLExtensionManager.h>

class GLARBShadingLanguage100 {
public:
	GLARBShadingLanguage100(void);
	~GLARBShadingLanguage100(void);
	static bool isSupported(void) {
		return GLExtensionManager::isExtensionSupported("GL_ARB_shading_language_100");
	}
	;
};

#endif /*GLARBSHADINGLANGUAGE100_H_*/
