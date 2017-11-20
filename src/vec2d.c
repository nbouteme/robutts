#include "vec2d.h"

vec2d_t vec2d_add(vec2d_t a, vec2d_t b) {
	return (vec2d_t) {
		a.x + b.x,
		a.y + b.y,
	};
}

vec2d_t vec2d_sub(vec2d_t a, vec2d_t b) {
	return (vec2d_t) {
		a.x - b.x,
		a.y - b.y,
	};
}

float vec2d_dot(vec2d_t a, vec2d_t b) {
	return a.x * b.x + a.y * b.y;
}

vec2d_t vec2d_muls(vec2d_t a, float s) {
	return (vec2d_t) {
		a.x * s,
		a.y * s,
	};
}

vec2d_t vec2d_up() {
	return (vec2d_t) {
		0,
		-1,
	};
}

vec2d_t vec2d_rot(vec2d_t a, float angle) {
	(void)angle;
	return a;
}

