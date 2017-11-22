#include <robutts.h>
#include <assets.h>

typedef struct file_ent {
	char *filename;
	long size;
} file_ent_t;

extern file_ent_t _filetable_start;
extern file_ent_t _filetable_end;

embedded_t get_asset(const char *name) {
	file_ent_t *files = &_filetable_start;
	while (files != &_filetable_end) {
		if (strcmp(files->filename, name) == 0)
			return (embedded_t) {
				(u8*)(files->filename + strlen(name) + 1),
				files->size
			};
		++files;
	}
	abort();
	return (embedded_t){0, 0};
}