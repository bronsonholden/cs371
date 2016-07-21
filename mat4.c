#include <string.h>
#include <math.h>
#include <mmintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include "mat4.h"

void mat4_ortho(struct mat4 *m, const struct vec3 *bl, const struct vec3 *tr)
{
	float w, h, d;

	mat4_id(m);
	w = tr->x - bl->x;
	h = tr->y - bl->y;
	m->a[0] = 2.0f / w;
	m->b[1] = 2.0f / h;
	d = tr->z - bl->z;
	m->c[2] = -2 / d;
	m->d[0] = (tr->x + bl->x) / (bl->x - tr->x);
	m->d[1] = (tr->y + bl->y) / (bl->y - tr->y);
	m->d[2] = (tr->z + bl->z) / -d;
}

void mat4_persp(struct mat4 *m, float fov, float ratio, float near,
                                                          float far)
{
	float h, d;

	/* Multiply fov by radians per degree / 2 */
	h = (float)(1.0 / tan(fov * 0.0087266f));
	d = near - far;

	mat4_id(m);
	m->a[0] = h / ratio;
	m->b[1] = h;
	m->c[2] = (near + far) / d;
	m->c[3] = -1.0f;
	m->d[2] = (2.0f * near * far) / d;
	m->d[3] = 0.0f;
}

void mat4_lookat(struct mat4 *m, const struct vec3 *pos,
                                   const struct vec3 *tar,
                                    const struct vec3 *up)
{
	float len;
	struct vec3 fwd;
	struct vec3 side;
	struct vec3 upv;

	fwd.x = tar->x - pos->x;
	fwd.y = tar->y - pos->y;
	fwd.z = tar->z - pos->z;

	len = vec3_len(&fwd);
	fwd.x /= len;
	fwd.y /= len;
	fwd.z /= len;

	vec3_crs(&side, &fwd, up);
	len = vec3_len(&side);
	side.x /= len;
	side.y /= len;
	side.z /= len;

	vec3_crs(&upv, &side, &fwd);
	len = vec3_len(&upv);
	upv.x /= len;
	upv.y /= len;
	upv.z /= len;

	m->a[0] = side.x;
	m->a[4] = side.y;
	m->a[8] = side.z;
	m->a[1] = upv.x;
	m->a[5] = upv.y;
	m->a[9] = upv.z;
	m->a[2] = -fwd.x;
	m->a[6] = -fwd.y;
	m->a[10] = -fwd.z;
	m->a[3] = m->a[7] = m->a[11] = 0;
	m->a[12] = -vec3_dot(&side, pos);
	m->a[13] = -vec3_dot(&upv, pos);
	m->a[14] = vec3_dot(&fwd, pos);
	m->a[15] = 1.0f;
}

void mat4_add(struct mat4 *m, const struct mat4 *lv,
                                const struct mat4 *rv)
{
	int i;

	for (i = 0; i < 16; ++i)
		m->a[i] = lv->a[i] + rv->a[i]; 
}

void mat4_subtract(struct mat4 *m, const struct mat4 *lv,
                                     const struct mat4 *rv)
{
	int i;

	for (i = 0; i < 16; ++i)
		m->a[i] = lv->a[i] - rv->a[i];
}

void mat4_multiplyf(struct mat4 *m, const float val)
{
	int i;

	for (i = 0; i < 16; ++i)
		m->a[i] *= val;
}

void mat4_multiply(struct mat4 *m, const struct mat4 *lv,
                                     const struct mat4 *rv)
{
	__m128 tmp1, tmp2;
	__m128 a, b, c, d;
	
	a = _mm_setr_ps(lv->a[0], lv->a[1], lv->a[2], lv->a[3]);
	b = _mm_setr_ps(lv->b[0], lv->b[1], lv->b[2], lv->b[3]);
	c = _mm_setr_ps(lv->c[0], lv->c[1], lv->c[2], lv->c[3]);
	d = _mm_setr_ps(lv->d[0], lv->d[1], lv->d[2], lv->d[3]);

	tmp1 = _mm_set1_ps(rv->a[0]);
	tmp2 = _mm_mul_ps(a, tmp1);
	tmp1 =_mm_set1_ps(rv->a[1]);
	tmp2 = _mm_add_ps(_mm_mul_ps(b, tmp1), tmp2);
	tmp1 =_mm_set1_ps(rv->a[2]);
	tmp2 = _mm_add_ps(_mm_mul_ps(c, tmp1), tmp2);
	tmp1 =_mm_set1_ps(rv->a[3]);
	tmp2 = _mm_add_ps(_mm_mul_ps(d, tmp1), tmp2);
	_mm_storeu_ps(&m->a[0], tmp2);

	tmp1 = _mm_set1_ps(rv->a[4]);
	tmp2 = _mm_mul_ps(a, tmp1);
	tmp1 =_mm_set1_ps(rv->a[5]);
	tmp2 = _mm_add_ps(_mm_mul_ps(b, tmp1), tmp2);
	tmp1 =_mm_set1_ps(rv->a[6]);
	tmp2 = _mm_add_ps(_mm_mul_ps(c, tmp1), tmp2);
	tmp1 =_mm_set1_ps(rv->a[7]);
	tmp2 = _mm_add_ps(_mm_mul_ps(d, tmp1), tmp2);
	_mm_storeu_ps(&m->a[4], tmp2);

	tmp1 = _mm_set1_ps(rv->a[8]);
	tmp2 = _mm_mul_ps(a, tmp1);
	tmp1 =_mm_set1_ps(rv->a[9]);
	tmp2 = _mm_add_ps(_mm_mul_ps(b, tmp1), tmp2);
	tmp1 =_mm_set1_ps(rv->a[10]);
	tmp2 = _mm_add_ps(_mm_mul_ps(c, tmp1), tmp2);
	tmp1 =_mm_set1_ps(rv->a[11]);
	tmp2 = _mm_add_ps(_mm_mul_ps(d, tmp1), tmp2);
	_mm_storeu_ps(&m->a[8], tmp2);

	tmp1 = _mm_set1_ps(rv->a[12]);
	tmp2 = _mm_mul_ps(a, tmp1);
	tmp1 =_mm_set1_ps(rv->a[13]);
	tmp2 = _mm_add_ps(_mm_mul_ps(b, tmp1), tmp2);
	tmp1 =_mm_set1_ps(rv->a[14]);
	tmp2 = _mm_add_ps(_mm_mul_ps(c, tmp1), tmp2);
	tmp1 =_mm_set1_ps(rv->a[15]);
	tmp2 = _mm_add_ps(_mm_mul_ps(d, tmp1), tmp2);
	_mm_storeu_ps(&m->a[12], tmp2);
}
