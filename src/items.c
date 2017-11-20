#include <robutts.h>

/*
  Ajoute au jeu et initialise un objet.
 */
void spawn_item(pitem_t item) {
	game_state_t *gs = get_game_state();
	++gs->n_items;
	gs->items = realloc(gs->items, sizeof(pitem_t) * gs->n_items);
	gs->items[gs->n_items - 1] = item;
	collectable_item_vtable[gs->items[gs->n_items - 1].type].init(&gs->items[gs->n_items - 1]);
}

/*
  Spawn un objet à un emplacement aléatoire parmi
  les spawns d'objets
 */
void spawn_rand_item() {
	game_state_t *gs = get_game_state();
	vec2_t pos = gs->ispawns[rand() % gs->nispawns];
	item_t type = rand() % 3; // Si ton objet doit pouvoir etre spawner, incrementer le 3
	spawn_item((pitem_t) {
			pos, type, 0
	});
}

/*
  Retire un objet du jeu
 */
void remove_item(game_state_t *gs, pitem_t *item) {
	int idx = item - gs->items;
	if (idx < 0 || idx > gs->n_items)
		return;
	collectable_item_vtable[item->type].destroy(item);
	gs->n_items--;
	memmove(item, item + 1, sizeof(pitem_t) * (gs->n_items - idx));
}

void score_item(pitem_t *self, robot_t *r) {
	++r->state.score;
	printf("%s a maintenant %d points\n", r->prop.name, r->state.score);
	game_state_t *gs = get_game_state();
	play_wav_async(gs->sound_player, gs->item_pick);
	remove_item(gs, self);
}

void life_item(pitem_t *self, robot_t *r) {
	r->state.life += 20.0f;
	printf("%s a maintenant %.1f points de vie\n", r->prop.name, r->state.life);
	game_state_t *gs = get_game_state();
	play_wav_async(gs->sound_player, gs->item_pick);
	remove_item(gs, self);
}

void bomb_item(pitem_t *self, robot_t *r) {
	if (self->user_ptr) {
		bag_remove(r, self);
		game_state_t *gs = get_game_state();
		play_wav_async(gs->sound_player, gs->bomb_plant);
		bag_add(r, &(pitem_t){r->priv.pos, ITEM_BOMB_ACT, 0});
		return;
	}
	bag_add(r, &(pitem_t){(vec2_t){0, 0}, ITEM_BOMB, r});
	game_state_t *gs = get_game_state();
	play_wav_async(gs->sound_player, gs->item_pick);
	remove_item(gs, self);
}

void bomb_act_item(pitem_t *self, robot_t *r) {
	(void) self;
	(void) r;
	spawn_item((pitem_t){self->pos, ITEM_EXPLOSION, 0});
	bag_remove(r, self);
}

static void nothing() {
}

void init_explosion(pitem_t *self) {
	expl_state_t *est = malloc(sizeof(expl_state_t));

	game_state_t *gs = get_game_state();
	play_wav_async(gs->sound_player, gs->explosion);
	est->frame = 0;
	est->energy = 100.0f;
	self->user_ptr = est;
}

void free_explosion(pitem_t *self) {
	free(self->user_ptr);
}

void update_explosion_state(pitem_t *self) {
	expl_state_t *est = self->user_ptr;

	est->frame++;
	if (est->frame > 10)
		est->energy *= 0.3;
	else
		est->energy *= 0.8;
	if (est->frame == 24)
		remove_item(get_game_state(), self);
}

void explosion_item(pitem_t *self, robot_t *r) {
	expl_state_t *est = self->user_ptr;
	vec2_t dir;
	float mult;
	float d;

	dir = vec2_sub(r->priv.pos, self->pos);
	mult = 80.0f - sqrtf(vec2_dot(dir, dir));
	d = est->energy * 0.25f * (mult / 80.0f);
	r->state.life -= d;
	est->energy *= 0.75f;
	if (dir.x == 0.0f && dir.y == 0.0f)
		dir = vec2_muls(vec2_rot(vec2_up(), rand()), 0.01f);
	/* 
	   distance minimale du contact = 80
	   et on prends plus de dégats en fonction 
	   de la distance par rapport au centre
	*/
	r->priv.linear_speed = vec2_add(r->priv.linear_speed, vec2_muls(vec2_norme(dir), mult));
}

/* 
   TODO: Le prof a dit que "le butin" ne compte que lorsque le robot
   retourne dans sa "base". "Le butin" fait référence à au moins l'objet
   score, donc: 

   - Modifier l'objet score pour ne pas immédiatement
   augmenter le score du robot mais à la place s'ajouter dans le sac

   - Créer un objet "base" qui, sur contact, va prendre les objets "score"
   du sac et incrémenter le score actuel en fonction.
   
*/

void draw_static_item(pitem_t *self, sprite_renderer_t *sr) {
	game_state_t *gs = get_game_state();
	sprite_t s = {
		// addresse de i_score_tex + type
		(&gs->i_score_tex)[self->type],
		vec2_sub(self->pos, (vec2_t){8, 8}),
		0,
		0x00FFFFFF, // teinte blanche
		{16, 16}
	};
	draw_sprite(sr, s);
}

void draw_explosion(pitem_t *self, sprite_renderer_t *sr) {
	game_state_t *gs = get_game_state();
	sprite_t s = {
		gs->explosion_tex[((expl_state_t*)self->user_ptr)->frame],
		vec2_sub(self->pos, (vec2_t){48, 48}),
		0,
		0x00FFFFFF,
		{96, 96}
	};
	draw_sprite(sr, s);
}

/*
  Pour rappel de l'interface, un objet est fait de 5 fonctions
  - Initialisation
  - Mise à jour
  - Contact avec un robot
  - Dessiner
  - Destruction

  L'indice dans le tableau correspond au type de l'objet, 
  voir l'enum item_t dans librobutts.h
 */

item_interface_t collectable_item_vtable[] = {
	{nothing, nothing, score_item, draw_static_item, nothing},
	{nothing, nothing, life_item, draw_static_item, nothing},
	{nothing, nothing, bomb_item, draw_static_item, nothing},
	{nothing, nothing, bomb_act_item, nothing, nothing},
	{init_explosion, update_explosion_state, explosion_item, draw_explosion, free_explosion},
};
