#ifndef ROBUTTS_H
#define ROBUTTS_H

#include <mgl.h>

#include <GL/gl.h>
#include <GL/freeglut.h>

#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <ctype.h>

#include <librobutts.h>
#include <vec2.h>

typedef struct s_audio_ctx audio_ctx;
typedef struct s_wave_ctx wave_ctx;
typedef struct robot robot_t;
typedef struct pitem pitem_t;
typedef struct sprite_renderer sprite_renderer_t;
typedef struct age_font age_font_t;

/*
  Plutot qu'un design classique de jeu avec entité composants et
  systemes je suis partie sur une conception qui établit clairement
  les différents acteurs du jeu ce qui rends la chose plus claire et
  plus simple conceptuellement mais plus compliquer à étendre.
  Les explosions sont grossierements des objets updatable.
 */
typedef struct game_state {
	robot_t	 *robots;
	pitem_t	 *items;
	int		  n_robots;
	int		  n_items;

	vec2_t	 *rspawns; // spawn pour robots
	unsigned nrspawns; // nombre

	vec2_t	 *ispawns; // spawn pour items
	unsigned nispawns; // nombre

	unsigned *bmp;
	unsigned background_tex;
	unsigned robot_tex;

	// On peut associer un type d'objet à une texture en offsettant à
	// partir de la, mais cela fait qu'on doit faire des trous pour
	// les types d'objets qui n'utilise pas d'image statique
	// Indice de base pour draw_static_item
	unsigned i_score_tex; // ICI +0 (ITEM_POINT)
	unsigned i_life_tex; // +1 (ITEM_LIFE)
	unsigned i_bomb_tex; // +2 (ITEM_BOMB)

	/* 
	   TODO: On peut éviter les paddings en réordonant les types
	   d'objets et en mettant les objets utilisant une image statique en
	   premier
	*/
	unsigned i_new_item_tex; // +3

	// nouvel texture pour un objet statique
	// unsigned blabla;
	
	unsigned explosion_tex[25];
	sprite_renderer_t *renderer;
	audio_ctx *sound_player;
	wave_ctx *item_pick;
	wave_ctx *bomb_plant;
	wave_ctx *explosion;
	wave_ctx *victory;
	age_font_t *nimbus;
	long finished;
} game_state_t;

typedef struct {
	int frame;
	float energy;
} expl_state_t;

#endif /* ROBUTTS_H */
