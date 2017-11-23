#include <robutts.h>
#include <assert.h>
#include <assets.h>
#include <texture.h>

typedef struct {
	u16 first_entry;
	u16 map_length;
	u8 bpp;
} __attribute__ ((packed)) tga_color_map_desc;

typedef struct {
	u8 attr : 4;
	u8 res : 1;
	u8 screen_origin : 1;
} __attribute__ ((packed)) tga_image_desc_byte;

typedef struct {
	u16 x_origin;
	u16 y_origin;
	u16 width;
	u16 height;
	u8 bpp;
	u8 desc; // tga_image_desc_byte
} __attribute__ ((packed)) tga_image;

typedef struct {
	u8 id_length;
	u8 color_map_type;
	u8 image_type;
	tga_color_map_desc color_map_desc;	
	tga_image image;
} __attribute__ ((packed)) tga_header;

int load_tga(bitmap_t *self, const char *name) {
	unsigned s;
	char *f = readfile(name, &s);
	tga_header *tga = (void*)f;

	int succ = 0;
	if ((tga->id_length == 0) &&
		(tga->color_map_type == 0) &&
		(tga->image_type == 2) &&
		(tga->image.bpp == 32) &&
		((tga->image.desc & 0xF) == 0x8)) {

		self->width = tga->image.width;
		self->height = tga->image.height;
		u64 sb = (tga->image.bpp >> 3) * self->width * self->height;
		self->buff = malloc(sb);
		memcpy(self->buff, f + sizeof(tga_header), sb);
		succ = 1;
	}
	free(tga);
	return succ;
}

int make_texture(unsigned *out, bitmap_t tex) {
	glGenTextures(1, out);
	glBindTexture(GL_TEXTURE_2D, *out);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.width, tex.height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, tex.buff);
	return 1;
}
