#include <mat3.h>
#include <mat4.h>
#include <string.h>
#include <math.h>

mat4_t mat4_identity() {
	return (mat4_t) {{
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	}};
}

vec4_t	mat4_transform4(mat4_t m, vec4_t a) {
	int i, j;
	float *f;
	float *g;
	vec4_t ret;

	memset(&ret, 0, sizeof(ret));
	f = &ret.x;
	g = &a.x;
	for (i = 0; i < 4; ++i)
		for (j = 0; j < 4; ++j)
			f[i] += m.v[i][j] * g[j];
	return (ret);
}

vec3_t	mat4_transform3(mat4_t m, vec3_t a) {
	vec4_t ret;

	ret = mat4_transform4(m, (vec4_t){
				a.x,
				a.y,
				a.z,
				1.0f
	});
	return (vec3_t){
			ret.x / ret.w,
			ret.y / ret.w,
			ret.z / ret.w
	};
}

mat4_t	mat4_translate(vec3_t pos) {
	return (mat4_t) {{
			{1, 0, 0, pos.x},
			{0, 1, 0, pos.y},
			{0, 0, 1, pos.z},
			{0, 0, 0,     1}
	}};
}

mat4_t	mat4_scale(vec3_t f) {
	return (mat4_t) {{
			{f.x, 0, 0, 0},
			{0, f.y, 0, 0},
			{0, 0, f.z, 0},
			{0, 0, 0,   1}
	}};
}

mat4_t	mat4_mult(mat4_t a, mat4_t b) {
	int i, j, k;
	mat4_t	c;

	memset(&c, 0, sizeof(c));
	for (i = 0; i < 4; ++i)
		for (k = 0; k < 4; ++k)
			for (j = 0; j < 4; ++j)
				c.v[i][j] += a.v[i][k] * b.v[k][j];
	return (c);
}

mat4_t	mat4_rotation(vec3_t u, float t)
{
	mat3_t i = mat3_identity();
	mat3_t q = (mat3_t) {{
			{0, -u.z, u.y},
			{u.z, 0, -u.x},
			{-u.y, u.x, 0}
	}};
	mat3_t r = mat3_add(i, mat3_add(mat3_muls(q, sinf(t)),
									mat3_muls(mat3_mult(q, q), 1.0f - cosf(t))));
	return (mat4_t) {{
			{r.v[0][0], r.v[0][1], r.v[0][2], 0.0f},
			{r.v[1][0], r.v[1][1], r.v[1][2], 0.0f},
			{r.v[2][0], r.v[2][1], r.v[2][2], 0.0f},
			{0.0f, 0.0f, 0.0f, 1.0f}
	}};
}

mat4_t mat4_ortho(float gauche, float droite, float bas, float haut, float near, float far) {
	return (mat4_t) {{
			{2.0f / (droite - gauche), 0, 0, 0},
			{0, 2.0f / (haut - bas), 0, 0},
			{0, 0, -2.0f / (far - near), 0},
			{-(droite + gauche) / (droite - gauche), -(haut + bas) / (haut - bas), - (far + near) / (far - near), 1}
		}};	
}