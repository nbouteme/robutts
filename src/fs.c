#include <robutts.h>

char *readfile_fs(const char *name, unsigned *size) {
	struct stat s;

	if (stat(name, &s) == -1)
		return 0;
	char *ret = calloc(1, s.st_size + 1);
	int fd = open(name, O_RDONLY);
	read(fd, ret, s.st_size);
	if (size)
		*size = s.st_size;
	close(fd);
	return ret;
}

char *readfile(const char *name, unsigned *size) {
	char *ptr = readfile_fs(name, size);
	if (ptr)
		return ptr;
#ifdef ASSET_MANAGER
	embedded_t e = get_asset(name);
	if (size)
		*size = e.size;
	ptr = malloc(e.size + 1);
	ptr[e.size] = 0;
	memcpy(ptr, e.data, e.size);
#endif
	return (char*)ptr;
}
