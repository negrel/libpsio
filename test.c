#define _POSIX_C_SOURCE 200112L

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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

static uint64_t instant_nanos(void)
{
#ifdef _WIN32
	LARGE_INTEGER Time;
	QueryPerformanceCounter(&Time);

	static LARGE_INTEGER Frequency = {0};
	if (Frequency.QuadPart == 0) {
		QueryPerformanceFrequency(&Frequency);
	}

	uint64_t Secs = Time.QuadPart / Frequency.QuadPart;
	uint64_t Nanos = Time.QuadPart % Frequency.QuadPart *
			 NOB_NANOS_PER_SEC / Frequency.QuadPart;
	return 1000 * 1000 * 100 * Secs + Nanos;
#else
	struct timespec ts;
	int err = clock_gettime(CLOCK_MONOTONIC, &ts);
	if (err)
		abort();

	return 1000 * 1000 * 1000 * (uint64_t)ts.tv_sec + (uint64_t)ts.tv_nsec;
#endif // _WIN32
}

static int test_sleep_ms(void)
{
	uint64_t start, end;

	start = instant_nanos();

	// Sleep.
	psio_sleep_ms(100);

	end = instant_nanos();

	if (end - start < 100 * 1000 * 1000)
		return 1;

	return 0;
}

int main(void) { TRY(test_sleep_ms); }
