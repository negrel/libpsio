/**
 * Portable synchronous I/O library.
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
 **/

#ifndef LIBPSIO_H_INCLUDE
#define LIBPSIO_H_INCLUDE

#include <stdint.h>

#define UNSUPPORTED_PLATFORM                                                   \
	"Unsupported platform! This code is meant for Windows or POSIX "       \
	"compliant systems."

#ifdef _WIN32
#define LIBPSIO_WIN
#else
#if defined(__unix__) || defined(__APPLE__) || defined(__POSIX__)
#define LIBPSIO_POSIX
#endif /* defined(__unix__) || defined(__APPLE__) || defined(__POSIX__) */
#endif /* _WIN32 */

/**
 * Causes the calling thread to sleep until the number of real-time
 * milliseconds have elapsed.
 *
 * A negative errno is returned in case of error.
 */
int psio_sleep_ms(uint32_t ms);

#ifdef LIBPSIO_IMPLEMENTATION

#ifdef LIBPSIO_POSIX
#include <errno.h>
#include <time.h>
#elif defined(LIBPSIO_WIN)
#include <synchapi.h>
#endif /* LIBPSIO_POSIX */

int psio_sleep_ms(uint32_t ms)
{
#ifdef LIBPSIO_POSIX
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
#elif defined(LIBPSIO_WIN)
	Sleep(ms);
	return 0;
#else
#error UNSUPPORTED_PLATFORM
#endif /* LIBPSIO_POSIX */
}

#endif /* LIBPSIO_IMPLEMENTATION */

#undef UNSUPPORTED_PLATFORM

#endif /* LIBPSIO_H_INCLUDE */
