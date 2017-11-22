#include <mgl.h>

void *glutGetProcAddress(const char *procName);

#define INIT_FUN(x) \
	if (!(x = glutGetProcAddress(#x))) return 0	\

int glInit() {
	INIT_FUN(glCreateShader);
	INIT_FUN(glShaderSource);								
	INIT_FUN(glCompileShader);
	INIT_FUN(glGetShaderiv);
	INIT_FUN(glGetShaderInfoLog);
	INIT_FUN(glCreateProgram);
	INIT_FUN(glDeleteProgram);
	INIT_FUN(glDeleteVertexArrays);
	INIT_FUN(glAttachShader);
	INIT_FUN(glLinkProgram);
	INIT_FUN(glGetProgramiv);
	INIT_FUN(glGetProgramInfoLog);
	INIT_FUN(glDeleteShader);	
	INIT_FUN(glUseProgram);
	INIT_FUN(glGenVertexArrays);
	INIT_FUN(glGenBuffers);
	INIT_FUN(glBindBuffer);
	INIT_FUN(glBufferData);
	INIT_FUN(glBindVertexArray);
	INIT_FUN(glEnableVertexAttribArray);
	INIT_FUN(glVertexAttribPointer);
	INIT_FUN(glGetUniformLocation);
	INIT_FUN(glUniform1i);
	INIT_FUN(glUniformMatrix4fv);
	INIT_FUN(glUniform3fv);
	INIT_FUN(glUniform1fv);
	INIT_FUN(glUniform1iv);
	return 1;
}