#ifndef MGL_H
#define MGL_H

#ifndef __GLEW_H__
#include <GL/glut.h>
#include <stddef.h>

typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLboolean;
typedef signed char GLbyte;
typedef short GLshort;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned long GLulong;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;
typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;

typedef GLuint (*pnfglCreateShader)(GLenum shaderType);
typedef void (*pfnglShaderSource)(GLuint shader,
								  GLsizei count,
								  const GLchar **string,
								  const GLint *length);
typedef void (*pfnglCompileShader)(GLuint shader);
typedef void (*pfnglGetShaderiv)(GLuint shader,
								 GLenum pname,
								 GLint *params);
typedef void (*pfnglGetShaderInfoLog)(GLuint shader,
									  GLsizei maxLength,
									  GLsizei *length,
									  GLchar *infoLog);
typedef GLuint (*pfnglCreateProgram)(void);
typedef void (*pfnglAttachShader)(GLuint program,
								  GLuint shader);
typedef void (*pfnglLinkProgram)(GLuint program);
typedef void (*pfnglGetProgramiv)(GLuint program,
								  GLenum pname,
								  GLint *params);
typedef void (*pfnglGetProgramInfoLog)(GLuint program,
									   GLsizei maxLength,
									   GLsizei *length,
									   GLchar *infoLog);
typedef void (*pfnglDeleteShader)(GLuint shader);
typedef void (*pfnglUseProgram)(GLuint program);
typedef void (*pfnglGenVertexArrays)(GLsizei n,
									 GLuint *arrays);
typedef void (*pfnglGenBuffers)(GLsizei n,
								GLuint * buffers);
typedef void (*pfnglBindBuffer)(GLenum target,
								GLuint buffer); 
typedef void (*pfnglBufferData)(GLenum target,
								GLsizeiptr size,
								const GLvoid * data,
								GLenum usage);
typedef void (*pfnglBindVertexArray)(GLuint array);
typedef void (*pfnglEnableVertexAttribArray)(GLuint index);
typedef void (*pfnglVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
typedef GLint (*pfnglGetUniformLocation)(GLuint program,
										 const GLchar *name);


typedef void (*pfnglUniform1i)(GLint location,
							   GLint v0);

typedef void (*pfnglUniformMatrix4fv)(GLint location,
									  GLsizei count,
									  GLboolean transpose,
									  const GLfloat *value);

typedef void (*pfnglUniform3fv)(GLint location,
								GLsizei count,
								const GLfloat *value);
pnfglCreateShader glCreateShader;
pfnglShaderSource glShaderSource;								
pfnglCompileShader glCompileShader;
pfnglGetShaderiv glGetShaderiv;
pfnglGetShaderInfoLog glGetShaderInfoLog;
pfnglCreateProgram glCreateProgram;
pfnglAttachShader glAttachShader;
pfnglLinkProgram glLinkProgram;
pfnglGetProgramiv glGetProgramiv;
pfnglGetProgramInfoLog glGetProgramInfoLog;
pfnglDeleteShader glDeleteShader;

pfnglUseProgram glUseProgram;
pfnglGenVertexArrays glGenVertexArrays;
pfnglGenBuffers glGenBuffers;
pfnglBindBuffer glBindBuffer;
pfnglBufferData glBufferData;
pfnglBindVertexArray glBindVertexArray;
pfnglEnableVertexAttribArray glEnableVertexAttribArray;
pfnglVertexAttribPointer glVertexAttribPointer;
pfnglGetUniformLocation glGetUniformLocation;

pfnglUniform1i glUniform1i;
pfnglUniformMatrix4fv glUniformMatrix4fv;
pfnglUniform3fv glUniform3fv;

// Marre.

#define defGLFun(ret, name, args)				\
typedef ret (*pfn##name)args;					\
pfn##name name

defGLFun(void, glDeleteProgram, (GLuint));
defGLFun(void, glDeleteVertexArrays, (GLsizei, const GLuint *));

int glInit();
#endif
#endif /* MGL_H */
