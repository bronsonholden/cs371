#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "GL/glew.h"

enum FRAMEBUFFER_ATTACHMENT
{
	POSITION_ATTACHMENT = 0,
	COLOR_ATTACHMENT,
	NORMAL_ATTACHMENT,
	TEXCOORD_ATTACHMENT,
	VELOCITY_ATTACHMENT,
	NUM_FRAMEBUFFER_ATTACHMENTS
};

struct framebuffer
{
	GLuint fbo;
	GLuint fba[NUM_FRAMEBUFFER_ATTACHMENTS];
	GLuint depth;
	GLuint fin[2];
};

int framebuffer_init(struct framebuffer *fb);

void framebuffer_deinit(struct framebuffer *fb);

void framebuffer_bind_geom(struct framebuffer *fb);

void framebuffer_bind_light(struct framebuffer *fb);

void framebuffer_bind_stencil(struct framebuffer *fb);

void framebuffer_bind_final(struct framebuffer *fb);

#endif /* FRAMEBUFFER_H */
