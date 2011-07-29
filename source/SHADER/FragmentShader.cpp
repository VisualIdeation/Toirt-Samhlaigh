/*
 * FragmentShader.cpp - Methods for FragmentShader class.
 *
 * Author: Patrick O'Leary
 * Created: November 12, 2007
 * Copyright 2007. All rights reserved.
 */
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>

#include <SHADER/FragmentShader.h>
#include <SHADER/ShaderManager.h>

/*
 * FragmentShader - Constructor for FragmentShader.
 */
FragmentShader::FragmentShader(void) {
	setProgramType(2);
	setProgramObject(glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB));
	CHECK_GL_ERROR();
} // end FragmentShader()

/*
 * ~FragmentShader - Destructor for FragmentShader.
 */
FragmentShader::~FragmentShader(void) {
} // end ~FragmentShader()

/*
 * CheckGLError - Check for GL error.
 *
 * parameter file - char*
 * parameter line - int
 * return - int
 */
int FragmentShader::CheckGLError(const char * file, int line) {
	GLenum glError;
	int returnCode = 0;
	glError = glGetError();
	while (glError != GL_NO_ERROR) {
		std::cout << "GL Error #" << glError << " in File " << file << " at line: " << line << std::endl;
		returnCode = 1;
		glError = glGetError();
	}
	return returnCode;
} // end CheckGLError()
