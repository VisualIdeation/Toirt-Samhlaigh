/*
 * FragmentShader.h - Class for fragment shader.
 *
 * Author: Patrick O'Leary
 * Created: November 12, 2007
 * Copyright 2007. All rights reserved.
 */
#ifndef FRAGMENTSHADER_H_
#define FRAGMENTSHADER_H_

#include <SHADER/ShaderProgram.h>

#define CHECK_GL_ERROR() CheckGLError(__FILE__, __LINE__)

class FragmentShader : public ShaderProgram {
public:
	FragmentShader(void);
	~FragmentShader(void);
	int CheckGLError(const char * file, int line);
};

#endif /*FRAGMENTSHADER_H_*/
