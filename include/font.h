#ifndef FONT_H
#define FONT_H

#include <texture.h>

typedef struct age_file age_file_t;

struct age_font {
	age_file_t *desc;
	bitmap_t bitmap;
};

typedef struct age_font age_font_t;

age_font_t *load_font(char *texfile, char *descfile);
void *make_string_bitmap(age_font_t *font, char *str, int *w, int *h);
#endif /* FONT_H */