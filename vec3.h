#ifndef VEC3_H
#define VEC3_H

#define vec3_set(v, a, b, c) \
	do { \
		(v)->x = a; \
		(v)->y = b; \
		(v)->z = c; \
	} while (0);

#define vec3_zero(v) vec3_set(v, 0, 0, 0)

struct vec3
{
	float x;
	float y;
	float z;
};

float vec3_len(const struct vec3 *v);

void vec3_nrm(struct vec3 *v);

void vec3_crs(struct vec3 *dv, const struct vec3 *lv, const struct vec3 *rv);

float vec3_dot(const struct vec3 *lv, const struct vec3 *rv);

void vec3_add(struct vec3* dv, const struct vec3 *lv, const struct vec3 *rv);

void vec3_sub(struct vec3* dv, const struct vec3 *lv, const struct vec3 *rv);

void vec3_mul(struct vec3 *v, const float val);

void vec3_div(struct vec3 *v, const float val);

#endif /* VEC3_H */
