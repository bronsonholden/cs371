#include "cube.h"

static const struct attribs cube_attribs[] = {
	/* +z face */
	{
		-1, -1, 1,
		0, 0, 1,
		1, 1, 1,
		0, 0
	},
	{
		-1, 1, 1,
		0, 0, 1,
		1, 1, 1,
		0, 1
	},
	{
		1, 1, 1,
		0, 0, 1,
		1, 1, 1,
		1, 1
	},
	{
		1, -1, 1,
		0, 0, 1,
		1, 1, 1,
		1, 0
	},
	/* -z face */
	{
		1, -1, -1,
		0, 0, -1,
		1, 1, 1,
		1, 0
	},
	{
		1, 1, -1,
		0, 0, -1,
		1, 1, 1,
		1, 1
	},
	{
		-1, 1, -1,
		0, 0, -1,
		1, 1, 1,
		0, 1
	},
	{
		-1, -1, -1,
		0, 0, -1,
		1, 1, 1,
		0, 0
	},
	/* +x face */
	{
		1, -1, 1,
		1, 0, 0,
		1, 1, 1,
		0, 0
	},
	{
		1, 1, 1,
		1, 0, 0,
		1, 1, 1,
		0, 1
	},
	{
		1, 1, -1,
		1, 0, 0,
		1, 1, 1,
		1, 1
	},
	{
		1, -1, -1,
		1, 0, 0,
		1, 1, 1,
		1, 0
	},
	/* -x face */
	{
		-1, -1, -1,
		-1, 0, 0,
		1, 1, 1,
		1, 0
	},
	{
		-1, 1, -1,
		-1, 0, 0,
		1, 1, 1,
		1, 1
	},
	{
		-1, 1, 1,
		-1, 0, 0,
		1, 1, 1,
		0, 1
	},
	{
		-1, -1, 1,
		-1, 0, 0,
		1, 1, 1,
		0, 0
	},
	/* +y face */
	{
		-1, 1, 1,
		0, 1, 0,
		1, 1, 1,
		0, 0
	},
	{
		-1, 1, -1,
		0, 1, 0,
		1, 1, 1,
		0, 1
	},
	{
		1, 1, -1,
		0, 1, 0,
		1, 1, 1,
		1, 1
	},
	{
		1, 1, 1,
		0, 1, 0,
		1, 1, 1,
		1, 0
	},
	/* -y face */
	{
		1, -1, 1,
		0, -1, 0,
		1, 1, 1,
		1, 0
	},
	{
		1, -1, -1,
		0, -1, 0,
		1, 1, 1,
		1, 1
	},
	{
		-1, -1, -1,
		0, -1, 0,
		1, 1, 1,
		0, 1
	},
	{
		-1, -1, 1,
		0, -1, 0,
		1, 1, 1,
		0, 0
	}
};

const GLshort cube_indices[] = {
	0, 1, 2,
	0, 2, 3,
	4, 5, 6,
	4, 6, 7,
	8, 9, 10,
	8, 10, 11,
	12, 13, 14,
	12, 14, 15,
	16, 17, 18,
	16, 18, 19,
	20, 21, 22,
	20, 22, 23
};

int create_cube_mesh(GLuint *attr, GLuint *idx)
{
	GLuint v, i;

	if (!attr || !idx)
		return 0;

	glGenBuffers(1, &v);
	glGenBuffers(1, &i);

	glBindBuffer(GL_ARRAY_BUFFER, v);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i);

	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(cube_attribs),
		(void *)cube_attribs,
		GL_STATIC_DRAW
	);

	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		sizeof(cube_indices),
		(void *)cube_indices,
		GL_STATIC_DRAW
	);

	*attr = v;
	*idx = i;

	return 1;
}
