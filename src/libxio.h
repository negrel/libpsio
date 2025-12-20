#include <stdbool.h>
#include <stddef.h>

#ifdef __unix__
#define XIO_UNIX
#endif
#ifdef __APPLE__
#define XIO_APPLE
#endif
#ifdef __POSIX__
#define XIO_POSIX
#endif
#ifdef _WIN32
#define XIO_WIN
#endif

/**
 * An object providing access to an open file on the filesystem.
 */
struct xio_file {
#if defined(XIO_POSIX) || defined(XIO_UNIX) || defined(XIO_APPLE)
	int fd;
#else
	HANDLE fd;
#endif
};

/**
 * Options and flags which can be used to configure how a file is opened.
 */
struct xio_open_options {
	bool append;
	bool create;
	bool create_new;
	bool read;
	bool truncate;
	bool write;
	// TODO: add platform specific extension.
};

/**
 * Open a file at `path` with the specified options.
 *
 * A negative error code is returned on error.
 */
int xio_open(char *path, struct xio_open_options opts, struct xio_file *file);

/**
 * Close opened file.
 *
 * A negative error code is returned on error.
 */
int xio_close(struct xio_file f);

/**
 * Reads some bytes from the file into the specified buffer, returning how many
 * bytes were read (at most INT_MAX bytes can be read in a single call).
 *
 * A negative error code is returned on error.
 */
int xio_read(struct xio_file f, void *buf, size_t count);

/**
 * Writes a buffer into the file, returning how many bytes were written (at most
 * INT_MAX bytes can be write in a single call).
 *
 * A negative error code is returned on error.
 */
int xio_write(struct xio_file f, void *buf, size_t count);
