#include <vec3.h>
#include <math.h>

vec3_t vec3_add(vec3_t a, vec3_t b) {
	return (vec3_t) {
		a.x + b.x,
		a.y + b.y,
		a.z + b.z
	};
}

vec3_t vec3_sub(vec3_t a, vec3_t b) {
	return (vec3_t) {
		a.x - b.x,
		a.y - b.y,
		a.z - b.z,
	};
}

float vec3_dot(vec3_t a, vec3_t b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3_t vec3_norme(vec3_t a) {
	float n = 1.0f / sqrtf(vec3_dot(a, a));

	a.x *= n;
	a.y *= n;
	a.z *= n;
	return a;
}

vec3_t vec3_muls(vec3_t a, float s) {
	return (vec3_t) {
		a.x * s,
		a.y * s,
		a.z * s,
	};
}

vec3_t vec3_up() {
	return (vec3_t) {
		0,
		-1,
		0
	};
}

