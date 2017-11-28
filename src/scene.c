#include <robutts.h>
#include <scene.h>
#include <robot.h>

int intersect_ray_circle(ray_t ra, vec2_t pos, float r, float *d) {
	vec2_t f = vec2_sub(ra.ori, pos);
	float a = vec2_dot(ra.dir, ra.dir);
	float b = 2 * vec2_dot(f, ra.dir);
	float c = vec2_dot(f, f) - r * r;
	float discriminant = b * b - 4 * a * c;
	float t, t1, t2;

	if (discriminant < 0) {
		return 0;
	}
	discriminant = sqrtf(discriminant);
	t2 = (-b - discriminant) / (2 * a);
	t1 = (-b + discriminant) / (2 * a);
	if (t1 > t2) {
		t = t1;
		t1 = t2;
		t2 = t;
	}
	if (t1 < 0) {
		if (t2 < 0)
			return (0);
		t = t1;
		t1 = t2;
		t2 = t;
	}
	*d = fminf(t1, t2);
	return (1);
}

intersect_data_t raycast_scene(ray_t r, int self) {
	game_state_t *game_state = get_game_state();
	intersect_data_t ret = (intersect_data_t) {INFINITY, COLL_NONE, {0}};
	float f, rd;
	int i;
	
	for (i = 0; i < game_state->n_robots; ++i) {
		if (i == self)
			continue;
		if (!intersect_ray_circle(r, game_state->robots[i].priv.pos, 32.0f, &f))
			continue;
		if (f >= ret.depth)
			continue;
		ret.depth = f;
		ret.type = COLL_ROBOT;
		ret.arg.robot_id = i;
	}

	for (i = 0; i < game_state->n_items; ++i) {
		rd = 8.0f;
		if (game_state->items[i].type == ITEM_EXPLOSION)
			rd = 48.0f;
		if (!intersect_ray_circle(r, game_state->items[i].pos, rd, &f))
			continue;
		if (f >= ret.depth)
			continue;
		ret.depth = f;
		ret.type = COLL_ITEM;
		ret.arg.item_id = i;
	}
	return ret;
}

int raycast_bitmap(ray_t r, float max, intersect_data_t *out) {
	game_state_t *game_state = get_game_state();
	unsigned *bmp = game_state->bmp;
	vec2_t sample;
	float dist = 0.0f;

	if (r.ori.x >= 1280 || r.ori.x < 0 || r.ori.y >= 720 || r.ori.y < 0) {
		out->depth = 0;
		out->type = COLL_WALL;
		return 1;
	}

	while(dist < max) {
		sample = vec2_add(r.ori, vec2_muls(r.dir, dist));
		if ((bmp[(int)sample.y * 1280 + (int)sample.x] & 0x00FFFFFF) == 0) {
			*out = (intersect_data_t){dist, COLL_WALL, {0}};
			return 1;
		}
		dist += 1.0f;
	}
	return 0;
}
