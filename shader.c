#include <stdio.h>
#include <malloc.h>
#include "shader.h"

int shader_init(struct shader *s)
{
	s->prog = glCreateProgram();
	if (!s->prog)
		return -1;

	s->vs = glCreateShader(GL_VERTEX_SHADER);
	if (!s->vs)
		return -1;

	s->fs = glCreateShader(GL_FRAGMENT_SHADER);
	if (!s->fs)
		return -1;

	return 0;
}

void shader_deinit(struct shader *s)
{
	glDetachShader(s->prog, s->vs);
	glDetachShader(s->prog, s->fs);
	glDeleteShader(s->vs);
	glDeleteShader(s->fs);
	glDeleteProgram(s->prog);
}

int shader_read_file(struct shader *s, GLenum type, const char *path)
{
	size_t len;
	char *src;
	FILE *f;
	int err = 0;

	f = fopen(path, "r");
	if (!f)
		return -1;
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	src = malloc(len + 1);
	if (!src)
		return -1;
	fseek(f, 0, 0);
	fread(src, 1, len, f);
	src[len] = 0;
	err = shader_read_string(s, type, src);
	free(src);
	fclose(f);

	return err;
}

int shader_read_string(struct shader *s, GLenum type, const char *str)
{
	switch (type) {
	case GL_VERTEX_SHADER:
		glShaderSource(s->vs, 1, &str, 0);
		return 0;
	case GL_FRAGMENT_SHADER:
		glShaderSource(s->fs, 1, &str, 0);
		return 0;
	default:
		return -1;
	}
}

static int compile_gl_shader(GLint sh)
{
	GLint st;
	char *msg;

	glCompileShader(sh);
	glGetShaderiv(sh, GL_COMPILE_STATUS, &st);

	if (st != GL_TRUE) {
		glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &st);
		msg = malloc(st + 1);
		glGetShaderInfoLog(sh, st, &st, msg);
		msg[st] = 0;
		printf("%s\n", msg);
		free(msg);
		getchar();
		return -1;
	}

	return 0;
}

int shader_compile(struct shader *s)
{
	if (compile_gl_shader(s->vs) < 0)
		return -1;

	if (compile_gl_shader(s->fs) < 0)
		return -1;

	return 0;
}

int shader_link(struct shader *s)
{
	glBindAttribLocation(s->prog, 0, "Position");
	glBindAttribLocation(s->prog, 1, "Normal");
	glBindAttribLocation(s->prog, 2, "Texcoord");

	glAttachShader(s->prog, s->vs);
	glAttachShader(s->prog, s->fs);
	glLinkProgram(s->prog);

	return 0;
}
