#ifndef ASSETS_H
#define ASSETS_H

typedef struct embedded_file {
	unsigned char *data;
	long size;
} embedded_t;

/*
  Lis un fichier entier, et écrit la taille dans la variable pointée par size
 */
char *readfile(const char *name, unsigned *size);

embedded_t get_asset(const char *name);

#endif /* ASSETS_H */