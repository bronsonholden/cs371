#include <string.h>
#include <math.h>
#include "quat.h"

#define sq(a) ((a) * (a))
#define abs(a) ((a) < 0 ? -(a) : (a))
#define feq(a, b) (abs((a) - (b)) < 0.00001f)

void quat_to_mat4(struct mat4 *mat, const struct quat *quat)
{
	mat->a[0] = 1.0f - (2 * sq(quat->c)) - (2 * sq(quat->d));
	mat->a[1] = (2 * quat->b * quat->c) + (2 * quat->d * quat->a);
	mat->a[2] = (2 * quat->b * quat->d) - (2 * quat->c * quat->a);
	mat->a[3] = 0.0f;
	mat->b[0] = (2 * quat->b * quat->c) - (2 * quat->d * quat->a);
	mat->b[1] = 1.0f - (2 * sq(quat->b)) - (2 * sq(quat->d));
	mat->b[2] = (2 * quat->c * quat->d) + (2 * quat->a * quat->b);
	mat->b[3] = 0.0f;
	mat->c[0] = (2 * quat->b * quat->d) + (2 * quat->c * quat->a);
	mat->c[1] = (2 * quat->c * quat->d) - (2 * quat->b * quat->a);
	mat->c[2] = 1.0f - (2 * sq(quat->b)) - (2 * sq(quat->c));
	mat->c[3] = 0.0f;
	mat->d[0] = 0.0f;
	mat->d[1] = 0.0f;
	mat->d[2] = 0.0f;
	mat->d[3] = 1.0f;
}

float quat_get_angle(const struct quat *quat)
{
	return acosf(quat->a) * 2.0f;
}

void quat_get_axis(struct vec3 *axis, const struct quat *quat)
{
	float inv;

	if (feq(quat->a, 1.0f))
		inv = 1.0f;
	else
		inv = sqrtf(1.0f - (quat->a * quat->a));
	axis->x = quat->b / inv;
	axis->y = quat->c / inv;
	axis->z = quat->d / inv;
}

void quat_set_axis_angle(struct quat *quat, const struct vec3 *axis,
                                                          float angle)
{
	float sa;

	sa = sinf(angle / 2.0f);
	quat->a = cosf(angle / 2.0f);
	quat->b = axis->x * sa;
	quat->c = axis->y * sa;
	quat->d = axis->z * sa;
}

void quat_mul(struct quat *quat, const struct quat *lv, const struct quat *rv)
{
	quat->a = lv->a * rv->a - (lv->b * rv->b + lv->c * rv->c
						+ lv->d * rv->d);
	quat->b = (lv->b * rv->a) + (rv->b * lv->a)
		+ (lv->c * rv->d) - (lv->d * rv->c);
	quat->c = (lv->c * rv->a) + (rv->c * lv->a)
		+ (lv->d * rv->b) - (rv->d * lv->b);
	quat->d = (lv->d * rv->a) + (rv->d * lv->a)
		+ (lv->b * rv->c) - (lv->c * rv->b);
}

void quat_invert(struct quat *quat)
{
	quat->a = -quat->a;
	quat->b = -quat->b;
	quat->c = -quat->c;
	quat->d = -quat->d;
	quat_normalize(quat);
}

void quat_inverted(struct quat *dest, const struct quat *src)
{
	dest->a = -src->a;
	dest->b = -src->b;
	dest->c = -src->c;
	dest->d = -src->d;
	quat_normalize(dest);
}

void quat_normalize(struct quat *quat)
{
	float len;

	len = quat_length(quat);
	quat->a /= len;
	quat->b /= len;
	quat->c /= len;
	quat->d /= len;
}

void quat_normalized(struct quat *dest, const struct quat *src)
{
	float len;

	len = quat_length(src);
	dest->a = src->a / len;
	dest->b = src->b / len;
	dest->c = src->c / len;
	dest->d = src->d / len;
}

float quat_length(const struct quat *quat)
{
	return sqrtf(quat_length2(quat));
}

float quat_length2(const struct quat *quat)
{
	return sq(quat->a) + sq(quat->b) + sq(quat->c) + sq(quat->d);
}
