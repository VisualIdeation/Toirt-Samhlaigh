#ifndef SHADEROBJECT_H_
#define SHADEROBJECT_H_

#include <iostream>
#include <vector>

#include <GL/gl.h>

#include <SHADER/ShaderProgram.h>

#define CHECK_GL_ERROR() CheckGLError(__FILE__, __LINE__)

class ShaderObject {
public:
	ShaderObject(void);
	~ShaderObject(void);
	void addShader(ShaderProgram* shaderProgram);
	void begin(void);
	int CheckGLError(const char * file, int line);
	void end(void);
	const char * getLinkerLog(void);
	void getUniformfv(const char * name, GLfloat* values);
	void getUniformiv(const char * name, GLint* values);
	bool glslEnabled(void);
	bool link(void);
	bool sendUniform1f(const char * name, GLfloat value);
	bool sendUniform2f(const char * name, GLfloat value0, GLfloat value1);
	bool sendUniform3f(const char * name, GLfloat value0, GLfloat value1, GLfloat value2);
	bool sendUniform4f(const char * name, GLfloat value0, GLfloat value1, GLfloat value2, GLfloat value3);
	bool sendUniform1i(const char * name, GLint value);
	bool sendUniform2i(const char * name, GLint value0, GLint value1);
	bool sendUniform3i(const char * name, GLint value0, GLint value1, GLint value2);
	bool sendUniform4i(const char * name, GLint value0, GLint value1, GLint value2, GLint value3);
	bool sendUniform1fv(const char * name, GLsizei count, GLfloat *values);
	bool sendUniform2fv(const char * name, GLsizei count, GLfloat *values);
	bool sendUniform3fv(const char * name, GLsizei count, GLfloat *values);
	bool sendUniform4fv(const char * name, GLsizei count, GLfloat *values);
	bool sendUniform1iv(const char * name, GLsizei count, GLint *values);
	bool sendUniform2iv(const char * name, GLsizei count, GLint *values);
	bool sendUniform3iv(const char * name, GLsizei count, GLint *values);
	bool sendUniform4iv(const char * name, GLsizei count, GLint *values);
	bool sendUniformMatrix2fv(const char * name, GLsizei count, GLboolean transpose, GLfloat *values);
	bool sendUniformMatrix3fv(const char * name, GLsizei count, GLboolean transpose, GLfloat *values);
	bool sendUniformMatrix4fv(const char * name, GLsizei count, GLboolean transpose, GLfloat *values);
	void setManageMemory(void);
private:
	GLhandleARB shaderObject;
	GLcharARB* linkerLog;
	bool linked;
	bool manageMemory;
	std::vector<ShaderProgram*> shaderList;
	GLint getUniLoc(const GLcharARB *name);
};

#endif /*SHADEROBJECT_H_*/
