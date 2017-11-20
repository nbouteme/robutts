#ifndef VEC3_H
#define VEC3_H

typedef struct {
	float x;
	float y;
	float z;
} vec3_t;

vec3_t vec3_add(vec3_t a, vec3_t b);
vec3_t vec3_sub(vec3_t a, vec3_t b);
vec3_t vec3_norme(vec3_t a);
float vec3_dot(vec3_t a, vec3_t b);
vec3_t vec3_muls(vec3_t a, float s);
vec3_t vec3_up();
vec3_t vec3_rot(vec3_t a, float angle);

#endif /* VEC3_H */
