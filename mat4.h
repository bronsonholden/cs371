#ifndef MAT4_H
#define MAT4_H

#include "vec3.h"

struct mat4
{
	float a[4];
	float b[4];
	float c[4];
	float d[4];
};

#define mat4_id(ptr) \
	do { \
		(ptr)->a[0] = 1.0f; \
		(ptr)->a[1] = 0.0f; \
		(ptr)->a[2] = 0.0f; \
		(ptr)->a[3] = 0.0f; \
		(ptr)->b[0] = 0.0f; \
		(ptr)->b[1] = 1.0f; \
		(ptr)->b[2] = 0.0f; \
		(ptr)->b[3] = 0.0f; \
		(ptr)->c[0] = 0.0f; \
		(ptr)->c[1] = 0.0f; \
		(ptr)->c[2] = 1.0f; \
		(ptr)->c[3] = 0.0f; \
		(ptr)->d[0] = 0.0f; \
		(ptr)->d[1] = 0.0f; \
		(ptr)->d[2] = 0.0f; \
		(ptr)->d[3] = 1.0f; \
	} while (0);

void mat4_ortho(struct mat4 *m, const struct vec3 *bl,
                                 const struct vec3 *tr);

void mat4_persp(struct mat4 *m, float fov, float ratio, float near,
                                                        float far);

void mat4_lookat(struct mat4 *m, const struct vec3 *pos,
                                    const struct vec3 *tar,
                                    const struct vec3 *up);

void mat4_add(struct mat4 *m, const struct mat4 *lv,
                                  const struct mat4 *rv);

void mat4_subtract(struct mat4 *m, const struct mat4 *lv,
                                      const struct mat4 *rv);

void mat4_multiplyf(struct mat4 *m, const float val);

void mat4_multiply(struct mat4 *m, const struct mat4 *lv,
                                      const struct mat4 *rv);

#endif /* MAT4_H */
