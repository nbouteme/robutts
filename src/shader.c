#include <robutts.h>
#include <assert.h>
#include <assets.h>

int load_shader(const char *filename, int type) {
	unsigned s;
	const char *f;
	char *log;
	int success = 1;
	int size;

	s = glCreateShader(type);
	f = readfile(filename, 0);
	glShaderSource(s, 1, &f, NULL);
	glCompileShader(s);
	
	glGetShaderiv(s, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderiv(s, GL_INFO_LOG_LENGTH, &size);
		assert(size != 0);
		log = calloc(1, size);
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
	char *log;

	s = glCreateProgram();
	glAttachShader(s, v);
	glAttachShader(s, f);
	glLinkProgram(s);
	glGetProgramiv(s, GL_LINK_STATUS, &success);
	if (!success)
	{
		log = calloc(1, 512);
		glGetProgramInfoLog(s, 512, NULL, log);
		fputs(log, stderr);
	}
	assert(success);
	glDeleteShader(v);
	glDeleteShader(f);
	return s;
}
