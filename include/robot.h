#ifndef ROBOT_H
#define ROBOT_H

#include <vec2.h>
#include <scene.h>
#include <sprite.h>
#include <process.h>


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
typedef struct robot {
	process_t process;
	robot_properties prop;
	robot_state state;
	hidden_state_t priv;
	int dead;
} robot_t;

void collision_with_robot(robot_t *self, robot_t *other);
void bag_add(robot_t *self, const pitem_t *);
void bag_remove(robot_t *self, pitem_t *);

/*
  Éxecute une suite de requete de la part du robot jusqu'au marqueur de fin
 */
void exec_cmd_stream(robot_t *c);
void draw_robot(robot_t *self, sprite_renderer_t *sr);

/*
  Instancie une liste de robots avec leur ligne de commande
 */
robot_t *make_robots(int argc, char *argv[argc]);
int init_robot(robot_t *c, vec2_t position);
void send_command(robot_t *r, command_t cmd);

#endif /* ROBOT_H */