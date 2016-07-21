#ifndef RENDERER_H
#define RENDERER_H

#include "GL/glew.h"
#include "shader.h"
#include "framebuffer.h"
#include "mat4.h"
#include "vec3.h"

enum RENDERER_DISPLAY_STATE
{
	/* Display the final image on the whole screen */
	RENDERER_DISPLAY_STATE_FULL = 0,
	/* Display spliced image, each quadrant shows contents of a
	 * different buffer.
	 */
	RENDERER_DISPLAY_STATE_SPLICE,
	/* Display whole scene in different quadrants, each showing contents
	 * of different buffers.
	 */
	RENDERER_DISPLAY_STATE_SPLIT,
	NUM_RENDERER_DISPLAY_STATES
};

#define POSTPROCESS_NEGATIVE_BIT          0x01
#define POSTPROCESS_XINVERT_BIT           0x02
#define POSTPROCESS_YINVERT_BIT           0x04
#define POSTPROCESS_TEXTURED_BIT          0x08

struct renderer
{
	struct framebuffer fb;
	/* bitmask enbaling certain post-process effects */
	int pproc;
	int state;
	/* Used for SPLICE and SPLIT display states; which color attachments
	 * are being displayed.
	 */
	GLenum quadrants[4];
	/* Post process swap. 0 or 1, tracks which attachment to sample from
	 * and render to.
	 */
	GLuint swap;
	/* Shapes buffers, for rendering our scene */
	GLuint cube_attr;
	GLuint cube_idx;
	GLuint quad_attr;
	/* primary shaders */
	struct shader geom;
	struct shader null;
	struct shader light;
	/* post-process shaders */
	struct shader negative;
	struct shader textured;
	/* matrices */
	struct mat4 mvp;
	struct mat4 prev_mvp;
	struct mat4 model;
	struct mat4 view;
	struct mat4 proj;
	/* lookat parameters */
	struct vec3 pos;
	struct vec3 tar;
	struct vec3 up;
	/* window/framebuffer size */
	GLuint width;
	GLuint height;
};

int renderer_init(struct renderer *r, int w, int h);
void renderer_deinit(struct renderer *r);
void renderer_prerender(struct renderer *r);
void renderer_bind_geometry(struct renderer *r);
void renderer_bind_stencil(struct renderer *r);
void renderer_bind_light(struct renderer *r);
void renderer_bind_final(struct renderer *r);
void renderer_bind_postprocess(struct renderer *r);
void renderer_geometry_pass(struct renderer *r);
void renderer_stencil_pass(struct renderer *r);
void renderer_light_pass(struct renderer *r);
void renderer_final_pass(struct renderer *r);
void renderer_negative_pass(struct renderer *r);
void renderer_textured_pass(struct renderer *r);
void renderer_default_postprocess_effects(struct renderer *r);
void renderer_default_display_state(struct renderer *r);
void renderer_default_quadrants(struct renderer *r);

#endif /* RENDERER_H */
