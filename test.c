#include <stdint.h>
#include <stdio.h>

#ifndef PSIO_IMPLEMENTATION
#define PSIO_IMPLEMENTATION
#endif
#include "libpsio.h"

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

	err = psio_instant_now(&start);
	if (err)
		return err;

	// Sleep.
	psio_sleep_ms(100);

	err = psio_instant_now(&end);
	if (err)
		return err;

	if (end - start < 100 * 1000 * 1000)
		return 1;

	return 0;
}

int main(void) { TRY(test_sleep_ms); }
