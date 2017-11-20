#include "librobutts.h"
#include <robutts.h>
#include <string.h>

robot_properties my_robot = {
	2.5, 1.5, 1, 0xFFFF00, "bf interpret"
};

struct bf_interpreter {
	char *rom;
	char *ram;
	int ip; // Instruction Pointer
	int dp; // Data Pointer
};

typedef struct bf_interpreter bf_interpreter_t;

bf_interpreter_t prog;

enum BF_REGISTER {
	BF_OPERATION,
	BF_OPERATION_ARG,
	BF_CMD_MR,
	BF_CMD_ARG,
	BF_SVC
};

/*
  La ram est préservée entre chaque invocation
  Les 5 premiers octets sont spéciaux:
  BF_OPERATION: un entier appartenant à command_t
  BF_OPERATION_ARG: un entier d'argument, utile selon la commande
  BF_CMD_SVC: un registre qui lance une requete lorsqu'il est incrémenté
  BF_CMD_MR: un registre qui contient la commande à exécuter lors d'un appel SVC
  BF_CMD_ARG: un registre qui contient l'argument de la commande à exécuter lors d'un appel SVC

  Les 39 octets qui suivent sont les propriété d'initialisation
  0: puissance moteur angulaire, en 5ieme d'unité
  1: puissance moteur linéaire, en 5ieme d'unité
  2: masse, en 5ieme d'unité
  3: ignoré
  4: bleu
  5: vert
  6: rouge
  7-38: nom
 */


void bf_run(bf_interpreter_t *self, command_t cmd, int arg) {
	int ip = self->ip = 0;
	int dp = self->dp = 0;
	char *ram = self->ram;
	char *rom = self->rom;
	memset(ram, 0, 5);
	ram[0] = cmd;
	ram[1] = arg;
	while (rom[ip] != 0 && rom[ip] != '|') {
		switch (rom[ip]) {
		case '>':
			++dp;
			break;
		case '<':
			--dp;
			break;
		case '+':
			++ram[dp];
			if (dp == BF_SVC) {
				switch (ram[BF_CMD_MR]) {
				case REQ_USE_ITEM:
					use_item(ram[BF_CMD_ARG]);
					break;
				default:
					break;
				}
				ram[dp] = 0;
			}
			break;
		case '-':
			--ram[dp];
			break;
		case '[':
			if (!ram[dp]) {
				int s = 1;
				while (s) {
					++ip;
					if (rom[ip] == '[')
						++s;
					else if (rom[ip] == ']')
						--s;
				}
			}
			break;
		case ']':
			if (ram[dp]) {
				int s = 1;
				while (s) {
					--ip;
					if (rom[ip] == ']')
						++s;
					else if (rom[ip] == '[')
						--s;
				}
			}
			break;
		default:
			break;
		}
		++ip;
	}
}

void init(int argc, char *argv[]) {
	(void)argc;
	prog.rom = readfile(argv[1], 0);
	char *d = prog.ram = calloc(8, 1024 * 1024);
	char *prop = d + 5;

	//int propsize = 39
	prop[0] = my_robot.angular_power * 5;
	prop[1] = my_robot.linear_power * 5;
	prop[2] = my_robot.mass * 5;
	*(int *)(prop + 3) = my_robot.color;
	memcpy(prop + 7, my_robot.name, 32);

	bf_run(&prog, CMD_INIT, 0);
	memcpy(&my_robot, d + 5, sizeof(my_robot));
	my_robot.angular_power = (float)prop[0] / 5.0f;
	my_robot.linear_power = (float)prop[1] / 5.0f;
	my_robot.mass = (float)prop[2] / 5.0f;
	my_robot.color = *(int *)(prop + 3);
	memcpy(my_robot.name, prop + 7, 32);
}

void int2charcpy(char *dst, void *srcp, size_t n) {
	size_t i = 0;
	int *src = srcp;
	while (i < n) {
		dst[i] = src[i] > 255 ? 255 : src[i];
		++i;
	}
}

void float2charcpy(char *dst, float *src, size_t n) {
	size_t i = 0;
	while (i < n) {
		dst[i] = src[i] > 255 ? 255 : src[i];
		++i;
	}
}

void update() {
	char *d = prog.ram;

	//reserved = 5
	//propsize = 39
	char *state = d + 5 + 39;

	state[0] = my_state.rot_eng_state;
	state[1] = my_state.lin_eng_state;
	state[2] = my_state.life > 255 ? 255 : (int)my_state.life;
	state[3] = my_state.score;
	state[4] = my_state.bag_size > 255 ? 255 : my_state.bag_size;
	state[5] = my_state.rays > 255 ? 255 : my_state.rays;

	memcpy(state + 6, my_state.bag, my_state.bag_size);
	int2charcpy(state + 6, my_state.bag, my_state.bag_size);
	float2charcpy(state + 6 + my_state.bag_size,
				  my_state.depth_buffer,
				  my_state.rays);
	int2charcpy(state + 6 + my_state.bag_size + my_state.rays,
				  my_state.obj_attr_buffer,
				  my_state.rays);

	bf_run(&prog, CMD_UPDATE, 0);
	
	// Si la cellule a overflow par soustraction, comme on pointe sur des char, on se retrouve avec une valeur negative ici, ce qui est voulu
	my_state.rot_eng_state = state[0];
	my_state.lin_eng_state = state[1];
}

void destroy() {
	bf_run(&prog, CMD_DESTROY, 0);
}

void item_collected(item_t i) {
	bf_run(&prog, CMD_COLLECT, i);
}

void collision(coll_t i) {
	bf_run(&prog, CMD_COLLISION, i);
}
