#ifndef MAT4_H
#define MAT4_H

#include <vec3.h>
#include <vec4.h>

typedef union {
	vec4_t s[4];
	float v[4][4];
} mat4_t;

mat4_t mat4_identity();
vec3_t mat4_transform3(mat4_t m, vec3_t a);
vec4_t mat4_transform4(mat4_t m, vec4_t a);
mat4_t mat4_mult(mat4_t a, mat4_t b);
mat4_t mat4_translate(vec3_t pos);
mat4_t mat4_rotation(vec3_t u, float t);
mat4_t mat4_scale(vec3_t f);
mat4_t mat4_ortho(float gauche, float droite, float bas, float haut, float near, float far);


#endif /* MAT4_H */
