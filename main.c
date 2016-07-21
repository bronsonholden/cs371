/* Paul Holden
 * CS 371
 * Computer Graphics
 * Fall 2014
 * Final Project: Post-Processing
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GL/glew.h"
#include "framebuffer.h"
#include "shader.h"
#include "mat4.h"
#include "cube.h"
#include "renderer.h"
#ifdef __GNUC__
#include "window.h"
#else
#include "glfw-3.0.3/include/GLFW/glfw3.h"
#endif

#ifdef __GNUC__
static struct window win;
#else
GLFWwindow *win;
#endif

struct renderer R;

#define CHECK_EXT(ext) \
	do { \
		if (!glewIsSupported(ext)) { \
			fprintf(stderr, "Missing extension %s\n", ext); \
			exit(1); \
		} else { \
			fprintf(stdout, "Found extension %s\n", ext); \
		} \
	} while (0);

static void print_gl_info(void);
static void render_pass(struct renderer *r);

int main(int argc, char *argv[])
{
	int close = 0;
#ifdef __GNUC__
	XEvent ev;

	if (window_init(&win, 800, 600) < 0) {
		fprintf(stderr, "Error creating window\n");
		exit(1);
	}
#else
	if (glfwInit() != GL_TRUE) {
		fprintf(stderr, "Error initializing GLFW\n");
		exit(1);
	}

	win = glfwCreateWindow(800, 600, "Paul Holden - Final Project", 0, 0);
	if (!win) {
		fprintf(stderr, "Error creating GLFW window\n");
		exit(1);
	}

	glfwMakeContextCurrent(win);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Error initializing GLEW\n");
		exit(1);
	}
#endif

	print_gl_info();

	CHECK_EXT("GL_EXT_framebuffer_object");
	CHECK_EXT("GL_ARB_texture_float");
	CHECK_EXT("GL_ARB_draw_buffers");

	if (renderer_init(&R, 800,  600) < 0) {
		fprintf(stderr, "Error initializing renderer\n");
		exit(1);
	}

	glClearColor(0, 0, 0, 1);

#ifdef __GNUC__
	while (!close) {
		render_pass(&R);
		glXSwapBuffers(win.dpy, win.win);
		while (XPending(win.dpy)) {
			XNextEvent(win.dpy, &ev);
			if (ev.type == KeyPress)
				close = 1;
		}
	}
#else
	while (!glfwWindowShouldClose(win)) {
		render_pass(&R);
		glfwSwapBuffers(win);
		glfwPollEvents();
	}
#endif

	renderer_deinit(&R);
	
#ifdef __GNUC__
	window_deinit(&win);
#else
	glfwDestroyWindow(win);
	glfwTerminate();
#endif
	exit(0);
}

void render_pass(struct renderer *r)
{
	renderer_prerender(r);
	renderer_bind_geometry(r);
	renderer_geometry_pass(r);
	renderer_bind_stencil(r);
	renderer_stencil_pass(r);
	renderer_bind_light(r);
	renderer_light_pass(r);
	renderer_bind_postprocess(r);
	renderer_negative_pass(r);
	renderer_textured_pass(r);
	renderer_bind_final(r);
	renderer_final_pass(r);
}

static void print_gl_info(void)
{
	int M, m;
	int fba;
	int texu;

	glGetIntegerv(GL_MAJOR_VERSION, &M);
	glGetIntegerv(GL_MINOR_VERSION, &m);
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &fba);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texu);

	printf("GLX Version %d.%d\n", M, m);
	printf("Max color attachments %d\n", fba);
	printf("Max texture units %d\n", texu);
}
