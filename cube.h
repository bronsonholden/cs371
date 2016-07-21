#ifndef CUBE_H
#define CUBE_H

#include "GL/glew.h"
#include "vec3.h"

struct attribs
{
	struct vec3 pos;
	struct vec3 nrm;
	struct vec3 col; /* vertex color */
	/* texcoords */
	float s;
	float t;
};

/* If successful, returns 1 and *attr will be set to the buffer name for the
 * created cube mesh vertex data, and *idx will be set to the buffer name
 * for the cube indices. Returns 0 on failure.
 */
int create_cube_mesh(GLuint *attr, GLuint *idx);

#endif /* CUBE_H */
