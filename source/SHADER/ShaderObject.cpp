/*
 * ShaderObject.cpp - Methods for ShaderObject class.
 *
 * Author: Patrick O'Leary
 * Created: November 12, 2007
 * Copyright 2007. All rights reserved.
 */

#include <cstdlib>

#include <SHADER/ShaderManager.h>
#include <SHADER/ShaderObject.h>

/*
 * ShaderObject - Constructor for ShaderObject.
 */
ShaderObject::ShaderObject(void) {
	shaderObject = 0;
	linkerLog = 0;
	manageMemory = false;
	shaderObject = glCreateProgramObjectARB();
	linked = false;
} // end ShaderObject()

/*
 * ~ShaderObject - Destructor for ShaderObject.
 */
ShaderObject::~ShaderObject(void) {
	if (linkerLog!=0)
		free(linkerLog);
	for (int i=0; i<shaderList.size(); i++) {
		glDetachObjectARB(shaderObject, shaderList[i]->getProgramObject());
		CHECK_GL_ERROR();
		if (manageMemory)
			delete shaderList[i];
	}
	glDeleteObjectARB(shaderObject);
	CHECK_GL_ERROR();
} // end ~ShaderObject()

/*
 * addShader
 *
 * parameter shaderProgram - ShaderProgram*
 */
void ShaderObject::addShader(ShaderProgram* shaderProgram) {
	if (shaderProgram==0)
		return;
	if (!shaderProgram->isCompiled()) {
		std::cout << "**warning** please compile program before adding object! trying to compile now..." << std::endl;
		if (!shaderProgram->compile()) {
			std::cout << "...compile ERROR!" << std::endl;
			return;
		} else {
			std::cout << "...ok!" << std::endl;
		}
	}
	shaderList.push_back(shaderProgram);
} // end addShader()

/*
 * begin - Using shader.
 */
void ShaderObject::begin(void) {
	if (shaderObject == 0)
		return;
	if (linked) {
		glUseProgramObjectARB(shaderObject);
		CHECK_GL_ERROR();
	}
} // end begin()

/*
 * CheckGLError - Check for GL error.
 *
 * parameter file - char*
 * parameter line - char*
 * return - int
 */
