#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <time.h>

#define LIBPSIO_IMPLEMENTATION
#include "libpsio.h"

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
	struct timespec start, end;
	time_t diff_ms;

	err = clock_gettime(CLOCK_MONOTONIC, &start);
	if (err)
		return err;

	// Sleep.
	psio_sleep_ms(100);

	err = clock_gettime(CLOCK_MONOTONIC, &end);
	if (err)
		return err;

	diff_ms = (end.tv_sec - start.tv_sec) * 1000;
	diff_ms += (end.tv_nsec - start.tv_nsec) / (1000 * 1000);

	if (diff_ms < 100)
		return 1;

	return 0;
}

int main(void) { TRY(test_sleep_ms); }
