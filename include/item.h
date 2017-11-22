#ifndef ITEM_H
#define ITEM_H

typedef struct robot robot_t;
typedef struct pitem pitem_t;
typedef struct sprite_renderer sprite_renderer_t;

/*
  Structure maintenant les fonctions à appeler pour un objet dans certain états.
  init: Quand l'objet est ajouté au terrain
  update: À chaque boucle de mise à jour du jeu
  activate: Au contact avec un robot
  draw: À chaque boucle de dessin
  destroy: quand l'objet est retiré du terrain
 */
typedef struct {
	void (*init)(pitem_t*);
	void (*update)(pitem_t*);
	void (*activate)(pitem_t*, robot_t*);
	void (*draw)(pitem_t*, sprite_renderer_t *);
	void (*destroy)(pitem_t*);
} item_interface_t;

// Fait correspondre un type d'objet à son interface
extern item_interface_t collectable_item_vtable[];


#endif /* ITEM_H */