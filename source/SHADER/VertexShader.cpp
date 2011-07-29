/*
 * VertexShader.cpp - Methods for VertexShader class.
 *
 * Author: Patrick O'Leary
 * Created: November 12, 2007
 * Copyright 2007. All rights reserved.
 */
#include <iostream>

#include <GL/gl.h>
#include <GL/glu.h>

#include <SHADER/ShaderManager.h>
#include <SHADER/VertexShader.h>

/*
 * VertexShader - Constructor for VertexShader.
 */
VertexShader::VertexShader() {
	setProgramType(1);
	setProgramObject(glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB));
	CHECK_GL_ERROR();
} // end VertexShader()

/*
 * ~VertexShader - Destructor for VertexShader.
 */
VertexShader::~VertexShader() {
} // end ~VertexShader()

/*
 * CheckGLError - Check for GL error.
 * parameter file - char*
 * parameter line - int
 * return - int
 */
int VertexShader::CheckGLError(const char * file, int line) {
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
