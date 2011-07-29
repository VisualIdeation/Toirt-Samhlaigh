/*
 * ShaderProgram.h - Class for a shader program.
 *
 * Author: Patrick O'Leary
 * Created: November 12, 2007
 * Copyright 2007. All rights reserved.
 */
#ifndef SHADERPROGRAM_H_
#define SHADERPROGRAM_H_

#include <iostream>

#include <GL/gl.h>

#define CHECK_GL_ERROR() CheckGLError(__FILE__, __LINE__)

class ShaderProgram {
public:
	ShaderProgram(void);
	~ShaderProgram(void);
	int CheckGLError(const char * file, int line);
	bool compile(void);
	bool isCompiled(void);
	const char * getCompilerLog(void);
	unsigned long getFileLength(std::ifstream& file);
	GLhandleARB getProgramObject(void);
	void setProgramObject(GLhandleARB _programObject);
	void setProgramType(int _programType);
	int load(char* filename);
	void loadFromMemory(const char* program);
private:
	int programType;
	GLhandleARB programObject;
	GLubyte* shaderSource;
	GLcharARB* compilerLog;
	bool compiled;
	bool memoryAllocation;
};

#endif /*SHADERPROGRAM_H_*/
