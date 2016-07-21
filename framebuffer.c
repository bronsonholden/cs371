#include "framebuffer.h"

static const GLenum drawbuffers[] = {
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1,
	GL_COLOR_ATTACHMENT2,
	GL_COLOR_ATTACHMENT3,
	GL_COLOR_ATTACHMENT4
};

static void framebuffer_attachment_parameters(
	GLenum target,
	enum FRAMEBUFFER_ATTACHMENT i
	)
{
	switch (i) {
	case POSITION_ATTACHMENT:
	case COLOR_ATTACHMENT:
	case NORMAL_ATTACHMENT:
	case TEXCOORD_ATTACHMENT:
	case VELOCITY_ATTACHMENT:
		glTexParameteri(
			target,
			GL_TEXTURE_MIN_FILTER,
			GL_NEAREST
		);
		glTexParameteri(
			target,
			GL_TEXTURE_MAG_FILTER,
			GL_NEAREST
		);
		glTexParameteri(
			target,
			GL_TEXTURE_WRAP_S,
			GL_CLAMP
		);
		glTexParameteri(
			target,
			GL_TEXTURE_WRAP_T,
			GL_CLAMP
		);
		break;
	default:
		;
	}
}

static int framebuffer_init_attachments(struct framebuffer *fb)
{
	int i;

	glGenTextures(NUM_FRAMEBUFFER_ATTACHMENTS, fb->fba);
	glGenTextures(1, &fb->depth);
	glGenTextures(2, fb->fin);
	for (i = 0; i < NUM_FRAMEBUFFER_ATTACHMENTS; ++i) {
		glBindTexture(GL_TEXTURE_2D, fb->fba[i]);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGB32F,
			800,
			600,
			0,
			GL_RGB,
			GL_FLOAT,
			0
		);
		framebuffer_attachment_parameters(GL_TEXTURE_2D, i);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0 + i,
			GL_TEXTURE_2D,
			fb->fba[i],
			0
		);
	}

	glBindTexture(GL_TEXTURE_2D, fb->depth);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_DEPTH24_STENCIL8,
		800,
		600,
		0,
		GL_DEPTH_STENCIL,
		GL_UNSIGNED_INT_24_8,
		0
	);
	glFramebufferTexture2D(
		GL_DRAW_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D,
		fb->depth,
		0
	);

	glBindTexture(GL_TEXTURE_2D, fb->fin[0]);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA32F,
		800,
		600,
		0,
		GL_RGBA,
		GL_FLOAT,
		0
	);
	framebuffer_attachment_parameters(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(
		GL_DRAW_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0 + NUM_FRAMEBUFFER_ATTACHMENTS,
		GL_TEXTURE_2D,
		fb->fin[0],
		0
	);
	glBindTexture(GL_TEXTURE_2D, fb->fin[1]);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA32F,
		800,
		600,
		0,
		GL_RGBA,
		GL_FLOAT,
		0
	);
	framebuffer_attachment_parameters(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(
		GL_DRAW_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0 + NUM_FRAMEBUFFER_ATTACHMENTS + 1,
		GL_TEXTURE_2D,
		fb->fin[1],
		0
	);

	return 0;
}

int framebuffer_init(struct framebuffer *fb)
{
	int err;

	glGenFramebuffers(1, &fb->fbo);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb->fbo);

	if (framebuffer_init_attachments(fb) < 0)
		return -1;
	glDrawBuffers(sizeof(drawbuffers) / sizeof(*drawbuffers), drawbuffers);
	err = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (err != GL_FRAMEBUFFER_COMPLETE)
		return -1;
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	return 0;
}

void framebuffer_deinit(struct framebuffer *fb)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glDeleteTextures(NUM_FRAMEBUFFER_ATTACHMENTS, fb->fba);
	glDeleteTextures(1, &fb->depth);
	glDeleteTextures(2, fb->fin);
	glDeleteFramebuffers(1, &fb->fbo);
}

void framebuffer_bind_geom(struct framebuffer *fb)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb->fbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + NUM_FRAMEBUFFER_ATTACHMENTS);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawBuffers(
		sizeof(drawbuffers) / sizeof(drawbuffers[0]),
		drawbuffers
	);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
}

void framebuffer_bind_light(struct framebuffer *fb)
{
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + NUM_FRAMEBUFFER_ATTACHMENTS);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(
		GL_TEXTURE_2D,
		fb->fba[POSITION_ATTACHMENT]
	);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(
		GL_TEXTURE_2D,
		fb->fba[COLOR_ATTACHMENT]
	);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(
		GL_TEXTURE_2D,
		fb->fba[NORMAL_ATTACHMENT]
	);

	glStencilFunc(GL_NOTEQUAL, 0, 1);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_FRONT);
}

void framebuffer_bind_stencil(struct framebuffer *fb)
{
	glDepthMask(GL_FALSE);
	glDrawBuffer(GL_NONE);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_ALWAYS, 0, 0);
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR, GL_KEEP);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_INCR, GL_KEEP);
}

void framebuffer_bind_final(struct framebuffer *fb)
{
	glCullFace(GL_BACK);
	glDisable(GL_BLEND);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, fb->fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
}
