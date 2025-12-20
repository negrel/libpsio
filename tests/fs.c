#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef LIBXIO_IMPLEMENTATION
#define LIBXIO_IMPLEMENTATION
#endif
#include "libxio.h"

#define TEST(fn)                                                               \
	do {                                                                   \
		int err = fn();                                                \
		if (err) {                                                     \
			printf("KO	%s: %d\n", #fn, err);                  \
			return err;                                            \
		} else {                                                       \
			printf("OK 	%s\n", #fn);                           \
		}                                                              \
	} while (0)

static int open_read_close(void)
{
	struct xio_open_options opts = {0};
	struct xio_file f;
	int err;
	char buf[1024];

	opts.read = true;
	err = xio_open("tests/data/hello.txt", opts, &f);
	if (err < 0)
		return -err;

	err = xio_read(f, buf, 1024);
	if (err < 0)
		return -err;

	if (err < 11)
		return EXIT_FAILURE;

	if (strncmp(buf, "Hello world!", 11) != 0)
		return 1;

	err = xio_close(f);
	if (err < 0)
		return -err;

	return 0;
}

static int open_write_close(void)
{
	struct xio_open_options opts = {0};
	struct xio_file f;
	int err;

	opts.create = true;
	opts.truncate = true;
	opts.write = true;
	err = xio_open("tests/data/write.txt", opts, &f);
	if (err < 0)
		return -err;

	err = xio_write(f, "Hello from fs.c", 15);
	if (err < 0)
		return -err;

	if (err != 15)
		return EXIT_FAILURE;

	err = xio_close(f);
	if (err < 0)
		return -err;

	return 0;
}

int main(void)
{
	TEST(open_read_close);
	TEST(open_write_close);
}
