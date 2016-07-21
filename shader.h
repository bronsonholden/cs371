#if !defined(SHADER_H)
#define SHADER_H

#include "GL/glew.h"

struct shader
{
	GLuint prog;
	GLuint vs;
	GLuint fs;
};

int shader_init(struct shader *s);

void shader_deinit(struct shader *s);

int shader_read_file(struct shader *s, GLenum type, const char *path);

int shader_read_string(struct shader *s, GLenum type, const char *str);

int shader_compile(struct shader *s);

int shader_link(struct shader *s);

#endif /* !defined(SHADER_H) */