int ShaderObject::CheckGLError(const char * file, int line) {
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
 * end - Using shader.
 */
void ShaderObject::end(void) {
	glUseProgramObjectARB(0);
	CHECK_GL_ERROR();
} // end end()

/*
 * getLinkerLog
 *
 * return - char*
 */
const char * ShaderObject::getLinkerLog(void) {
	int byteLength = 0;
	int stringLength = 0;
	if (shaderObject==0)
		return "Not a valid object";
	glGetObjectParameterivARB(shaderObject, GL_OBJECT_INFO_LOG_LENGTH_ARB, &byteLength);
	CHECK_GL_ERROR();
	if (byteLength > 1) {
		if (linkerLog!=0) {
			free(linkerLog);
			linkerLog =0;
		}
		if ((linkerLog = (GLcharARB*)malloc(byteLength)) == NULL) {
			std::cout << "ERROR: Could not allocationate linker log buffer" << std::endl;
			return "Out of memory";
		}
		glGetInfoLogARB(shaderObject, byteLength, &stringLength, linkerLog);
		CHECK_GL_ERROR();
	}
	if (linkerLog!=0)
		return (char*) linkerLog;
	return "Unknown linker log error";
} // end getLinkerLog()

/*
 * getUniLoc
 *
 * parameter name - const GLcharARB*
 * return - GLint
 */
GLint ShaderObject::getUniLoc(const GLcharARB* name) {
	GLint location;
	location = glGetUniformLocationARB(shaderObject, name);
	if (location == -1) {
		std::cout << "Error: can't find uniform variable \"" << name << "\"" << std::endl;
	}
	CHECK_GL_ERROR();
	return location;
} // end getUniLoc()

/*
 * getUniformfv
 *
 * parameter name - char*
 * parameter values - GLfloat*
 */
void ShaderObject::getUniformfv(const char * name, GLfloat* values) {
	GLint location;
	location = glGetUniformLocationARB(shaderObject, name);
	if (location == -1) {
		std::cout << "Error: can't find uniform variable \"" << name << "\"" << std::endl;
	}
	glGetUniformfvARB(shaderObject, location, values);

} // end getUniformfv()

/*
 * getUniformiv
 *
 * parameter name - char*
 * parameter values - GLint*
 */
void ShaderObject::getUniformiv(const char * name, GLint* values) {
	GLint location;
	location = glGetUniformLocationARB(shaderObject, name);
	if (location == -1) {
		std::cout << "Error: can't find uniform variable \"" << name << "\"" << std::endl;
	}

	glGetUniformivARB(shaderObject, location, values);

} // end getUniformiv()

/*
 * link - Link the shader object.
 *
 * return - bool
 */
bool ShaderObject::link(void) {
	if (linked) {
		std::cout << "**warning** Object is already linked, trying to link again" << std::endl;
		for (int i=0; i<shaderList.size(); i++) {
			glDetachObjectARB(shaderObject, shaderList[i]->getProgramObject());
			CHECK_GL_ERROR();
		}
	}
	for (int i=0; i<shaderList.size(); i++) {
		glAttachObjectARB(shaderObject, shaderList[i]->getProgramObject());
		CHECK_GL_ERROR();
	}
	int linkStatus;
	glLinkProgramARB(shaderObject);
	CHECK_GL_ERROR();
	glGetObjectParameterivARB(shaderObject, GL_OBJECT_LINK_STATUS_ARB, &linkStatus);
	CHECK_GL_ERROR();
	if (linkStatus) {
		linked = true;
		return true;
	} else {
		std::cout << "**linker error**" << std::endl;
	}
	return false;
} // end link()

/*
 * sendUniform1f
 *
 * parameter name - char*
 * parameter value - GLfloat
 * return - bool
 */
bool ShaderObject::sendUniform1f(const char * name, GLfloat value) {
	GLint location = getUniLoc(name);
	if (location==-1)
		return false;
	glUniform1fARB(location, value);
	return true;
} // end sendUniform1f()

/*
 * sendUniform2f
 *
 * parameter name - char*
 * parameter value0 - GLfloat
 * parameter value1 - GLfloat
 * return - bool
 */
bool ShaderObject::sendUniform2f(const char * name, GLfloat value0, GLfloat value1) {
	GLint location = getUniLoc(name);
	if (location==-1)
		return false;
	glUniform2fARB(location, value0, value1);
	return true;
} // end sendUniform2f()

/*
 * sendUniform3f
 *
 * parameter name - char*
 * parameter value0 - GLfloat
 * parameter value1 - GLfloat
 * parameter value2 - GLfloat
 * return - bool
 */
bool ShaderObject::sendUniform3f(const char * name, GLfloat value0, GLfloat value1, GLfloat value2) {
	GLint location = getUniLoc(name);
	if (location==-1)
		return false;
	glUniform3fARB(location, value0, value1, value2);
	return true;
} // end sendUniform3f()

/*
 * sendUniform4f
 *
 * parameter name - char*
 * parameter value0 - GLfloat
 * parameter value1 - GLfloat
 * parameter value2 - GLfloat
 * parameter value3 - GLfloat
 * return - bool
 */
bool ShaderObject::sendUniform4f(const char * name, GLfloat value0, GLfloat value1, GLfloat value2, GLfloat value3) {
	GLint location = getUniLoc(name);
	if (location==-1)
		return false;
	glUniform4fARB(location, value0, value1, value2, value3);
	return true;
} // end sendUniform4f()

/*
 * sendUniform1i
 *
 * parameter name - char*
 * parameter value - GLint
 * return - bool
 */
bool ShaderObject::sendUniform1i(const char * name, GLint value) {
	GLint location = getUniLoc(name);
	if (location==-1)
		return false;
	glUniform1iARB(location, value);
	return true;
} // end sendUniform1i()

/*
 * sendUniform2i
 *
 * parameter name - char*
 * parameter value0 - GLint
 * parameter value1 - GLint
 * return - bool
 */
bool ShaderObject::sendUniform2i(const char * name, GLint value0, GLint value1) {
	GLint location = getUniLoc(name);
	if (location==-1)
		return false;
	glUniform2iARB(location, value0, value1);
	return true;
} // end sendUniform2i()

/*
 * sendUniform3i
 *
 * parameter name - char*
 * parameter value0 - GLint
 * parameter value1 - GLint
 * parameter value2 - GLint
 * return - bool
 */
bool ShaderObject::sendUniform3i(const char * name, GLint value0, GLint value1, GLint value2) {
	GLint location = getUniLoc(name);
	if (location==-1)
		return false;
	glUniform3iARB(location, value0, value1, value2);
	return true;
} // end sendUniform3i()

/*
 * sendUniform4i
 *
 * parameter name - char*
 * parameter value0 - GLint
 * parameter value1 - GLint
 * parameter value2 - GLint
 * parameter value3 - GLint
 * return - bool
 */
bool ShaderObject::sendUniform4i(const char * name, GLint value0, GLint value1, GLint value2, GLint value3) {
	GLint location = getUniLoc(name);
	if (location==-1)
		return false;
	glUniform4iARB(location, value0, value1, value2, value3);
	return true;
} // end sendUniform4i()

/*
 * sendUniform1fv
 *
 * parameter name - char*
 * parameter count - GLsizei
 * parameter values - GLfloat*
 * return - bool
 */
bool ShaderObject::sendUniform1fv(const char * name, GLsizei count, GLfloat* values) {
	GLint location = getUniLoc(name);
	if (location==-1)
		return false;
	glUniform1fvARB(location, count, values);
	return true;
} // end sendUniform1fv()

/*
 * sendUniform2fv
 *
 * parameter name - char*
 * parameter count - GLsizei
 * parameter values - GLfloat*
 * return - bool
 */
bool ShaderObject::sendUniform2fv(const char *name, GLsizei count, GLfloat* values) {
	GLint location = getUniLoc(name);
	if (location==-1)
		return false;
	glUniform2fvARB(location, count, values);
	return true;
} // end sendUniform2fv()

/*
 * sendUniform3fv
 *
 * parameter name - char*
 * parameter count - GLsizei
 * parameter values - GLfloat*
 * return - bool
 */
bool ShaderObject::sendUniform3fv(const char * name, GLsizei count, GLfloat* values) {
	GLint location = getUniLoc(name);
	if (location==-1)
		return false;
	glUniform3fvARB(location, count, values);
	return true;
} // end sendUniform3fv()

/*
 * sendUniform4fv
 *
 * parameter name - char*
 * parameter count - GLsizei
 * parameter values - GLfloat*
 * return - bool
 */
bool ShaderObject::sendUniform4fv(const char * name, GLsizei count, GLfloat* values) {
	GLint location = getUniLoc(name);
	if (location==-1)
		return false;
	glUniform4fvARB(location, count, values);
	return true;
} // end sendUniform4fv()

/*
 * sendUniform1iv
 *
 * parameter name - char*
 * parameter count - GLsizei
 * parameter values - GLint*
 * return - bool
 */
bool ShaderObject::sendUniform1iv(const char * name, GLsizei count, GLint* values) {
	GLint location = getUniLoc(name);
	if (location==-1)
		return false;
	glUniform1ivARB(location, count, values);
	return true;
} // end sendUniform1iv()

/*
 * sendUniform2iv
 *
 * parameter name - char*
 * parameter count - GLsizei
 * parameter values - GLint*
 * return - bool
 */
bool ShaderObject::sendUniform2iv(const char * name, GLsizei count, GLint* values) {
	GLint location = getUniLoc(name);
	if (location==-1)
		return false;
	glUniform2ivARB(location, count, values);
	return true;
} // end sendUniform2iv()

/*
 * sendUniform3iv
 *
 * parameter name - char*
 * parameter count - GLsizei
 * parameter values - GLint*
 * return - bool
 */
bool ShaderObject::sendUniform3iv(const char * name, GLsizei count, GLint* values) {
	GLint location = getUniLoc(name);
	if (location==-1)
		return false;
	glUniform3ivARB(location, count, values);
	return true;
} // end sendUniform3iv()

/*
 * sendUniform4iv
 *
 * parameter name - char*
 * parameter count - GLsizei
 * parameter values - GLint*
 * return - bool
 */
bool ShaderObject::sendUniform4iv(const char * name, GLsizei count, GLint* values) {
	GLint location = getUniLoc(name);
	if (location==-1)
		return false;
	glUniform4ivARB(location, count, values);
	return true;
} // end sendUniform4iv()

/*
 * sendUniformMatrix2fv
 *
 * parameter name - char*
 * parameter count - GLsizei
 * parameter transpose - GLboolean
 * parameter values - GLfloat*
 * return - bool
 */
bool ShaderObject::sendUniformMatrix2fv(const char * name, GLsizei count, GLboolean transpose, GLfloat* values) {
	GLint location = getUniLoc(name);
	if (location==-1)
		return false;
	glUniformMatrix2fvARB(location, count, transpose, values);
	return true;
} // end sendUniformMatrix2fv()

/*
 * sendUniformMatrix3fv
 *
 * parameter name - char*
 * parameter count - GLsizei
 * parameter transpose - GLboolean
 * parameter values - GLfloat*
 * return - bool
 */
bool ShaderObject::sendUniformMatrix3fv(const char * name, GLsizei count, GLboolean transpose, GLfloat* values) {
	GLint location = getUniLoc(name);
	if (location==-1)
		return false;
	glUniformMatrix3fvARB(location, count, transpose, values);
	return true;
} // end sendUniformMatrix3fv()

/*
 * sendUniformMatrix4fv
 *
 * parameter name - char*
 * parameter count - GLsizei
 * parameter transpose - GLboolean
 * parameter values - GLfloat*
 * return - bool
 */
bool ShaderObject::sendUniformMatrix4fv(const char * name, GLsizei count, GLboolean transpose, GLfloat* values) {
	GLint location = getUniLoc(name);
	if (location==-1)
		return false;
	glUniformMatrix4fvARB(location, count, transpose, values);
	return true;
} // end sendUniformMatrix4fv()

/*
 * setManageMemory
 */
void ShaderObject::setManageMemory(void) {
	manageMemory = true;
} // end setManageMemory()
