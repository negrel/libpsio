#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>

#include "libxio.h"

#define TRY(errno)                                                             \
	if (errno < 0)                                                         \
	return errno

#define MODE_PERM S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH

static inline int get_access_mode(struct xio_open_options o)
{
	if (o.read && !o.write && !o.append)
		return O_RDONLY;
	if (!o.read && o.write && !o.append)
		return O_WRONLY;
	if (o.read && o.write && !o.append)
		return O_RDWR;
	if (!o.read /* && o.write */ && o.append)
		return O_WRONLY | O_APPEND;
	if (o.read /* && o.write */ && o.append)
		return O_RDWR | O_APPEND;
	// if (!o.read && !o.write && !o.append)
	return -EINVAL;
}

static inline int get_creation_mode(struct xio_open_options o)
{
	if (!o.write && !o.append) {
		if (o.truncate && o.create && o.create_new) {
			// Creating or truncating a file requires write or
			// append access.
			return -EINVAL;
		}
	}
	if (!o.write && o.append) {
		if (o.truncate && !o.create_new) {
			// Creating or truncating a file requires write or
			// append access.
			return -EINVAL;
		}
	}

	if (!o.create && !o.truncate && !o.create_new)
		return 0;
	if (o.create && !o.truncate && !o.create_new)
		return O_CREAT;
	if (!o.create && o.truncate && !o.create_new)
		return O_TRUNC;
	if (o.create && o.truncate && !o.create_new)
		return O_CREAT | O_TRUNC;
	// if (/* o.create && o.truncate && */ o.create_new)
	return O_CREAT | O_EXCL;
}

int xio_open(char *path, struct xio_open_options opts, struct xio_file *file)
{
	int access_mode, creation_mode, flags, fd;

	if (!file)
		return -EINVAL;

	access_mode = get_access_mode(opts);
	TRY(access_mode);

	creation_mode = get_creation_mode(opts);
	TRY(creation_mode);

	flags = O_CLOEXEC | access_mode | creation_mode;

	// TODO: add mode support.
	while (true) {
		fd = open(path, flags, MODE_PERM);
		if (fd > 0) {
			file->fd = fd;
			return 0;
		} else if (errno != EINTR)
			return -errno;
	}
}

int xio_close(struct xio_file f)
{
	while (true) {
		if (close(f.fd) == -1) {
			if (errno != EINTR)
				return -errno;
		} else
			return 0;
	}
}

int xio_read(struct xio_file f, void *buf, size_t count)
{
	ssize_t r;

	if (!buf)
		return -EINVAL;

	if (count == 0)
		return 0;

	while (true) {
		r = read(f.fd, buf, count > INT_MAX ? INT_MAX : count);
		if (r > 0)
			return (int)r;
		else if (errno != EINTR)
			return -errno;
	}
}

int xio_write(struct xio_file f, void *buf, size_t count)
{
	ssize_t w;

	if (!buf)
		return -EINVAL;

	if (count == 0)
		return 0;

	while (true) {
		w = write(f.fd, buf, count > INT_MAX ? INT_MAX : count);
		if (w > 0)
			return (int)w;
		else if (errno != EINTR)
			return -errno;
	}
}

#undef TRY
#undef MODE_PERM
