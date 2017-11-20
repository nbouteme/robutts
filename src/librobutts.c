#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "librobutts.h"

void init(int, char *[])	__attribute__ ((weak, alias("do_nothing")));
void update_state()			__attribute__ ((weak, alias("do_nothing")));
void update()				__attribute__ ((weak, alias("do_nothing")));
void destroy()				__attribute__ ((weak, alias("do_nothing")));
void item_collected(item_t)	__attribute__ ((weak, alias("do_nothing")));
void collision(coll_t)		__attribute__ ((weak, alias("do_nothing")));

void do_nothing() {
}

robot_state my_state;

robot_properties my_robot __attribute__ ((weak)) = {
	1.0f,
	1.0f,
	10.0f,
	0x00FF00FF,
	"Default bot"
};

int use_item(int idx) {
	request_t r = REQ_USE_ITEM;
	if (write(1, &r, sizeof(r)) == -1)
		perror("client write failed");
	if (write(1, &idx, sizeof(idx)) == -1)
		perror("client write2 failed");
	if (read(0, &idx, sizeof(idx)) == -1) {
		perror("client read failed");
	}
	return idx;
}

int main(int argc, char *argv[argc])
{
	command_t cmd;
	memset(&my_state, 0, sizeof(my_state));

	while (read(0, &cmd, sizeof(cmd))) {
		item_t t;
		coll_t c;
		request_t r;
		static int in = 0;
		switch(cmd) {
		case CMD_INIT:
			if (in)
				break;
			in = 1;
			init(argc, argv);
			write(1, &my_robot, sizeof(my_robot));
			break;
		case CMD_UPDATE_STATE:

			/* TODO: recycler plutot que liberer */
			free(my_state.bag);
			free(my_state.depth_buffer);
			free(my_state.obj_attr_buffer);
			
			read(0, &my_state, sizeof(my_state));

			my_state.bag				= malloc(sizeof(item_t) * my_state.bag_size);
			my_state.depth_buffer		= malloc(sizeof(float) * my_state.rays);
			my_state.obj_attr_buffer	= malloc(sizeof(int) * my_state.rays);

			read(0, my_state.bag, sizeof(item_t) * my_state.bag_size);
			read(0, my_state.depth_buffer, sizeof(float) * my_state.rays);			
			read(0, my_state.obj_attr_buffer, sizeof(int) * my_state.rays);
			update_state();
			break;
		case CMD_UPDATE:
			update();

			r = REQ_UPDATE;
			write(1, &r, sizeof(r));
			write(1, &my_state, sizeof(int) * 2);
			r = REQ_END;
			write(1, &r, sizeof(r));
			break;
		case CMD_DESTROY:
			destroy();
			r = REQ_END;
			write(1, &r, sizeof(r));
			break;
		case CMD_COLLECT:
			read(0, &t, sizeof(t));
			item_collected(t);
			r = REQ_END;
			write(1, &r, sizeof(r));
			break;
		case CMD_COLLISION:
			read(0, &c, sizeof(c));
			collision(c);
			r = REQ_END;
			write(1, &r, sizeof(r));
			break;
		default:
			fputs("Unknown command received", stderr);
		}
	}
	fflush(stderr);
    return 0;
}
