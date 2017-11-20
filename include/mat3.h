#ifndef MAT3_H
#define MAT3_H

#include <vec3.h>

typedef union {
	vec3_t s[3];
	float v[3][3];
} mat3_t;

mat3_t mat3_identity();
mat3_t	mat3_mult(mat3_t a, mat3_t b);
mat3_t	mat3_add(mat3_t a, mat3_t b);
mat3_t	mat3_muls(mat3_t a, float s);

#endif /* MAT3_H */
