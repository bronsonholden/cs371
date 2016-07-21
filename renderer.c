#include <stdio.h>
#include <string.h>
#include <math.h>
#include "cube.h"
#include "renderer.h"

static int renderer_init_shaders(struct renderer *r);

static int init_shader(struct shader *sh, const char *vp, const char *fp);

/* TODO move this function */
static int create_quad_mesh(GLuint *attr);

static const GLenum drawbuffers[] = {
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1,
	GL_COLOR_ATTACHMENT2,
	GL_COLOR_ATTACHMENT3,
	GL_COLOR_ATTACHMENT4
};

int renderer_init(struct renderer *r, int w, int h)
{
	if (framebuffer_init(&r->fb) < 0)
		return -1;
	if (renderer_init_shaders(r) < 0)
		return -1;
	if (create_cube_mesh(&r->cube_attr, &r->cube_idx) < 0) {
		fprintf(stderr, "Error creating cube mesh\n");
		return -1;
	}
	if (create_quad_mesh(&r->quad_attr) < 0) {
		fprintf(stderr, "Error creating fullscreen quad\n");
		return -1;
	}

	vec3_set(&r->pos, 2, 4, 2);
	vec3_set(&r->tar, 0, 0, 0);
	vec3_set(&r->up, 0, 1, 0);
	r->width = w;
	r->height = h;
	renderer_default_display_state(r);
	renderer_default_quadrants(r);
	renderer_default_postprocess_effects(r);

	return 0;
}

void renderer_deinit(struct renderer *r)
{
	framebuffer_deinit(&r->fb);
	shader_deinit(&r->geom);
	shader_deinit(&r->null);
	shader_deinit(&r->light);
	shader_deinit(&r->negative);
	shader_deinit(&r->textured);
	glDeleteBuffers(1, &r->cube_attr);
	glDeleteBuffers(1, &r->cube_idx);
	glDeleteBuffers(1, &r->quad_attr);
}

void renderer_prerender(struct renderer *r)
{
	struct mat4 tmp;

	mat4_lookat(&r->view, &r->pos, &r->tar, &r->up);
	mat4_persp(
		&r->proj,
		70.0f,
		(float)(r->width) / r->height,
		0.1f,
		500.0f
	);
	mat4_id(&r->model);
	mat4_multiply(&tmp, &r->proj, &r->view);
	memcpy(&r->prev_mvp, &r->mvp, sizeof(r->mvp));
	mat4_multiply(&r->mvp, &tmp, &r->model);
}

void renderer_bind_geometry(struct renderer *r)
{
	struct mat4 tmp;
	GLuint prog;

	prog = r->geom.prog;
	/* Prepare for rendering geometry */
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, r->fb.fbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + NUM_FRAMEBUFFER_ATTACHMENTS);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawBuffers(
		sizeof(drawbuffers) / sizeof(drawbuffers[0]),
		drawbuffers
	);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	mat4_multiply(&tmp, &r->proj, &r->view);

	/* Set up shader variables */
	glUseProgram(prog);
	glUniformMatrix4fv(
		glGetUniformLocation(prog, "ModelMatrix"),
		1,
		GL_FALSE,
		r->model.a
	);
	mat4_multiply(&tmp, &r->proj, &r->view);
	glUniformMatrix4fv(
		glGetUniformLocation(prog, "ViewProjectionMatrix"),
		1,
		GL_FALSE,
		tmp.a
	);

	glUniformMatrix4fv(
		glGetUniformLocation(prog, "PreviousMVP"),
		1,
		GL_FALSE,
		r->prev_mvp.a
	);
	
	glBindBuffer(GL_ARRAY_BUFFER, r->cube_attr);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->cube_idx);
	/* Position attrib */
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(struct attribs),
		0
	);
	/* Normal attrib */
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(struct attribs),
		(GLvoid *)(sizeof(struct vec3))
	);
	/* Vertex color attrib */
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(struct attribs),
		(GLvoid *)(sizeof(struct vec3) * 2)
	);
	/* Texcoord attrib */
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(
		3,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(struct attribs),
		(GLvoid *)(sizeof(struct vec3) * 3)
	);
}

void renderer_bind_stencil(struct renderer *r)
{
	GLuint prog;

	prog = r->null.prog;
	glDepthMask(GL_FALSE);
	glEnable(GL_STENCIL_TEST);
	glDrawBuffer(GL_NONE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_ALWAYS, 0, 0);
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

	glUseProgram(prog);
	glUniformMatrix4fv(
		glGetUniformLocation(prog, "ModelViewProjectionMatrix"),
		1,
		GL_FALSE,
		r->mvp.a
	);
	
	glBindBuffer(GL_ARRAY_BUFFER, r->cube_attr);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->cube_idx);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(struct attribs),
		0
	);
}

