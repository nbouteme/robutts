#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <robutts.h>
#include <GLFW/glfw3.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>

robot_properties my_robot = {
	1.0f, 2.0f, 1.0f, 0x0FFF00, "default name"
};

int sfd;
struct in_addr sin_addr;
struct sockaddr_in addr;
socklen_t client_addr_size;
int client_sock;
struct pollfd fds;
void init(int argc, char *argv[]) {
	(void)argc;

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	inet_aton(argv[1], &sin_addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[2]));
	addr.sin_addr = sin_addr;

	connect(sfd, (void*)&addr, sizeof(addr));
	command_t cmd = CMD_INIT;
	write(sfd, &cmd, sizeof(cmd));
	read(sfd, &my_robot, sizeof(my_robot));

	fds = (struct pollfd){sfd, POLLOUT | POLLIN, 0};
}

void exec_subcmd_stream() {
	request_t r;
	int i;
	do {
		read(sfd, &r, sizeof(r));
		switch (r) {
		case REQ_UPDATE:
			read(sfd, &my_state, sizeof(int) * 2);
			break;
		case REQ_USE_ITEM:
			read(sfd, &i, sizeof(int));
			i = use_item(i);
			write(sfd, &i, sizeof(int));
			break;
		default:
			break;
		}
	} while (r != REQ_END);
}

void update() {
	poll(&fds, 1, 0);
	command_t c = CMD_UPDATE;
	write(sfd, &c, sizeof(c));
	if(fds.revents & POLLIN) {
		poll(&fds, 1, -1);
		exec_subcmd_stream();
	}
}

void update_state() {
	poll(&fds, 1, 0);
	if(fds.revents & POLLOUT) {
		command_t c = CMD_UPDATE_STATE;
		write(sfd, &c, sizeof(c));
		write(sfd, &my_state, sizeof(my_state));
		write(sfd, my_state.bag, sizeof(item_t) * my_state.bag_size);
		write(sfd, my_state.depth_buffer, sizeof(float) * my_state.rays);
		write(sfd, my_state.obj_attr_buffer, sizeof(int) * my_state.rays);
	}
}

void destroy() {	
	poll(&fds, 1, 0);
	if(fds.revents & POLLOUT && fds.revents & POLLIN) {
		command_t c = CMD_DESTROY;
		write(sfd, &c, sizeof(c));
		exec_subcmd_stream();
	}
	shutdown(sfd, SHUT_RDWR);
	close(sfd);
}

void item_collected(item_t i) {
	command_t c = CMD_COLLECT;
	write(sfd, &c, sizeof(c));
	write(sfd, &i, sizeof(i));
	exec_subcmd_stream();
}

void collision(coll_t i) {
	poll(&fds, 1, 0);
	if(fds.revents & POLLOUT && fds.revents & POLLIN) {
		command_t c = CMD_COLLISION;
		write(sfd, &c, sizeof(c));
		write(sfd, &i, sizeof(i));
		exec_subcmd_stream();
	}
}
