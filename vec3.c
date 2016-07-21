#include <math.h>
#include "vec3.h"

float vec3_len(const struct vec3 *v)
{
	return (float)sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

void vec3_crs(struct vec3 *dv, const struct vec3 *lv, const struct vec3 *rv)
{
	dv->x = (lv->y * rv->z) - (rv->y * lv->z);
	dv->y = (lv->z * rv->x) - (lv->x * rv->z);
	dv->z = (lv->x * rv->y) - (lv->y * rv->x);
}

float vec3_dot(const struct vec3 *lv, const struct vec3 *rv)
{
	return lv->x * rv->x + lv->y * rv->y + lv->z * rv->z;
}

void vec3_nrm(struct vec3 *v)
{
	float len = vec3_len(v);

	if (len < 0.00001f && len > -0.00001f)
		return;

	v->x /= len;
	v->y /= len;
	v->z /= len;
}

void vec3_add(struct vec3 *dv, const struct vec3 *lv, const struct vec3 *rv)
{
	dv->x = lv->x + rv->x;
	dv->y = lv->y + rv->y;
	dv->z = lv->z + rv->z;
}

void vec3_sub(struct vec3 *dv, const struct vec3 *lv, const struct vec3 *rv)
{
	dv->x = lv->x - rv->x;
	dv->y = lv->y - rv->y;
	dv->z = lv->z - rv->z;
}

void vec3_mul(struct vec3 *v, const float val)
{
	v->x *= val;
	v->y *= val;
	v->z *= val;
}

void vec3_div(struct vec3 *v, const float val)
{
	if (val == 0)
		return;

	v->x /= val;
	v->y /= val;
	v->z /= val;
}
