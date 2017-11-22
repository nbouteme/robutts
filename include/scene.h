#ifndef SCENE_H
#define SCENE_H

#include <librobutts.h>
#include <vec2.h>

typedef struct game_state game_state_t;

/*
  Un objet dans le sens physique
 */
typedef struct pitem {
	vec2_t pos;
	item_t type;
	void *user_ptr;
} pitem_t;

/*
  représente l'indice du robot avec lequel une collision s'est produite
  ou l'indice de l'objet avec lequel une collision s'est produite
 */
typedef union {
	item_t item_id;
	int	   robot_id;
} collarg_t;

/*
  Un triplet d'information d'une collision, liée à un rayon: où, et quoi
 */
typedef struct {
	float depth;
	coll_t type;
	collarg_t arg;
} intersect_data_t;

/*
  Un rayon avec direction normalisée
 */
typedef struct {
	vec2_t ori;
	vec2_t dir;
} ray_t;

/*
  Renvoie l'intersection entre le rayon et un objet de la scene, en
  ignorant les collision entre le rayon et l'objet référencé par self
 */
intersect_data_t raycast_scene(ray_t r, int self);
int raycast_bitmap(ray_t r, float max, intersect_data_t *out);

/*
  Renvoie un pointeur vers l'état du jeu
 */
game_state_t *get_game_state();
void spawn_rand_item();
void spawn_item(pitem_t);


#endif /* SCENE_H */