void renderer_bind_light(struct renderer *r)
{
	GLuint prog;

	prog = r->light.prog;
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + NUM_FRAMEBUFFER_ATTACHMENTS);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, r->fb.fba[POSITION_ATTACHMENT]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, r->fb.fba[COLOR_ATTACHMENT]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, r->fb.fba[NORMAL_ATTACHMENT]);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, r->fb.fba[TEXCOORD_ATTACHMENT]);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, r->fb.fba[VELOCITY_ATTACHMENT]);
	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glUseProgram(prog);
	glUniform1i(
		glGetUniformLocation(prog, "PositionMap"),
		0
	);
	glUniform1i(
		glGetUniformLocation(prog, "ColorMap"),
		1
	);
	glUniform1i(
		glGetUniformLocation(prog, "NormalMap"),
		2
	);
	glUniform1i(
		glGetUniformLocation(prog, "TexcoordMap"),
		3
	);
	glUniform1i(
		glGetUniformLocation(prog, "VelocityMap"),
		4
	);
	glUniformMatrix4fv(
		glGetUniformLocation(prog, "ModelViewProjectionMatrix"),
		1,
		GL_FALSE,
		r->mvp.a
	);
	glUniform2f(
		glGetUniformLocation(prog, "ScreenDimensions"),
		r->width,
		r->height
	);
	glUniform3fv(
		glGetUniformLocation(prog, "EyePosition"),
		1,
		&r->pos.x
	);
	
	glBindBuffer(GL_ARRAY_BUFFER, r->cube_attr);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->cube_idx);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(struct attribs),
		0
	);
}

void renderer_bind_final(struct renderer *r)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, r->fb.fbo);
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void renderer_bind_postprocess(struct renderer *r)
{
	glDisable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);
	r->swap = 0;
}

void renderer_geometry_pass(struct renderer *r)
{
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
}

void renderer_stencil_pass(struct renderer *r)
{
	glBindBuffer(GL_ARRAY_BUFFER, r->cube_attr);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->cube_idx);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
}

void renderer_light_pass(struct renderer *r)
{
	/* TODO move this. create a struct for it or something */
	static struct vec3 light_pos, light_color;
	static float light_amb, light_diff, light_exp, light_lin, light_cst;
	GLuint prog;
	static float val = 0;

	val += 0.012f;
	light_color.x = 1;
	light_color.y = 1;
	light_color.z = 1;
	light_pos.x = (float)cos(val) * 2.0f;
	light_pos.y = 2;
	light_pos.z = (float)sin(val) * 2.0f;
	light_amb = 0;
	light_diff = 0.9f;
	light_exp = 0.008f;
	light_lin = 0.08f;
	light_cst = 1.0f;

	prog = r->light.prog;
	glBindBuffer(GL_ARRAY_BUFFER, r->cube_attr);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->cube_idx);
	/* per object uniforms */
	glUniform3fv(
		glGetUniformLocation(prog, "Light.pos"),
		1,
		&light_pos.x
	);
	glUniform3fv(
		glGetUniformLocation(prog, "Light.color"),
		1,
		&light_color.x
	);
	glUniform1f(
		glGetUniformLocation(prog, "Light.amb"),
		light_amb
	);
	glUniform1f(
		glGetUniformLocation(prog, "Light.diff"),
		light_diff
	);
	glUniform1f(
		glGetUniformLocation(prog, "Light.exp"),
		light_exp
	);
	glUniform1f(
		glGetUniformLocation(prog, "Light.lin"),
		light_lin
	);
	glUniform1f(
		glGetUniformLocation(prog, "Light.cst"),
		light_cst
	);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
}

void renderer_final_full(struct renderer *r)
{
	glReadBuffer(
		GL_COLOR_ATTACHMENT0 + NUM_FRAMEBUFFER_ATTACHMENTS + r->swap
	);
	glBlitFramebuffer(
		0, 0, r->width, r->height,
		0, 0, r->width, r->height,
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST
	);
}

void renderer_final_splice(struct renderer *r)
{
	int w, h;
	int hw, hh;

	w = r->width;
	h = r->height;
	hw = w / 2;
	hh = h / 2;

	glReadBuffer(r->quadrants[0]);
	glBlitFramebuffer(
		0, 0, hw, hh,
		0, 0, hw, hh,
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST
	);
	glReadBuffer(r->quadrants[1]);
	glBlitFramebuffer(
		hw, 0, w, hh,
		hw, 0, w, hh,
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST
	);
	glReadBuffer(r->quadrants[2]);
	glBlitFramebuffer(
		0, hh, hw, h,
		0, hh, hw, h,
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST
	);
	glReadBuffer(r->quadrants[3]);
	glBlitFramebuffer(
		hw, hh, w, h,
		hw, hh, w, h,
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST
	);
}

void renderer_final_split(struct renderer *r)
{
	int w, h;
	int hw, hh;

	w = r->width;
	h = r->height;
	hw = w / 2;
	hh = h / 2;

	glReadBuffer(r->quadrants[0]);
	glBlitFramebuffer(
		0, 0, w, h,
		0, 0, hw, hh,
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST
	);
	glReadBuffer(r->quadrants[1]);
	glBlitFramebuffer(
		0, 0, w, h,
		hw, 0, w, hh,
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST
	);
	glReadBuffer(r->quadrants[2]);
	glBlitFramebuffer(
		0, 0, w, h,
		0, hh, hw, h,
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST
	);
	glReadBuffer(r->quadrants[3]);
	glBlitFramebuffer(
		0, 0, w, h,
		hw, hh, w, h,
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST
	);
}

