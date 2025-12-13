#include <stdint.h>
#include <stdio.h>

#ifndef XIO_IMPLEMENTATION
#define XIO_IMPLEMENTATION
#endif
#include "libxio.h"

#ifdef _WIN32
#endif

#define TRY(fn)                                                                \
	do {                                                                   \
		int err = fn();                                                \
		if (err) {                                                     \
			printf("KO	%s: %d\n", #fn, err);                  \
			return err;                                            \
		} else {                                                       \
			printf("OK 	%s\n", #fn);                           \
		}                                                              \
	} while (0)

static int test_sleep_ms(void)
{
	int err;
	uint64_t start, end;

	err = xio_instant_now(&start);
	if (err)
		return err;

	// Sleep.
	xio_sleep_ms(100);

	err = xio_instant_now(&end);
	if (err)
		return err;

	if (end - start < 100 * 1000 * 1000)
		return 1;

	return 0;
}

int main(void) { TRY(test_sleep_ms); }
