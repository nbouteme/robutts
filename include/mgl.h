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

// Marre.
#define defGLFun(ret, name, args)				\
typedef ret (*pfn##name)args;					\
pfn##name name

defGLFun(void, glDeleteProgram, (GLuint));
defGLFun(void, glDeleteVertexArrays, (GLsizei, const GLuint *));
defGLFun(void, glUniform1fv, (GLint, GLsizei, const GLfloat *));
defGLFun(void, glUniform1iv, (GLint, GLsizei, const GLint *));
defGLFun(GLuint, glCreateShader, (GLenum shaderType));
defGLFun(void, glShaderSource, (GLuint shader, GLsizei count, const GLchar **string, const GLint *length));
defGLFun(void, glCompileShader, (GLuint shader));
defGLFun(void, glGetShaderiv, (GLuint shader, GLenum pname, GLint *params));
defGLFun(void, glGetShaderInfoLog, (GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog));
defGLFun(GLuint, glCreateProgram, (void));
defGLFun(void, glAttachShader, (GLuint program, GLuint shader));
defGLFun(void, glLinkProgram, (GLuint program));
defGLFun(void, glLinkProgram, (GLuint program));
defGLFun(void, glGetProgramiv, (GLuint program, GLenum pname, GLint *params));
defGLFun(void, glGetProgramInfoLog, (GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog));
defGLFun(void, glDeleteShader, (GLuint shader));
defGLFun(void, glUseProgram, (GLuint program));
defGLFun(void, glGenVertexArrays, (GLsizei n, GLuint *arrays));
defGLFun(void, glGenBuffers, (GLsizei n, GLuint * buffers));
defGLFun(void, glBindBuffer, (GLenum target, GLuint buffer)); 
defGLFun(void, glBufferData, (GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage));
defGLFun(void, glBindVertexArray, (GLuint array));
defGLFun(void, glEnableVertexAttribArray, (GLuint index));
defGLFun(void, glVertexAttribPointer, (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer));
defGLFun(GLint, glGetUniformLocation, (GLuint program, const GLchar *name));
defGLFun(void, glUniform1i, (GLint location, GLint v0));
defGLFun(void, glUniformMatrix4fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value));
defGLFun(void, glUniform3fv, (GLint location, GLsizei count, const GLfloat *value));

int glInit();
#endif
#endif /* MGL_H */
