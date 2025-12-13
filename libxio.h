/**
 * Cross platform I/O library.
 *
 * MIT License
 *
 * Copyright (c) 2024 Alexandre Negrel
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef LIBXIO_H_INCLUDE
#define LIBXIO_H_INCLUDE

#include <stdint.h>

#define UNSUPPORTED_PLATFORM                                                   \
	"Unsupported platform! This code is meant for Windows or POSIX "       \
	"compliant systems."

#ifdef _WIN32
#define XIO_WIN
#else
#if defined(__unix__) || defined(__APPLE__) || defined(__POSIX__)
#define XIO_POSIX
#endif /* defined(__unix__) || defined(__APPLE__) || defined(__POSIX__) */
#endif /* _WIN32 */

#define XIO_NANOS_PER_SEC (1000 * 1000 * 1000)

/**
 * Returns a measurement (in nanoseconds) of a monotonically non decreasing
 * clock. It only allows measuring the duration between two instants (or
 * comparing two instants).
 *
 * A negative errno is returned in case of error.
 */
int xio_instant_now(uint64_t *now);

/**
 * Causes the calling thread to sleep until the number of real-time
 * milliseconds have elapsed.
 *
 * A negative errno is returned in case of error.
 */
int xio_sleep_ms(uint32_t ms);

#ifdef XIO_IMPLEMENTATION

#include <errno.h>

#ifdef XIO_POSIX
#include <time.h>
#elif defined(XIO_WIN)
#include <windows.h>

#include <profileapi.h>
#include <synchapi.h>
#endif /* XIO_POSIX */

int xio_instant_now(uint64_t *now)
{
#ifdef XIO_POSIX
	int err;
	struct timespec tp;
	err = clock_gettime(CLOCK_MONOTONIC, &tp);
	if (err)
		return -errno;
	*now = (uint64_t)tp.tv_sec * XIO_NANOS_PER_SEC;
	*now += (uint64_t)tp.tv_nsec;
	return 0;
#elif defined(XIO_WIN)
	LARGE_INTEGER Time, Frequency;
	uint64_t time, frequency;

	if (!QueryPerformanceCounter(&Time))
		return -ENOTSUP;
	time = (uint64_t)Time.QuadPart;

	if (!QueryPerformanceFrequency(&Frequency))
		return -ENOTSUP;
	frequency = (uint64_t)Frequency.QuadPart;

	if (frequency == 0)
		return -ERANGE;

	*now = time / frequency * XIO_NANOS_PER_SEC;
	*now += time % frequency * XIO_NANOS_PER_SEC / frequency;

	return 0;
#else
#error UNSUPPORTED_PLATFORM
#endif
}

int xio_sleep_ms(uint32_t ms)
{
#ifdef XIO_POSIX
	int err;
	struct timespec rq, rm;

	rq.tv_sec = ms / 1000;
	rq.tv_nsec = (ms % 1000) * 1000 * 1000;
	while (1) {
		rm = (struct timespec){0};
		err = nanosleep(&rq, &rm);
		switch (err) {
		case 0:
			return 0;
		case EINTR:
			rq = rm;
			continue;
		default:
			return -err;
		}
	}
#elif defined(XIO_WIN)
	Sleep(ms);
	return 0;
#else
#error UNSUPPORTED_PLATFORM
#endif /* XIO_POSIX */
}

#endif /* XIO_IMPLEMENTATION */

#undef UNSUPPORTED_PLATFORM

#endif /* LIBXIO_H_INCLUDE */
