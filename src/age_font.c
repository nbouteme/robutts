#include <robutts.h>
#include <font.h>
#include <assets.h>

struct age_symbol {
	unsigned id;
	float left;
	float top;
	float right;
	float bottom;
	float xoffset;
	float yoffset;
	float advance;
} __attribute__((packed));

typedef struct age_symbol age_symbol_t;

struct age_file {
	char header[4];
	unsigned count;
	float height;
	age_symbol_t symbols[];
} __attribute__((packed));

static age_symbol_t *get_symbol(age_file_t *font, char s) {
	for(unsigned i = 0; i < font->count; ++i) {
		if ((char)font->symbols[i].id == s) {
			return &font->symbols[i];
		}
	}
	return 0;
}

static age_file_t *load_age_file(char *fn) {
	age_file_t *ret = (void *)readfile(fn, 0);
	return ret;
}

age_font_t *load_font(char *texfile, char *descfile) {
	age_font_t *ret = malloc(sizeof(age_font_t));
	load_tga(&ret->bitmap, texfile);
	ret->desc = load_age_file(descfile);
	return ret;
}

static void blit_char(unsigned *buffer,
					  age_font_t *file,
					  age_symbol_t *sym,
					  int x, int y, int w);

static int max(int a, int b) {
	return a < b ? b : a;
}

/*
  Format 0xARGB
*/
void *make_string_bitmap(age_font_t *font, char *str, int *w, int *h) {
	char *s = str;
	int width = 0;
	int cwidth = 0;
	int cheight = 0;
	int height = 0;
	int first = 0;
	age_symbol_t *sym;
	void *m;
	int x;
	int y;
	while (*s) {
		if (*s == '\n') {
			++s;
			width = max(width, cwidth);
			height += cheight;
			cwidth = 0;
			continue;
		}
		sym = get_symbol(font->desc, *s);
		if (sym) {
			if (first)
				if (sym->xoffset < 0)
					width += -sym->xoffset;
			cwidth += sym->advance;
			cheight = max(cheight, (sym->bottom - sym->top) + sym->yoffset);
			++s;
		}
	}
	width = max(width, cwidth);
	height += cheight;
	
	*w = width;
	*h = height;

	m = calloc(sizeof(unsigned), width * height);
	s = str;
	x = 0;
	y = 0;
	while (*s) {
		if (*s == '\n') {
			++s;
			x = 0;
			y += font->desc->height;
			continue;
		}
		sym = get_symbol(font->desc, *s);
		blit_char(m, font, sym, x, y, width);
		if (sym)
			x += sym->advance;
		++s;
	}

	return m;
}

static void blit_char(unsigned *buffer,
					  age_font_t *file,
					  age_symbol_t *sym,
					  int x, int y, int w) {
	int i, j, dy, dx, sy, sx;
	y += sym->yoffset;
	x += sym->xoffset;
	for (i = 0; i < sym->bottom - sym->top; ++i) {
		for (j = 0; j < sym->right - sym->left; ++j) {
			dy = y + i;
			dx = x + j;
			sy = sym->top + i;
			sx = sym->left + j;
			buffer[dy * w + dx] = file->bitmap.buff[sy * file->bitmap.width + sx];
		}
	}
}
