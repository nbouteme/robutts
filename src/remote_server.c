#define _GNU_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <robutts.h>
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

void child_died(int s) {
	(void)s;
	wait(0);
	exit(0);
}

int main(int argc, char *argv[argc])
{
	struct sigaction s;
	sigset_t ss;
	sigemptyset(&ss);
	s.sa_handler = child_died;
	s.sa_mask = ss;
	s.sa_flags = 0;
	sigaction(SIGCHLD, &s, 0);

	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	struct in_addr sin_addr;
	inet_aton(argv[1], &sin_addr);
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[2]));
	addr.sin_addr = sin_addr;
	bind(sfd, (void*)&addr, sizeof(addr));
	listen(sfd, 2);
	socklen_t client_addr_size;
	int client_sock = accept(sfd, (void*)&addr, &client_addr_size);

	make_socket_process(client_sock, &subbot, argv[3]);
	pause();
    return 0;
}
