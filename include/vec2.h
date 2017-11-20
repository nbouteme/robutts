#ifndef VEC2_H
#define VEC2_H

typedef struct {
	float x;
	float y;
} vec2_t;

vec2_t vec2_add(vec2_t a, vec2_t b);
vec2_t vec2_sub(vec2_t a, vec2_t b);
float vec2_dot(vec2_t a, vec2_t b);
vec2_t vec2_muls(vec2_t a, float s);
vec2_t vec2_up();
vec2_t vec2_rot(vec2_t a, float angle);
vec2_t vec2_reflect(vec2_t i, vec2_t n);
vec2_t vec2_cross(vec2_t a);
vec2_t vec2_norme(vec2_t a);

#endif /* VEC2_H */
