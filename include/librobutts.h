#ifndef LIBROBUTTS_H
#define LIBROBUTTS_H

#include <sys/types.h>

typedef int i32;
typedef long i64;
typedef short i16;
typedef char i8;

typedef unsigned int u32;
typedef unsigned long u64;
typedef unsigned short u16;
typedef unsigned char u8;

/*
  Les commandes sont des constantes qui sont communiquées du serveur
  de jeu vers un client.  

  CMD_INIT: Le serveur demande au client de communiquer les parametres
  d'initialisation du robot
  
  CMD_UPDATE_STATE: L'état du robot a changé, le serveur communique le
  nouvel état.

  CMD_UPDATE: Le serveur demande au client le nouvel état des moteurs
  du robot

  CMD_DESTROY: Le serveur informe le client que son robot est mort.

  CMD_COLLECT: Le robot vient de ramasser un objet qui a été placé
  dans son sac

  CMD_COLLISION: Le robot est entré en collision avec un objet solide
 */
typedef enum {
	CMD_INIT = 0x00646d63,
	CMD_UPDATE_STATE = 0x01646d63,
	CMD_UPDATE = 0x02646d63,
	CMD_DESTROY = 0x03646d63,
	CMD_COLLECT = 0x04646d63,
	CMD_COLLISION = 0x05646d63
} command_t;

/*
  Se sont des constantes utilisés pour les requetes client -> serveur
  
  REQ_INIT: Inutilisé.
  REQ_UPDATE: Le client s'apprete à communiquer l'état de ses moteurs

  REQ_USE_ITEM: Le client veut utiliser l'objet à l'indice spécifié
  dans son sac, le serveur répond 0 ou 1 selon si l'objet a pu être
  utilisé

  REQ_END: Le client a fini de faire ses demande et rend la main au serveur
 */
typedef enum {
	REQ_INIT = 0x00716572,
	REQ_UPDATE = 0x01716572,
	REQ_USE_ITEM = 0x02716572,
	REQ_END = 0x03716572
} request_t;

typedef enum {
	ITEM_POINT,		// Donne un point
	ITEM_LIFE,		// Redonne 20 PV
	ITEM_BOMB,		// Place une bombe cachée à la position du robot lors de l'utilisation, puis donne un activateur pour cette bombe
	// Les objets en dessous ne peuvent être spawné
	ITEM_BASE,

	// Les objets en dessous ne peuvent pas etre directement affichés avec draw_static_item
	ITEM_BOMB_ACT,	// Fait exploser la bombe correspondante lors de l'activation
	ITEM_EXPLOSION, // Cause des dégât et une poussée
} item_t;

/*
  Constantes utilisés pour indiquer avec quoi le robot est entré en collision
 */
typedef enum {
	COLL_NONE,
	COLL_WALL,
	COLL_ITEM,
	COLL_ROBOT
} coll_t;

/*
  Pour le moteur rotatif, le sens conventionel est le sens des aiguilles d'une montre
 */
typedef enum {
	ENG_REVERSE = -1,
	ENG_OFF = 0,
	ENG_ON = 1
} engine_state_t;

typedef struct {
	i32		rot_eng_state;
	i32		lin_eng_state;
	// immutable (dans le sens ou leurs modifications sont insignifiantes)
	float	life;
	i32		score;
	i32		bag_size;
	i32		rays; // nombre de capteurs.
	item_t	*bag;
	float	*depth_buffer; // taille de rays
	coll_t	*obj_attr_buffer; // taille de rays
} robot_state;

/*
  You may have to export a global variable of this type, if you need to change it,
  do it in the init function.
  If you don't, a default robot is used.
 */
typedef struct {
	float 	angular_power;
	float 	linear_power;
	float 	mass;
	u32	 	color; // 0xXXRRGGBB;
	char	name[32];
} robot_properties;

extern robot_properties my_robot;
extern robot_state my_state;

/*
	Utilise un objet dont l'indice dans le sac est donné en parametre
	Renvoie 0 si erreur.
*/
int use_item(int idx);

/*
  Cette fonction est appelée avec argc et argv.
  Peut être utilisée pour modifier my_robot si des valeurs sont 
  déterminées que lors de l'execution.
 */
void init(int, char *[]);

/*
  Si le robot est trop long à répondre, il sera tué.

  Appelée quand le robot change d'état. Aucune action 
  ne doit être prise ici. C'est purement à titre indicatif.
 */
void update_state();

/*
  Si le robot est trop long à répondre, il sera tué.

  Dans cette fonction il faut modifier l'état des moteurs dans my_state
  en fonction de l'états des capteurs, aussi accessibles dans my_state.
 */
void update();

/*
  Appelée quand le robot est tué. Selon les causes du décès, il peut
  encore effectuer des requetes ici. 
  Si vous avez besoin de routines pour nettoyer avant de quitter,
  c'est ici que ca se passe. Ou bien appeler atexit dans init()
 */
void destroy();

/*
  Appelée quand un objet est ajouté dans le sac.
 */
void item_collected(item_t);

/*
  Appelée lors d'un contact entre le robot et un obstacle.
 */
void collision(coll_t);

/*
  Lit au moins n octets de fd. Sauf si EOF est atteind, dans
  ce cas la, la valeur renvoiyée est inférieure à n et les appels
  successifs renverront 0.
  En cas d'erreur, -1 est renvoyé et errno est affecté
 */
int exact_read(int fd, void *buff, size_t n);

#endif
