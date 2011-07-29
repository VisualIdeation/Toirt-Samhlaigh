/*
 * VertexShader.h - Class for vertex shader.
 *
 * Author: Patrick O'Leary
 * Created: November 12, 2007
 * Copyright 2007. All rights reserved.
 */
#ifndef VERTEXSHADER_H_
#define VERTEXSHADER_H_

#include <SHADER/ShaderProgram.h>

#define CHECK_GL_ERROR() CheckGLError(__FILE__, __LINE__)

class VertexShader : public ShaderProgram {
public:
	VertexShader(void);
	~VertexShader(void);
	int CheckGLError(const char * file, int line);
};

#endif /*VERTEXSHADER_H_*/
