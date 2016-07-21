EXECUTABLE_NAME = project

SOURCE = \
	main.c \
	framebuffer.c \
	mat4.c \
	vec3.c \
	quat.c \
	cube.c \
	window.c \
	shader.c \
	renderer.c \

DFLAGS = -DGLEW_STATIC
IFLAGS = -I.
LFLAGS = -lX11 -lGL -lm
CFLAGS = -o ${EXECUTABLE_NAME}

all: ${EXECUTABLE_NAME}
${EXECUTABLE_NAME}: glew.o ${SOURCE}
	gcc $^ ${DFLAGS} ${IFLAGS} ${LFLAGS} ${CFLAGS};
glew.o:
	gcc ${IFLAGS} -c glew.c;
clean:
	rm -f ${EXECUTABLE_NAME} glew.o;
