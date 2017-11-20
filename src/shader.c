#include <robutts.h>
#include <assert.h>

int load_shader(const char *filename, int type) {
	unsigned s;

	s = glCreateShader(type);
	const char *f = readfile(filename, 0);
	glShaderSource(s, 1, &f, NULL);
	glCompileShader(s);
	int success = 1;
	glGetShaderiv(s, GL_COMPILE_STATUS, &success);
	if (!success) {
		int size;
		glGetShaderiv(s, GL_INFO_LOG_LENGTH, &size);
		assert(size != 0);
		char *log = calloc(1, size);
		glGetShaderInfoLog(s, size, 0, log);
		fputs(log, stderr);
		abort();
	};
	free((void*)f);
	return s;
}

int make_shader(unsigned v, unsigned f) {
	unsigned int s;
	int success;

	s = glCreateProgram();
	glAttachShader(s, v);
	glAttachShader(s, f);
	glLinkProgram(s);
	glGetProgramiv(s, GL_LINK_STATUS, &success);
	if (!success)
	{
		char *log = calloc(1, 512);
		glGetProgramInfoLog(s, 512, NULL, log);
		fputs(log, stderr);
	}
	assert(success);
	glDeleteShader(v);
	glDeleteShader(f);
	return s;
}
