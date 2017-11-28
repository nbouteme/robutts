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
	int sfd;
	struct in_addr sin_addr;
	struct sockaddr_in addr;
	socklen_t client_addr_size = sizeof(addr);
	int client_sock;

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	inet_aton(argv[1], &sin_addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[2]));
	addr.sin_addr = sin_addr;
	bind(sfd, (void*)&addr, sizeof(addr));
	listen(sfd, 2);
	client_sock = accept(sfd, (void*)&addr, &client_addr_size);
	make_socket_process(client_sock, &subbot, argv[3]);
	wait(0);
    return 0;
}
