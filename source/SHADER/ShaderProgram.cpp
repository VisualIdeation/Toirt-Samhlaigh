/*
 * ShaderProgram.cpp - Methods for ShaderProgram class.
 *
 * Author: Patrick O'Leary
 * Created: November 12, 2007
 * Copyright 2007. All rights reserved.
 */

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>

#include <SHADER/ShaderManager.h>
#include <SHADER/ShaderProgram.h>

/*
 * ShaderProgram - Constructor for ShaderProgram.
 */
ShaderProgram::ShaderProgram(void) {
	compilerLog = 0;
	compiled = false;
	programType = 0;
	programObject = 0;
	shaderSource = 0;
	memoryAllocation = false;
} // end ShaderProgram()

/*
 * ~ShaderProgram - Destructor for ShaderProgram.
 */
ShaderProgram::~ShaderProgram(void) {
	if (compilerLog!=0)
		free(compilerLog);
	if (shaderSource!=0) {
		if (memoryAllocation)
			delete[] shaderSource;
	}
	if (compiled) {
		glDeleteObjectARB(programObject);
		CHECK_GL_ERROR();
	}
} // end ~ShaderProgram()

/*
 * CheckGLError - Check for GL error.
 *
 * parameter file - char*
 * parameter line - int
 * return - int
 */
int ShaderProgram::CheckGLError(const char * file, int line) {
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

/*
 * compile - Compile shader program.
 *
 * return - bool
 */
bool ShaderProgram::compile(void) {
	compiled = false;
	int compileStatus = 0;
	if (shaderSource==0)
		return false;
	GLint length = (GLint) strlen((const char*)shaderSource);
	glShaderSourceARB(programObject, 1, (const GLcharARB **)&shaderSource, &length);
	CHECK_GL_ERROR();
	glCompileShaderARB(programObject);
	CHECK_GL_ERROR();
	glGetObjectParameterivARB(programObject, GL_OBJECT_COMPILE_STATUS_ARB, &compileStatus);
	CHECK_GL_ERROR();
	if (compileStatus)
		compiled=true;
	return compiled;
} // end compile()

/*
 * isCompiled
 *
 * return - bool
 */
bool ShaderProgram::isCompiled(void) {
	return compiled;
} // end isCompiled()

/*
 * getCompilerLog
 *
 * return - char*
 */
const char * ShaderProgram::getCompilerLog(void) {
	int byteLength = 0;
	int stringLength = 0;
	if (programObject==0)
		return "Not a valid object";
	glGetObjectParameterivARB(programObject, GL_OBJECT_INFO_LOG_LENGTH_ARB, &byteLength);
	CHECK_GL_ERROR();
	if (byteLength > 1) {
		if (compilerLog!=0) {
			free(compilerLog);
			compilerLog =0;
		}
		if ((compilerLog = (GLcharARB*)malloc(byteLength)) == NULL) {
			std::cout << "ERROR: Could not allocate compiler log buffer" << std::endl;
			return "Out of memory";
		}
		glGetInfoLogARB(programObject, byteLength, &stringLength, compilerLog);
		CHECK_GL_ERROR();
	}
	if (compilerLog!=0)
		return (char*) compilerLog;
	return "Unknown compiler log error";
} // end getCompilerLog()

/*
 * getFileLength
 *
 * parameter file - std::ifstream&
 * return - unsigned long
 */
unsigned long ShaderProgram::getFileLength(std::ifstream& file) {
	if (!file.good())
		return 0;
	unsigned long position=file.tellg();
	file.seekg(0, std::ios::end);
	unsigned long length = file.tellg();
	file.seekg(std::ios::beg);
	return length;
} // end getFileLength()

/*
 * getProgramObject
 *
 * return - GLhandleARB
 */
GLhandleARB ShaderProgram::getProgramObject(void) {
	return programObject;
} // end getProgramObject()

/*
 * setProgramObject
 *
 * parameter _programObject - GLhandleARB
 */
void ShaderProgram::setProgramObject(GLhandleARB _programObject) {
	programObject=_programObject;
} // end setProgramObject()

/*
 * setProgramType
 *
 * parameter _programType - int
 */
void ShaderProgram::setProgramType(int _programType) {
	programType=_programType;
} // end setProgramType()

/*
 * load
 *
 * parameter filename - char*
 * return - int
 */
int ShaderProgram::load(char* filename) {
	std::ifstream file;
	file.open(filename, std::ios::in);
	if (!file)
		return -1;
	unsigned long length = getFileLength(file);
	if (length==0)
		return -2;
	if (shaderSource!=0) {
		if (memoryAllocation)
			delete[] shaderSource;
	}
	shaderSource = (GLubyte*) new char[length+1];
	if (shaderSource == 0)
		return -3;
	memoryAllocation = true;
	shaderSource[length] = 0;
	unsigned int i=0;
	while (file.good()) {
		shaderSource[i++] = file.get();
		if (i>length)
			i=length;
	}
	shaderSource[i] = 0;
	file.close();
	return 0;
} // end load()

/*
 * loadFromMemory
 *
 * parameter program - const char*
 */
void ShaderProgram::loadFromMemory(const char* program) {
	if (shaderSource!=0) {
		if (memoryAllocation)
			delete[] shaderSource;
	}
	memoryAllocation = false;
	shaderSource = (GLubyte*) program;
} // end loadFromMemory()
