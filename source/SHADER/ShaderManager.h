/*
 * ShaderManager.h - Class for managing shader programs.
 * 
 * Author: Patrick O'Leary
 * Created: November 12, 2007
 * Copyright 2007. All rights reserved.
 */
#ifndef SHADERMANAGER_H_
#define SHADERMANAGER_H_

#define DEFAULT 0
#define INTERACTIVE 1

#define CHECK_GL_ERROR() CheckGLError(__FILE__, __LINE__)

#include <SHADER/ShaderObject.h>

class ShaderManager {
public:

	ShaderManager(void);
	~ShaderManager(void);
	int CheckGLError(char* file, int line);
	bool initializeGLSL(void);
	ShaderObject* loadFromFile(char* vertexFile, char* fragmentFile);
	ShaderObject* loadFromMemory(const char* vertexMemory, const char* fragmentMemory);
	bool free(ShaderObject* shaderObject);
private:
	std::vector<ShaderObject*> shaderObjectList;
};

#endif /*SHADERMANAGER_H_*/
