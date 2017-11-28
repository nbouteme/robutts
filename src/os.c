#include <robutts.h>
#include <process.h>

long nano_diff(struct timespec a, struct timespec b) {
	long acc = 0;
	// Surement, on va pas overflow? Haha. Fuck GLUT.
	acc += a.tv_sec * 1000000000 + a.tv_nsec;
	acc -= b.tv_sec * 1000000000 + b.tv_nsec;
	return acc;
}

int exact_read(int fd, void *buff, size_t n) {
	int state;
	int lefttoread = n;
	while (lefttoread) {
		state = read(fd, buff, lefttoread);
		if (state == -1)
			return -1;
		if (state == 0)
			break;
		lefttoread -= state;
	}
	return n - lefttoread;
}

int make_linked_process(process_t *ret, char *cmd) {
	int readpipe[2];
	int writepipe[2];

	pipe(readpipe);
	pipe(writepipe);

	ret->pid = fork();
	if (ret->pid == 0) { // child
		close(readpipe[1]);
		close(writepipe[0]);
		dup2(readpipe[0], 0); // redirige l'entrée standard dans le pipe
		dup2(writepipe[1], 1); // redirige la sortie ---------------
		execvp("sh", (char*[]){"sh", "-c", cmd, 0});
		return 0;
	}
	close(readpipe[0]);
	close(writepipe[1]);
	ret->stdin = readpipe[1];
	ret->stdout = writepipe[0];
	return 1;
}
