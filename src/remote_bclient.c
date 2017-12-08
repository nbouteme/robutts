#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <robutts.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <poll.h>
#include <netinet/tcp.h>
#include <pthread.h>

robot_properties my_robot = {
	1.0f, 2.0f, 1.0f, 0x0FFF00, "default name"
};

int update_pending = 0;
int sfd;
struct pollfd fds;

int tcp;
struct sockaddr_in tcp_saddr;

void *refuse(void *args) {
	(void)args;
	int get_out_of_my_life_i_hate_you_you_f$cking_a$$hole = 0;
	while(1) {
		socklen_t csize = sizeof(tcp_saddr);
		int rfd = accept(tcp, (void*)&tcp_saddr, &csize);
		fprintf(stderr, "Sending a fuck-off to %d\n", rfd);
		if (rfd < 0)
			perror("Failed to fuck off");
		write(rfd, &get_out_of_my_life_i_hate_you_you_f$cking_a$$hole, sizeof(get_out_of_my_life_i_hate_you_you_f$cking_a$$hole));
		close(rfd);
	}
	return 0;
}

void init(int argc, char *argv[]) {
	/*
	  Broadcast le port argv[1] sur 29295
	  listen avec 1 connexion max
	  Attendre une connexion pendant 1 seconde
	  Si pas de connexion, rebroadcaster
	  sinon accepter la connexion
	  et faire le reste comme d'hab
	 */
	fprintf(stderr, "INIT CALLED\n");
	struct in_addr udp_inaddr, tcp_inaddr;
	struct sockaddr_in udp_saddr, tcp_saddr;
	int broadcast = 1;
	command_t cmd = CMD_INIT;
	//struct linger lo = { 1, 0 };

	(void)argc;
	int udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udp < 0) {
		perror("Failed to create UDP socket");
		exit(1);
	}
	inet_aton("255.255.255.255", &udp_inaddr);
	udp_saddr.sin_family = AF_INET;
	udp_saddr.sin_port = htons(29295);
	udp_saddr.sin_addr = udp_inaddr;

	setsockopt(udp, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(int));
	setsockopt(udp, SOL_SOCKET, SO_REUSEADDR, &broadcast, sizeof(int));
	setsockopt(udp, SOL_SOCKET, SO_REUSEPORT, &broadcast, sizeof(int));

	int port = atoi(argv[1]);

	tcp = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp < 0) {
		perror("Failed to create TCP socket");
		exit(1);
	}
	setsockopt(tcp, SOL_SOCKET, SO_REUSEADDR, &broadcast, sizeof(int));
	setsockopt(tcp, SOL_TCP, TCP_NODELAY, &broadcast, sizeof(int));
	//setsockopt(tcp, SOL_SOCKET, SO_LINGER, &lo, sizeof(lo));
	inet_aton("0.0.0.0", &tcp_inaddr);
	tcp_saddr.sin_family = AF_INET;
	tcp_saddr.sin_port = htons(atoi(argv[1]));
	tcp_saddr.sin_addr = tcp_inaddr;
	if (bind(tcp, (void*)&tcp_saddr, sizeof(tcp_saddr)) < 0) {
		perror("Failed to bind");
		exit(1);
	}
	if (listen(tcp, 0) < 0){
		perror("Failed to listed");
		exit(1);
	}
	fds = (struct pollfd){tcp, POLLIN, 0};
	while (1) {
		sendto(udp, &port, sizeof(port), 0, (void*)&udp_saddr, sizeof(udp_saddr));
		if (poll(&fds, 1, 250) > 0) {
			break;
		}
	}
	shutdown(udp, SHUT_RDWR);
	close(udp);
	socklen_t csize = sizeof(tcp_saddr);
	sfd = accept(tcp, (void*)&tcp_saddr, &csize);
	if (sfd < 0) {
		perror("Failed to accept main client");
		exit(1);
	}
	int i_lovey_ou = 1;
	write(sfd, &i_lovey_ou, sizeof(i_lovey_ou));
	pthread_t thread;
    pthread_create(&thread, NULL, refuse, NULL);

	fds = (struct pollfd){sfd, POLLIN | POLLOUT, 0};

	write(sfd, &cmd, sizeof(cmd));
	exact_read(sfd, &my_robot, sizeof(my_robot));
	fprintf(stderr, "wrote to client %s\n", argv[1]);
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
