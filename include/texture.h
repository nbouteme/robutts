#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct {
	int width;
	int height;
	unsigned *buff;
} bitmap_t;

/*
  Charge un fichier TGA 32 bits non-compressé à partir du nom de fichier.
  Renvoie 0 si le chargement a échoué.
 */
int load_tga(bitmap_t *self, const char *name);

/*
  Crée une texture à partir d'un bitmap. Écrit l'id de la texture dans la variable pointée par out
  Renvoie 0 si le chargement a échoué.  
 */
int make_texture(unsigned *out, bitmap_t tex);


#endif /* TEXTURE_H */