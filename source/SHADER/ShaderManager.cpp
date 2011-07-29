/*
 * ShaderManager.cpp - Methods for ShaderManager class.
 * 
 * Author: Patrick O'Leary
 * Created: November 12, 2007
 * Copyright 2007. All rights reserved.
 */
#include <iostream>
#include <string>
#include <vector>

#include <GL/gl.h>
#include <GL/glu.h>

/* Vrui includes */
#include <GL/GLExtensionManager.h>
/*
#include <GL/Extensions/GLARBFragmentShader.h>
#include <GL/Extensions/GLARBShaderObjects.h>
#include <GL/Extensions/GLARBVertexShader.h>
*/

#include <SHADER/GLARBFragmentShader.h>
#include <SHADER/GLARBShaderObjects.h>
#include <SHADER/GLARBShadingLanguage100.h>
#include <SHADER/GLARBVertexShader.h>
#include <SHADER/FragmentShader.h>
#include <SHADER/ShaderManager.h>
#include <SHADER/VertexShader.h>

/*
 * ShaderManager - Constructor for ShaderManager.
 */
ShaderManager::ShaderManager() {
} // end ShaderManager()

/*
 * ~ShaderManager - Destructor for ShaderManager.
 */
ShaderManager::~ShaderManager() {
	std::vector<ShaderObject*>::iterator i=shaderObjectList.begin();
	while (i!=shaderObjectList.end()) {
		ShaderObject* shaderObject = *i;
		i=shaderObjectList.erase(i);
		delete shaderObject;
	}
} // end ~ShaderManager()

/*
 * loadFromFile - Load vertex and fragment shader from files.
 * 
 * parameter vertexFile - char*
 * parameter fragmentFile - char*
 * return - ShaderObject*
 */
ShaderObject* ShaderManager::loadFromFile(char* vertexFile, char* fragmentFile) {
	ShaderObject* shaderObject = new ShaderObject();
	VertexShader* vertexShader = new VertexShader;
	FragmentShader* fragmentShader = new FragmentShader;
	if (vertexFile!=0)
		if (vertexShader->load(vertexFile) != 0) {
			std::cout << "error: can't load vertex shader!" << std::endl;
			delete shaderObject;
			delete vertexShader;
			delete fragmentShader;
			return 0;
		}
	if (fragmentFile!=0)
		if (fragmentShader->load(fragmentFile) != 0) {
			std::cout << "error: can't load fragment shader!" << std::endl;
			delete shaderObject;
			delete vertexShader;
			delete fragmentShader;
			return 0;
		}
	if (vertexFile!=0)
		if (!vertexShader->compile()) {
			std::cout << "COMPILER ERROR (Vertex Shader): ";
			std::cout << vertexShader->getCompilerLog() << std::endl;
			delete shaderObject;
			delete vertexShader;
			delete fragmentShader;
			return 0;
		}
	if (fragmentFile!=0)
		if (!fragmentShader->compile()) {
			std::cout << "COMPILER ERROR (Fragment Shader): ";
			std::cout << fragmentShader->getCompilerLog() << std::endl;
			delete shaderObject;
			delete vertexShader;
			delete fragmentShader;
			return 0;
		}
	if (vertexFile!=0)
		shaderObject->addShader(vertexShader);
	if (fragmentFile!=0)
		shaderObject->addShader(fragmentShader);
	if (!shaderObject->link()) {
		std::cout << "**LINKER ERROR (Shader): ";
		std::cout << shaderObject->getLinkerLog() << std::endl;
		delete shaderObject;
		delete vertexShader;
		delete fragmentShader;
		return 0;
	}
	shaderObjectList.push_back(shaderObject);
	shaderObject->setManageMemory();
	return shaderObject;
} // end loadFromFile()

/*
 * loadFromMemory - Load vertex and fragment shader from memory.
 * 
 * parameter vertexMemory - const char*
 * parameter fragmentMemory - const char*
 * return - ShaderObject*
 */
ShaderObject* ShaderManager::loadFromMemory(const char* vertexMemory, const char* fragmentMemory) {
	ShaderObject* shaderObject = new ShaderObject();
	VertexShader* vertexShader = new VertexShader();
	FragmentShader* fragmentShader = new FragmentShader();
	if (vertexMemory!=0) {
		vertexShader->loadFromMemory(vertexMemory);
	}
	if (fragmentMemory!=0)
		fragmentShader->loadFromMemory(fragmentMemory);
	if (vertexMemory!=0)
		if (!vertexShader->compile()) {
			std::cout << "COMPILER ERROR (Vertex Shader): ";
			std::cout << vertexShader->getCompilerLog() << std::endl;
			delete shaderObject;
			delete vertexShader;
			delete fragmentShader;
			return 0;
		}
	if (fragmentMemory!=0)
		if (!fragmentShader->compile()) {
			std::cout << "COMPILER ERROR (Fragment Shader): ";
			std::cout << fragmentShader->getCompilerLog() << std::endl;
			delete shaderObject;
			delete vertexShader;
			delete fragmentShader;
			return 0;

		}
	if (vertexMemory!=0)
		shaderObject->addShader(vertexShader);
	if (fragmentMemory!=0)
		shaderObject->addShader(fragmentShader);
	if (!shaderObject->link()) {
		std::cout << "LINKER ERROR (Shader): ";
		std::cout << shaderObject->getLinkerLog() << std::endl;
		delete shaderObject;
		delete vertexShader;
		delete fragmentShader;
		return 0;
	}
	shaderObjectList.push_back(shaderObject);
	shaderObject->setManageMemory();
	return shaderObject;
} // end loadFromMemory()

/*
 * free - Remove a shader object.
 * 
 * parameter shaderObject - ShaderObject*
 * return - bool
 */
bool ShaderManager::free(ShaderObject* shaderObject) {
	std::vector<ShaderObject*>::iterator i=shaderObjectList.begin();
	while (i!=shaderObjectList.end()) {
		if ((*i)==shaderObject) {
			shaderObjectList.erase(i);
			delete shaderObject;
			return true;
		}
		i++;
	}
	return false;
}

/*
 * CheckGLError - Check for GL error.
 * 
 * parameter file - char*
 * parameter line - int
 * return - int
 */
int ShaderManager::CheckGLError(char* file, int line) {
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
 * initializeGLSL - Initialize GLSL.
 */
bool ShaderManager::initializeGLSL(void) {
	bool error=true;
	if (GLARBFragmentShader::isSupported()) {
		GLARBFragmentShader::initExtension();
	} else
		error=false;
	if (GLARBVertexShader::isSupported()) {
		GLARBVertexShader::initExtension();
	} else
		error=false;
	if (GLARBShaderObjects::isSupported()) {
		GLARBShaderObjects::initExtension();
	} else
		error=false;
	if (GLARBShadingLanguage100::isSupported()) {
	} else
		error=false;
	return error;
} // end initializeGLSL()
