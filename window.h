#ifndef WINDOW_H
#define WINDOW_H

#include <X11/Xlib.h>
#include <GL/glew.h>
#include <GL/glx.h>

#define INPUT_MASK ( \
	ExposureMask | \
	StructureNotifyMask | \
	PointerMotionMask | \
	ButtonPressMask | \
	ButtonReleaseMask | \
	KeyPressMask | \
	0 \
	)

struct window
{
	Display *dpy;
	Window win;
	int screen;
	GLXContext ctx;
	XVisualInfo *vi;
	Colormap cmap;
	XSetWindowAttributes attr;
};

int window_init(struct window *win, int w, int h);

void window_deinit(struct window *win);

#endif /* WINDOW_H */
