#ifndef PROCESS_H
#define PROCESS_H

#include <unistd.h>
#include <time.h>

/*
  Enumeration des possibles causes de morts
 */

typedef enum {
	D_HANGED = 1,
	D_EXITED,
	D_PROT,
	D_LOST,
	D_BROKEN
} death_reason_t;

/*
  Un processus avec ses entrée/sorties redirigé 
 */
typedef struct {
	pid_t pid;
	int stdin;
	int stdout;
} process_t;

/*
  Crée un processus en redirigeant son entrée/sortie sur des pipes
 */
int make_linked_process(process_t *ret, char *cmd);

/*
  Différence en nanosecondes entre deux points
 */
long nano_diff(struct timespec a, struct timespec b);


#endif /* PROCESS_H */