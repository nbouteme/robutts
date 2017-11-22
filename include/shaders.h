#ifndef SHADERS_H
#define SHADERS_H

/*
  Crée un shader du type spécifié à partir du nom de fichier donné en parametre
  Renvoie un entier qui référence le programme crée.
 */
int load_shader(const char *filename, int type);

/*
  Crée et link un shader complet à partir du vertex shader et fragment shader
  Renvoie un entier qui référence le programme crée.  
 */
int make_shader(unsigned v, unsigned f);

#endif /* SHADERS_H */