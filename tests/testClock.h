// source: https://stackoverflow.com/questions/1861294/how-to-calculate-execution-time-of-a-code-snippet-in-c

#ifndef TESTCLOCK_H
#define TESTCLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif
#include <stdint.h>

/* Remove if already defined */
typedef int64_t int64; 
typedef uint64_t uint64;

/* Returns the amount of milliseconds elapsed since the UNIX epoch. Works on both
 * windows and linux. */

uint64 GetTimeMs64(void);

#ifdef __cplusplus
}
#endif

#endif /* TESTCLOCK_H */