void renderer_final_pass(struct renderer *r)
{
	switch (r->state) {
	case RENDERER_DISPLAY_STATE_FULL:
		renderer_final_full(r);
		break;
	case RENDERER_DISPLAY_STATE_SPLICE:
		renderer_final_splice(r);
		break;
	case RENDERER_DISPLAY_STATE_SPLIT:
		renderer_final_split(r);
		break;
	default:
		;
	}
}

void renderer_negative_pass(struct renderer *r)
{
	int off = r->swap ^ 1;

	if (!(r->pproc & POSTPROCESS_NEGATIVE_BIT))
		return;

	glUseProgram(r->negative.prog);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, r->fb.fin[r->swap]);
	glUniform1i(
		glGetUniformLocation(r->negative.prog, "ColorMap"),
		0
	);
	glUniform2f(
		glGetUniformLocation(r->negative.prog, "ScreenDimensions"),
		r->width,
		r->height
	);
	glDrawBuffer(
		GL_COLOR_ATTACHMENT0 + NUM_FRAMEBUFFER_ATTACHMENTS + off
	);
	glBindBuffer(GL_ARRAY_BUFFER, r->quad_attr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		0
	);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	r->swap ^= 1;
}

void renderer_textured_pass(struct renderer *r)
{
	int off = r->swap ^ 1;

	if (!(r->pproc & POSTPROCESS_TEXTURED_BIT))
		return;

	glUseProgram(r->textured.prog);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, r->fb.fin[r->swap]);
	glUniform1i(
		glGetUniformLocation(r->textured.prog, "FinalMap"),
		0
	);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, r->fb.fba[COLOR_ATTACHMENT]);
	glUniform1i(
		glGetUniformLocation(r->textured.prog, "ColorMap"),
		1
	);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, r->fb.fba[TEXCOORD_ATTACHMENT]);
	glUniform1i(
		glGetUniformLocation(r->textured.prog, "TexcoordMap"),
		2
	);
	glUniform2f(
		glGetUniformLocation(r->textured.prog, "ScreenDimensions"),
		r->width,
		r->height
	);

	glDrawBuffer(
		GL_COLOR_ATTACHMENT0 + NUM_FRAMEBUFFER_ATTACHMENTS + off
	);
	glBindBuffer(GL_ARRAY_BUFFER, r->quad_attr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		0
	);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	r->swap ^= 1;
}

void renderer_default_postprocess_effects(struct renderer *r)
{
	r->pproc = 0;
}

void renderer_default_display_state(struct renderer *r)
{
	r->state = RENDERER_DISPLAY_STATE_FULL;
}

void renderer_default_quadrants(struct renderer *r)
{
	int i;

	for (i = 0; i < 4; ++i)
		r->quadrants[i] = GL_COLOR_ATTACHMENT0 + i;
}

int renderer_init_shaders(struct renderer *r)
{
	if (
		init_shader(
			&r->geom,
			"shader/geom_vert.glsl",
			"shader/geom_frag.glsl"
		) < 0
	)
	{
		fprintf(stderr, "Error initializing geometry shader\n");
		return -1;
	}
	if (
		init_shader(
			&r->null,
			"shader/null_vert.glsl",
			"shader/null_frag.glsl"
		) < 0
	)
	{
		fprintf(stderr, "Error initializing null shader\n");
		return -1;
	}
	if (
		init_shader(
			&r->light,
			"shader/light_vert.glsl",
			"shader/light_frag.glsl"
		) < 0
	)
	{
		fprintf(stderr, "Error initializing light shader\n");
		return -1;
	}
	if (
		init_shader(
			&r->negative,
			"shader/postprocess_vert.glsl",
			"shader/negative_frag.glsl"
		) < 0
	)
	{
		fprintf(stderr, "Error initializing negative shader\n");
		return -1;
	}
	if (
		init_shader(
			&r->textured,
			"shader/postprocess_vert.glsl",
			"shader/textured_frag.glsl"
		) < 0
	)
	{
		fprintf(stderr, "Error initializing textured shader\n");
		return -1;
	}

	return 0;
}

int create_quad_mesh(GLuint *attr)
{
	const GLfloat quad[] = {
		-1, -1, 0,
		-1, 1, 0,
		1, 1, 0,
		-1, -1, 0,
		1, 1, 0,
		1, -1, 0
	};
	if (!attr)
		return -1;

	glGenBuffers(1, attr);
	glBindBuffer(GL_ARRAY_BUFFER, *attr);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(quad),
		quad,
		GL_STATIC_DRAW
	);

	return 0;
}

int init_shader(struct shader *sh, const char *vp, const char *fp)
{
	if (shader_init(sh) < 0)
		return -1;
	if (shader_read_file(sh, GL_VERTEX_SHADER, vp) < 0)
		return -1;
	if (shader_read_file(sh, GL_FRAGMENT_SHADER, fp) < 0)
		return -1;
	if (shader_compile(sh) < 0 || shader_link(sh) < 0)
		return -1;

	return 0;
}
