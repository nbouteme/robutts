#include <mat3.h>
#include <string.h>

mat3_t mat3_identity() {
	return (mat3_t) {{
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1},
	}};
}

mat3_t	mat3_add(mat3_t a, mat3_t b)
{
	for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				a.v[i][j] += b.v[i][j];
	return a;
}

mat3_t	mat3_mult(mat3_t a, mat3_t b) {
	mat3_t	c;

	memset(&c, 0, sizeof(c));
	for (int i = 0; i < 3; ++i)
		for (int k = 0; k < 3; ++k)
			for (int j = 0; j < 3; ++j)
				c.v[i][j] += a.v[i][k] * b.v[k][j];
	return (c);
}

mat3_t	mat3_muls(mat3_t a, float s) {
	for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				a.v[i][j] *= s;
	return (a);
}
