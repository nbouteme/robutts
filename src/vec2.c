#include "vec2.h"
#include <math.h>

vec2_t vec2_add(vec2_t a, vec2_t b) {
	return (vec2_t) {
		a.x + b.x,
		a.y + b.y,
	};
}

vec2_t vec2_sub(vec2_t a, vec2_t b) {
	return (vec2_t) {
		a.x - b.x,
		a.y - b.y,
	};
}

float vec2_dot(vec2_t a, vec2_t b) {
	return a.x * b.x + a.y * b.y;
}

vec2_t vec2_muls(vec2_t a, float s) {
	return (vec2_t) {
		a.x * s,
		a.y * s,
	};
}

vec2_t vec2_up() {
	return (vec2_t) {
		0,
		-1,
	};
}

vec2_t vec2_rot(vec2_t a, float angle) {
	float cost = cosf(angle);
	float sint = sinf(angle);

	return (vec2_t) {
		a.x * cost - a.y * sint,
		a.x * sint + a.y * cost			
	};
}

vec2_t vec2_norme(vec2_t a) {
	float n = 1.0f / sqrtf(vec2_dot(a, a));

	a.x *= n;
	a.y *= n;
	return a;
}

vec2_t vec2_cross(vec2_t a) {
	float t = a.x;
	a.x = -a.y;
	a.y = t;
	return a;
}

vec2_t vec2_reflect(vec2_t i, vec2_t n) {
	//I - 2.0 * dot(N, I) * N
	return (vec2_sub(i, vec2_muls(n, 2.0f * vec2_dot(n, i))));
}
