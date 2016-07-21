#include "window.h"

static int default_attribs[] = {
	GLX_RGBA,
	GLX_RED_SIZE, 8,
	GLX_GREEN_SIZE, 8,
	GLX_BLUE_SIZE, 8,
	GLX_DEPTH_SIZE, 16,
	GLX_DOUBLEBUFFER,
	None
};

int window_init(struct window *win, int w, int h)
{
	win->dpy = XOpenDisplay(0);
	if (!win->dpy)
		return -1;

	win->screen = DefaultScreen(win->dpy);
	win->vi = glXChooseVisual(win->dpy, win->screen, default_attribs);
	if (!win->vi)
		return -1;
	win->ctx = glXCreateContext(win->dpy, win->vi, 0, GL_TRUE);
	if (!win->ctx)
		return -1;
	win->cmap = XCreateColormap(win->dpy,
			RootWindow(win->dpy, win->vi->screen),
			win->vi->visual,
			AllocNone
		);
	win->attr.colormap = win->cmap;
	win->attr.border_pixel = 0;
	win->attr.event_mask = INPUT_MASK;
	win->win = XCreateWindow(
			win->dpy,
			RootWindow(win->dpy, win->vi->screen),
			0,
			0,
			w,
			h,
			0,
			win->vi->depth,
			InputOutput,
			win->vi->visual,
			CWBorderPixel | CWColormap | CWEventMask,
			&win->attr
		);
	if (!win->win)
		return -1;
	XMapWindow(win->dpy, win->win);
	glXMakeCurrent(win->dpy, win->win, win->ctx);
	XStoreName(win->dpy, win->win, "Paul Holden - Final Project");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
		return -1;

	return 0;
}

void window_deinit(struct window *win)
{
	if (!win->dpy)
		return;

	glXMakeCurrent(win->dpy, None, 0);
	XFree(win->vi);
	XFreeColormap(win->dpy, win->cmap);
	glXDestroyContext(win->dpy, win->ctx);
	XDestroyWindow(win->dpy, win->win);
	XCloseDisplay(win->dpy);
}
