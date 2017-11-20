#ifndef ROBUTTS_H
#define ROBUTTS_H

#ifdef USE_GLEW
# define GLEW_STATIC
# include <GL/glew.h>
#else
# include <mgl.h>
#endif

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

#include "librobutts.h"
#include "vec2.h"
#include <mat4.h>

/*
  Enumeration des possibles causes de morts
 */

typedef enum {
	D_HANGED = 1,
	D_EXITED,
	D_PROT,
	D_LOST,
	D_BROKEN
} death_reason_t;

/*
  Un processus avec ses entrée/sorties redirigé 
 */
typedef struct {
	pid_t pid;
	int stdin;
	int stdout;
} process_t;

/*
  Un rayon avec direction normalisée
 */
typedef struct {
	vec2_t ori;
	vec2_t dir;
} ray_t;

/*
  représente l'indice du robot avec lequel une collision s'est produite
  ou l'indice de l'objet avec lequel une collision s'est produite
 */
typedef union {
	item_t item_id;
	int	   robot_id;
} collarg_t;

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
  Un triplet d'information d'une collision, liée à un rayon: où, et quoi
 */
typedef struct {
	float depth;
	coll_t type;
	collarg_t arg;
} intersect_data_t;

/*
  Un objet dans le sens physique
 */
typedef struct {
	vec2_t pos;
	item_t type;
	void *user_ptr;
} pitem_t;

/*
  État caché du robot qui n'est pas communiqué au client
 */
typedef struct
{
	vec2_t pos;
	vec2_t linear_speed;
	float angle;
	float angular_speed;
	collarg_t *obj_idx_buffer;
	pitem_t *bag_buffer;
	sprite_t name_sprite;
	int rc;
	int ic;
} hidden_state_t;

/*
  Un robot, lié à un processus qui execute son code, propriété, état communiqué mutable et état interne
 */
typedef struct {
	process_t process;
	robot_properties prop;
	robot_state state;
	hidden_state_t priv;
	int dead;
} robot_t;

/*
  Maintient des information necessaire au dessin d'un sprite.
 */
typedef struct {
	unsigned	shader;
	unsigned	vao;
	unsigned	proj_u;
	unsigned	model_u;
	unsigned	color_u;
	mat4_t		proj;
} sprite_renderer_t;

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

typedef struct embedded_file {
	unsigned char *data;
	long size;
} embedded_t;

typedef struct {
	int width;
	int height;
	unsigned *buff;
} bitmap_t;


typedef struct age_file age_file_t;

struct age_font {
	age_file_t *desc;
	bitmap_t bitmap;
};

typedef struct age_font age_font_t;

/*
  Forward déclarations pour pointeurs opaques
 */
typedef struct s_audio_ctx audio_ctx;
typedef struct s_wave_ctx wave_ctx;

/*
  Plutot qu'un design classique de jeu avec entité composants et
  systemes je suis partie sur une conception qui établit clairement
  les différents acteurs du jeu ce qui rends la chose plus claire et
  plus simple conceptuellement mais plus compliquer à étendre.
  Les explosions sont grossierements des objets updatable.
 */
typedef struct {
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

age_font_t *load_font(char *texfile, char *descfile);
void *make_string_bitmap(age_font_t *font, char *str, int *w, int *h);

audio_ctx *make_audio_player();
wave_ctx *load_wav(audio_ctx *ap, const char *fn);
wave_ctx *load_wav_data(audio_ctx *ap, u8 *fn, u32 size);
void play_wav_async(audio_ctx *ap, wave_ctx *wav);
void destroy_audio_sample(wave_ctx *wav);
void destroy_audio_player(audio_ctx *ctx);

int init_robot(robot_t *c, vec2_t position);
void send_command(robot_t *r, command_t cmd);

/*
  Instancie une liste de robots avec leur ligne de commande
 */
robot_t *make_robots(int argc, char *argv[argc]);

/*
  Crée un processus en redirigeant son entrée/sortie sur des pipes
 */
int make_linked_process(process_t *ret, char *cmd);

/*
  Lis un fichier entier, et écrit la taille dans la variable pointée par size
 */
char *readfile(const char *name, unsigned *size);

/*
  Différence en nanosecondes entre deux points
 */
long nano_diff(struct timespec a, struct timespec b);

/*
  Charge un fichier TGA 32 bits non-compressé à partir du nom de fichier.
  Renvoie 0 si le chargement a échoué.
 */
int load_tga(bitmap_t *self, const char *name);

/*
  Crée une texture à partir d'un bitmap. Écrit l'id de la texture dans la variable pointée par out
  Renvoie 0 si le chargement a échoué.  
 */
int make_texture(unsigned *out, bitmap_t tex);

/*
  Crée un shader du type spécifié à partir du nom de fichier donné en parametre
  Renvoie un entier qui référence le programme crée.
 */
int load_shader(const char *filename, int type);

/*
  Crée et link un shader complet à partir du vertex shader et fragment shader
  Renvoie un entier qui référence le programme crée.  
 */
int make_shader(unsigned v, unsigned f);

/*
  Renvoie un pointeur vers l'état du jeu
 */
game_state_t *get_game_state();
void draw_sprite(sprite_renderer_t *self, sprite_t sprite);
sprite_renderer_t *make_sprite_renderer();
void spawn_rand_item();
void spawn_item(pitem_t);
/*
  Renvoie l'intersection entre le rayon et un objet de la scene, en
  ignorant les collision entre le rayon et l'objet référencé par self
 */
intersect_data_t raycast_scene(ray_t r, int self);
int raycast_bitmap(ray_t r, float max, intersect_data_t *out);
void collision_with_robot(robot_t *self, robot_t *other);
void bag_add(robot_t *self, const pitem_t *);
void bag_remove(robot_t *self, pitem_t *);

/*
  Éxecute une suite de requete de la part du robot jusqu'au marqueur de fin
 */
void exec_cmd_stream(robot_t *c);
void draw_robot(robot_t *self, sprite_renderer_t *sr);

embedded_t get_asset(const char *name);


#endif /* ROBUTTS_H */
