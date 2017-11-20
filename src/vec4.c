#include <vec4.h>

vec4_t vec4_add(vec4_t a, vec4_t b) {
	return (vec4_t) {
		a.x + b.x,
		a.y + b.y,
		a.z + b.z,
		a.w + b.w
	};
}

vec4_t vec4_sub(vec4_t a, vec4_t b) {
	return (vec4_t) {
		a.x - b.x,
		a.y - b.y,
		a.z - b.z,
		a.w - b.w
	};
}

float vec4_dot(vec4_t a, vec4_t b) {
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

vec4_t vec4_muls(vec4_t a, float s) {
	return (vec4_t) {
		a.x * s,
		a.y * s,
		a.z * s,
		a.w * s
	};
}

