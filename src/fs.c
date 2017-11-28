#include <robutts.h>
#include <assets.h>

char *readfile_fs(const char *name, unsigned *size) {
	struct stat s;
	char *ret;
	int fd;

	if (stat(name, &s) == -1)
		return 0;
	ret = calloc(1, s.st_size + 1);
	fd = open(name, O_RDONLY);
	exact_read(fd, ret, s.st_size);
	if (size)
		*size = s.st_size;
	close(fd);
	return ret;
}

char *readfile(const char *name, unsigned *size) {
	char *ptr = readfile_fs(name, size);
#ifdef ASSET_MANAGER
	embedded_t e;
#endif

	if (ptr)
		return ptr;
#ifdef ASSET_MANAGER
	free(ptr);
	e = get_asset(name);
	if (size)
		*size = e.size;
	ptr = malloc(e.size + 1);
	ptr[e.size] = 0;
	memcpy(ptr, e.data, e.size);
#endif
	return (char*)ptr;
}
