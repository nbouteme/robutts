#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <robutts.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <netinet/tcp.h>

#include <time.h>

// call this function to start a nanosecond-resolution timer
struct timespec timer_start(){
    struct timespec start_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
    return start_time;
}

long nano_diff(struct timespec a, struct timespec b);

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
	int broadcast = 1;

	(void)argc;
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &broadcast, sizeof(int));
	setsockopt(sfd, SOL_TCP, TCP_NODELAY, &broadcast, sizeof(int));
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
	unsigned count;
	int i;
	do {
		recv(sfd, &r, sizeof(r), MSG_PEEK | MSG_WAITALL);
		ioctl(sfd, FIONREAD, &count);
		switch (r) {
		case REQ_UPDATE:
			exact_read(sfd, &r, sizeof(r));
			exact_read(sfd, &my_state, sizeof(int) * 2);
			update_pending = 0;
			break;
		case REQ_USE_ITEM:
			recv(sfd, &r, sizeof(r), 0);
			exact_read(sfd, &i, sizeof(int));
			i = use_item(i);
			write(sfd, &i, sizeof(int));
			break;
		case REQ_END:
			recv(sfd, &r, sizeof(r), 0);
			break;
		default:
			recv(sfd, &r, sizeof(r), 0);
			break;
		}
	} while (r != REQ_END);
}

void update() {
	command_t c = CMD_UPDATE;
	poll(&fds, 1, 0);
	if (!update_pending && (fds.revents & POLLOUT)) {
		write(sfd, &c, sizeof(c));
		update_pending = 1;
	}
	if (update_pending && (fds.revents & POLLIN)) {
		exec_subcmd_stream();
	}
}

void update_state() {
	command_t c = CMD_UPDATE_STATE;
	if (update_pending)
		return;
	poll(&fds, 1, 0);
	if (!(fds.revents & POLLOUT))
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
