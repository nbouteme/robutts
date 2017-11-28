#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <robutts.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>

robot_properties my_robot = {
	1.0f, 2.0f, 1.0f, 0x0FFF00, "default name"
};

int update_pending = 0;
int sfd;

struct pollfd fds;

void init(int argc, char *argv[]) {
	struct in_addr sin_addr;
	struct sockaddr_in addr;
	command_t cmd = CMD_INIT;

	(void)argc;
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	inet_aton(argv[1], &sin_addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[2]));
	addr.sin_addr = sin_addr;

	connect(sfd, (void*)&addr, sizeof(addr));
	fds = (struct pollfd){sfd, POLLIN | POLLOUT, 0};

	write(sfd, &cmd, sizeof(cmd));
	exact_read(sfd, &my_robot, sizeof(my_robot));
}

void exec_subcmd_stream() {
	request_t r = 0;
	int i;
	do {
		exact_read(sfd, &r, sizeof(r));
		switch (r) {
		case REQ_UPDATE:
			exact_read(sfd, &my_state, sizeof(int) * 2);
			break;
		case REQ_USE_ITEM:
			exact_read(sfd, &i, sizeof(int));
			i = use_item(i);
			write(sfd, &i, sizeof(int));
			break;
		case REQ_END:
			break;
		default:
			fprintf(stderr, "Unknown req from serv END %#x %#x\n", REQ_END, r);
			break;
		}
	} while (r != REQ_END);
}

void update() {
	command_t c = CMD_UPDATE;
	poll(&fds, 1, 0);
	if (!update_pending) {
		write(sfd, &c, sizeof(c));
		update_pending = 1;
	}
	if (fds.revents & POLLIN) {
		exec_subcmd_stream();
		update_pending = 0;
	}
}

void update_state() {
	command_t c = CMD_UPDATE_STATE;
	if (update_pending)
		return;
	write(sfd, &c, sizeof(c));
	write(sfd, &my_state, sizeof(my_state));
	write(sfd, my_state.bag, sizeof(item_t) * my_state.bag_size);
	write(sfd, my_state.depth_buffer, sizeof(float) * my_state.rays);
	write(sfd, my_state.obj_attr_buffer, sizeof(int) * my_state.rays);
}

void destroy() {
	command_t c = CMD_DESTROY;
	if (update_pending)
		return;
	write(sfd, &c, sizeof(c));
	//poll(&fds[0], 1, 0);
	exec_subcmd_stream();
	shutdown(sfd, SHUT_RDWR);
	close(sfd);
}
