#define _GNU_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <robutts.h>
#include <process.h>
#include <poll.h>
#include <signal.h>
#include <sys/wait.h>

process_t subbot;

struct pollfd fds;

int make_socket_process(int s, process_t *ret, char *cmd) {
	ret->pid = fork();
	if (ret->pid == 0) { // child
		dup2(s, 0); // redirige l'entrée standard dans le pipe
		dup2(s, 1); // redirige la sortie ---------------
		execvp("sh", (char*[]){"sh", "-c", cmd, 0});
		return 0;
	}
	ret->stdin = s;
	ret->stdout = s;
	return 1;
}

int main(int argc, char *argv[argc])
{
	/*
	  ecouter 29295 -> donne ip server
	  connecter ip server -> autorisation de connexion -> port
	  creer process sur socket avec port
	  ???
	  profit
	*/

	
	int sfd;
	//struct in_addr sin_addr;
	struct sockaddr_in addr, o_addr;
	socklen_t client_addr_size = sizeof(addr);
	int client_sock;
	int broadcast = 1;
	int port;

	sfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	setsockopt(sfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(int));
	setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT, &broadcast, sizeof(int));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(29295);
	addr.sin_addr.s_addr = htons(INADDR_ANY);

	if (bind(sfd, (void*)&addr, sizeof(addr)) < 0) {
		perror("Failed to bind");
		return 1;
	}

	while (1) {
		printf("Waiting for a player slot to be available...\n");
		{
			int r = 0;
			int n = sizeof(port);
			char *buf = (void*)&port;
			while (r != sizeof(port)) {
				client_addr_size = sizeof(o_addr);
				int ret = recvfrom(sfd, &buf[r], n, 0, (void*)&o_addr, &client_addr_size);
				r += ret;
				n -= ret;
			}
		}
		if (port == 0) {
			puts("port was 0");
			continue;
		}
		printf("Connecting to port %d\n", port);
		client_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (client_sock < 0) {
			perror("Failed to create client socket");
			return 1;
		}			
		o_addr.sin_family = AF_INET;
		o_addr.sin_port = htons(port);
		printf("connect to 0x%#x\n", o_addr.sin_addr.s_addr);
		if (connect(client_sock, (void*)&o_addr, sizeof(o_addr)) < 0) {
			puts("The search continues...");
			usleep(250000);
			continue;
		}
		perror("Connect:");
		int do_you_love_me;
		fds = (struct pollfd){client_sock, POLLIN, 0};
		if (poll(&fds, 1, 250) == 0) {
			puts("The search continues...");
			usleep(250000);
			continue;
		}
		exact_read(client_sock, &do_you_love_me, sizeof(do_you_love_me));
		if (do_you_love_me) {
			break;
		}
		puts("The search continues...");
		usleep(250000);
	}
	printf("Connected!\n");
	make_socket_process(client_sock, &subbot, argv[1]);
	wait(0);
    return 0;
}
