#include <robutts.h>
#include <robot.h>
#include <sprite.h>
#include <scene.h>
#include <texture.h>
#include <font.h>

void send_command(robot_t *r, command_t cmd) {
	int e = write(r->process.stdin, &cmd, sizeof(cmd));
	if (e < 0)
		perror("send command");
	fsync(r->process.stdin);
}

static void rebuild_bag_state(robot_t *self) {
	for(int i = 0; i < self->state.bag_size; ++i) {
		self->state.bag[i] = self->priv.bag_buffer[i].type;
	}
}

void bag_remove(robot_t *self, pitem_t *item) {
	int idx = item - self->priv.bag_buffer;
	self->state.bag_size--;
	memmove(item, item + 1, sizeof(pitem_t) * (self->state.bag_size - idx));
	rebuild_bag_state(self);
}

void bag_add(robot_t *self, const pitem_t *item) {
	self->priv.bag_buffer = realloc(self->priv.bag_buffer, sizeof(pitem_t) * (self->state.bag_size + 1));
	self->state.bag = realloc(self->state.bag, sizeof(item_t) * (self->state.bag_size + 1));
	memcpy(self->priv.bag_buffer + self->state.bag_size, item, sizeof(pitem_t));
	self->state.bag_size++;
	rebuild_bag_state(self);
	send_command(self, CMD_COLLECT);
	write(self->process.stdin, &item->type, sizeof(item_t));
	exec_cmd_stream(self);
}

robot_t *make_robots(int argc, char *argv[argc]) {
	robot_t *ret = calloc(argc, sizeof(robot_t));
	int i = 0;
	while (i < argc) {
		if(!make_linked_process(&ret[i].process, argv[i])) {
			free(ret);
			return 0;
		}
		++i;
	}
	return ret;
}

void collision_with_robot(robot_t *self, robot_t *other) {
	float ma_masse, sa_masse, total_mass, energie;
	vec2_t vec_energie, ref, don, relspeed, et;
	// energie = masse / 2 * vitesse ^ 2
	// vec_energie = norm(linear_speed) * energie;
	// energie_reflechie = vec_energie / (ma_masse / total_masse)
	// energie_donnée = vec_energie / (sa_masse / total_masse)

	if (self->priv.rc)
		return;
	self->priv.rc = 1;
	relspeed = vec2_sub(self->priv.linear_speed, other->priv.linear_speed);
	et = vec2_sub(self->priv.pos, other->priv.pos);
	ma_masse = self->prop.mass;
	sa_masse = other->prop.mass;
	total_mass = ma_masse + sa_masse;
	energie = 0.5f * ma_masse * vec2_dot(relspeed, relspeed);
	vec_energie = vec2_muls(vec2_norme(et), energie);
	ref = vec2_muls(vec2_sub((vec2_t){0, 0}, vec_energie), (ma_masse / total_mass) * 0.5f);
	don = vec2_muls(vec_energie, (sa_masse / total_mass) * 0.5f);
	self->priv.linear_speed = don;
	self->priv.pos = vec2_add(other->priv.pos, vec2_muls(vec2_norme(don), 64.0f));
	other->priv.linear_speed = vec2_add(other->priv.linear_speed, ref);

	energie = sqrtf(vec2_dot(ref, ref));
	other->state.life -= energie;
	energie = sqrtf(vec2_dot(don, don));
	self->state.life -= energie;
}

int init_robot(robot_t *c, vec2_t position) {
	int fail = 0;
	bitmap_t nametex;
	char short_name[12] = {0};
	int n = sigsetjmp(jb, 1);
	can_jump = 1;
	if (n) {
		alarm(0);
		return 0;
	}

	c->priv.pos = position;
	c->state.life = 100.0f;
	c->state.rays = 64;

	alarm(30);
	send_command(c, CMD_INIT);
	read(c->process.stdout, &c->prop, sizeof(c->prop));
	alarm(0);

	c->prop.name[31] = 0;
	fail = ((c->prop.mass < 1 || c->prop.angular_power < 1 || c->prop.linear_power < 1) ||
			(c->prop.mass + c->prop.angular_power + c->prop.linear_power) > 5.1f);
	c->prop.angular_power *= 1.0f / 100.0f;
	c->state.depth_buffer = calloc(sizeof(float), c->state.rays);
	c->state.obj_attr_buffer = calloc(sizeof(float), c->state.rays);
	c->priv.obj_idx_buffer = calloc(sizeof(collarg_t), c->state.rays);

	memcpy(short_name, c->prop.name, 11);
	nametex.buff = make_string_bitmap(get_game_state()->nimbus,
									  short_name,
									  &nametex.width,
									  &nametex.height);
	make_texture(&c->priv.name_sprite.tid, nametex);
	c->priv.name_sprite.angle = 0;
	c->priv.name_sprite.color = 0xFFFFFFFF;
	c->priv.name_sprite.dims = (vec2_t){nametex.width, nametex.height};
	return !fail;
}

void draw_robot(robot_t *self, sprite_renderer_t *sr) {
	game_state_t *gs = get_game_state();
	sprite_t s = {gs->robot_tex,
				  vec2_sub(self->priv.pos, (vec2_t){32, 32}),
				  -self->priv.angle,
				  self->prop.color, {64, 64}};
	draw_sprite(sr, s);
	s = self->priv.name_sprite;
	s.pos = self->priv.pos;
	s.pos.y -= 36 + s.dims.y;
	s.pos.x -= s.dims.x / 2;
	draw_sprite(sr, s);
}
