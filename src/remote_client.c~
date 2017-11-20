#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <robutts.h>
#include <GLFW/glfw3.h>

robot_properties my_robot = {
	1.0f, 2.0f, 1.0f, 0x0FFF00, "default name"
};

int sfd;
struct in_addr sin_addr;
struct sockaddr_in addr;
socklen_t client_addr_size;
int client_sock;

process_t subbot;

void init(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	inet_aton(argv[1], &sin_addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[2]));
	addr.sin_addr = sin_addr;

	if (argc == 3) {

	} else {
		bind(sfd, (void*)&addr, sizeof(addr));
		listen(sfd, 2);
		client_sock = accept(sfd, (void*)&addr, &client_addr_size);

		make_linked_process(&subbot, argv[1]);
		command_t cmd = CMD_INIT;
		write(subbot.stdin, &cmd, sizeof(cmd));
		read(subbot.stdout, &my_robot, sizeof(my_robot));
	}
}

void exec_subcmd_stream() {
	request_t r;
	int i;
	do {
		read(subbot.stdout, &r, sizeof(r));
		switch (r) {
		case REQ_UPDATE:
			read(subbot.stdout, &my_state, sizeof(int) * 2);
			break;
		case REQ_USE_ITEM:
			read(subbot.stdout, &i, sizeof(int));
			i = use_item(i);
			write(subbot.stdin, &i, sizeof(int));
			break;
		default:
			break;
		}
	} while (r != REQ_END);
}

void update() {
	c = CMD_UPDATE;
	write(subbot.stdin, &c, sizeof(c));
	exec_subcmd_stream();
}

void update_state() {
	command_t c = CMD_UPDATE_STATE;

	write(subbot.stdin, &c, sizeof(c));
	write(subbot.stdin, &my_state, sizeof(my_state));
	write(subbot.stdin, my_state.bag, sizeof(item_t) * my_state.bag_size);
	write(subbot.stdin, my_state.depth_buffer, sizeof(float) * my_state.rays);
	write(subbot.stdin, my_state.obj_attr_buffer, sizeof(int) * my_state.rays);
}

void destroy() {	
	command_t c = CMD_DESTROY;
	write(subbot.stdin, &c, sizeof(c));
	exec_subcmd_stream();
	close(client_sock);
	shutdown(sfd, SHUT_RDWR);
	close(sfd);
}

void item_collected(item_t i) {
	command_t c = CMD_COLLECT;
	write(subbot.stdin, &c, sizeof(c));
	write(subbot.stdin, &i, sizeof(i));
	exec_subcmd_stream();
}

void collision(coll_t i) {
	command_t c = CMD_COLLISION;
	write(subbot.stdin, &c, sizeof(c));
	write(subbot.stdin, &i, sizeof(i));
	exec_subcmd_stream();
}
