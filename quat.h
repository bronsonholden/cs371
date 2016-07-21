#ifndef QUAT_H
#define QUAT_H

#include "vec3.h"
#include "mat4.h"

struct quat
{
	float a;
	float b;
	float c;
	float d;
};

#define quat_id(q) \
	do { \
		(q)->a = 1; \
		(q)->b = 0; \
		(q)->c = 0; \
		(q)->d = 0; \
	} while (0);

void quat_to_mat4(struct mat4 *mat, const struct quat *quat);

float quat_get_angle(const struct quat *quat);

void quat_get_axis(struct vec3 *axis, const struct quat *quat);

void quat_set_axis_angle(struct quat *quat, const struct vec3 *axis,
                                                             float angle);

void quat_mul(struct quat *quat, const struct quat *lv,
                                      const struct quat *rv);

void quat_invert(struct quat *quat);

void quat_inverted(struct quat *dest, const struct quat *src);

void quat_normalize(struct quat *quat);

void quat_normalized(struct quat *dest, const struct quat *src);

float quat_length(const struct quat *quat);

float quat_length2(const struct quat *quat);

#endif /* QUAT_H */
