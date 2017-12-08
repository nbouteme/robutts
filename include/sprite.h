#ifndef SPRITE_H
#define SPRITE_H

#include <vec2.h>
#include <mat4.h>
#include <scene.h>

/*
  tid: ID de la texture à utiliser lord du dessin
  pos: Position en coordonée fenetre relative au haut-gauche du sprite
  angle: l'angle de rotation relatif au centre du sprite
  color: Une teinte appliquée par multiplication
  dims: Les dimensions du sprite
*/
typedef struct {
	unsigned tid;
	vec2_t pos;
	float angle;
	int color;
	vec2_t dims;
} sprite_t;

/*
  Maintient des information necessaire au dessin d'un sprite.
 */
typedef struct sprite_renderer {
	unsigned	shader;
	unsigned	vao;
	unsigned	proj_u;
	unsigned	model_u;
	unsigned	color_u;
	mat4_t		proj;
} sprite_renderer_t;

void draw_sprite(sprite_renderer_t *self, sprite_t sprite);
sprite_renderer_t *make_sprite_renderer();
sprite_renderer_t *make_sprite_renderer_window(int w, int h);

#endif /* SPRITE_H */