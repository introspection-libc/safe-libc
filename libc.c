#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <signal.h>
#include <errno.h>

#define INTERCEPTOR(x)	__introspection_##x
#define REAL(x)		x

#define LOG(...)	fprintf(stderr, __VA_ARGS__)

ssize_t _size_right(const void*);

/* link with --export-dynamic (-E) */
#include <execinfo.h>

#ifndef NO_STACKTRACE
#define	STACKTRACE_SIZE 1024
static void* stacktrace[STACKTRACE_SIZE];

static void __safec_print_stacktrace(void)
{
	int size = backtrace(stacktrace, STACKTRACE_SIZE);
	LOG("[backtrace] %d frames\n", size);
	if(size > 0) {
		int i;
		char** symbols = backtrace_symbols(stacktrace, size);
		if(symbols) {
			for(i = 0; i < size; i++)
			LOG("#%d %s\n", i, symbols[i]);
			free(symbols);
		} else {
			for(i = 0; i < size; i++)
			LOG("#%d %p\n", i, stacktrace[i]);
		}
	}
}
#else
static void __safec_print_stacktrace(void)
{
	LOG("[backtrace not enabled]\n");
}
#endif

#if 0
static void __safec_abort(void)
{
	__safec_print_stacktrace();
	raise(SIGABRT);
}
#endif

/* stdio */
char* INTERCEPTOR(gets)(char* s)
{
	int err = errno;
	ssize_t bufsz = _size_right(s);
	if(bufsz <= 1) {
		LOG("[gets] buffer too small to store any data (%zd)\n", bufsz);
		__safec_print_stacktrace();
		if(bufsz > 0)
			*s = 0;
		return NULL;
	}
	errno = err;
	return REAL(fgets)(s, bufsz, stdin);
}

char* INTERCEPTOR(fgets)(char* s, int size, FILE* stream)
{
	int err = errno;
	ssize_t bufsz = _size_right(s);
	int len = size;
	if(bufsz < 0) {
		LOG("[fgets] overflow: %zd byte(s)\n", -bufsz);
		__safec_print_stacktrace();
		return NULL;
	}
	if(bufsz < size) {
		LOG("[fgets] invalid size: %d vs %zu\n", size, bufsz);
		if(bufsz > INT_MAX) {
			LOG("[fgets] integer overflow\n");
			len = INT_MAX;
		} else {
			len = (int) bufsz;
		}
		__safec_print_stacktrace();
	}
	errno = err;
	return REAL(fgets)(s, len, stream);
}

/* strings */
size_t INTERCEPTOR(strlen)(const char* s)
{
	ssize_t bufsz = _size_right(s);
	if(bufsz < 0) {
		LOG("[strlen] overflow: %zd byte(s)\n", -bufsz);
		__safec_print_stacktrace();
		return 0;
	}
	return strnlen(s, bufsz);
}

size_t INTERCEPTOR(strnlen)(const char* s, size_t maxlen)
{
	ssize_t bufsz = _size_right(s);
	size_t len = maxlen;
	if(bufsz < 0) {
		LOG("[strnlen] overflow: %zd byte(s)\n", -bufsz);
		__safec_print_stacktrace();
		return 0;
	}
	if(bufsz < maxlen) {
		LOG("[strnlen] invalid maxlen: %zu vs %zu\n", maxlen, bufsz);
		len = bufsz;
		__safec_print_stacktrace();
	}
	return REAL(strnlen)(s, len);
}

char* INTERCEPTOR(strcpy)(char* dest, const char* src)
{
	size_t srcsz = INTERCEPTOR(strlen)(src);
	ssize_t dstsz = _size_right(dest);
	char* result;

	if(dstsz < 0) {
		LOG("[strcpy] overflow: %zd byte(s) (dest)\n", -dstsz);
		__safec_print_stacktrace();
		return dest;
	}

	if(dstsz < srcsz) {
		LOG("[strcpy] overflow: %zu vs %zu\n", srcsz, dstsz);
		__safec_print_stacktrace();
	}

    result = REAL(strncpy)(dest, src, dstsz);
    dest[srcsz] = 0;
    return result;
}

char* INTERCEPTOR(strncpy)(char* dest, const char* src, size_t n)
{
	ssize_t dstsz = _size_right(dest);
	size_t len = n;

	if(dstsz < 0) {
		LOG("[strncpy] overflow: %zd byte(s) (dest)\n", -dstsz);
		__safec_print_stacktrace();
		return dest;
	}
	if(dstsz < n) {
		LOG("[strncpy] overflow: %zu vs %zu\n", n, dstsz);
		len = dstsz;
		__safec_print_stacktrace();
	}

	return REAL(strncpy)(dest, src, len);
}

char* INTERCEPTOR(strcat)(char* dest, const char* src)
{
	ssize_t dstsz = _size_right(dest);
	size_t dstln = INTERCEPTOR(strlen)(dest);
	size_t srcln = INTERCEPTOR(strlen)(src);
	if(dstsz < 0) {
		LOG("[strcat] overflow: %zd byte(s) (dest)\n", -dstsz);
		__safec_print_stacktrace();
		return dest;
	}
	if(dstln + srcln + 1 > dstsz) {
		size_t n = dstsz - dstln - 1;
		LOG("[strcat] overflow: %zu vs %zu\n", dstln + srcln + 1, dstsz);
		__safec_print_stacktrace();
		return REAL(strncat)(dest, src, n);
	} else {
		/* enough space */
		return REAL(strcat)(dest, src);
	}
}

char* INTERCEPTOR(strncat)(char* dest, const char* src, size_t n)
{
	ssize_t dstsz = _size_right(dest);
	size_t dstln = INTERCEPTOR(strlen)(dest);
	size_t len = n;
	if(dstsz < 0) {
		LOG("[strncat] overflow: %zd byte(s) (dest)\n", -dstsz);
		__safec_print_stacktrace();
		return dest;
	}
	if(dstln + n + 1 > dstsz) {
		len = dstsz - dstln - 1;
		LOG("[strncat] invalid n: %zu vs %zu\n", n, len);
		__safec_print_stacktrace();
	}
	return REAL(strncat)(dest, src, n);
}

int INTERCEPTOR(strcmp)(const char* s1, const char* s2)
{
	ssize_t s1sz = _size_right(s1);
	ssize_t s2sz = _size_right(s2);
	ssize_t sz = s1sz < s2sz ? s1sz : s2sz;
	if(s1sz < 0 || s2sz < 0) {
		if(s1sz < 0) {
			LOG("[strcmp] overflow: %zd byte(s) (s1)\n", -s1sz);
		} else {
			LOG("[strcmp] overflow: %zd byte(s) (s2)\n", -s2sz);
		}
		__safec_print_stacktrace();
		return -1;
	}
	if(s1sz != s2sz) {
		int val = REAL(strncmp)(s1, s2, sz);
		int l1 = INTERCEPTOR(strlen)(s1);
		int l2 = INTERCEPTOR(strlen)(s2);
		if(l1 == l2) {
			return val;
		} else if(l1 < l2) {
			return !val ? -1 : val;
		} else { /* l1 > l2 */
			return !val ? 1 : val;
		}
	} else {
		return REAL(strncmp)(s1, s2, sz);
	}
}

int INTERCEPTOR(strncmp)(const char* s1, const char* s2, size_t n)
{
	ssize_t s1sz = _size_right(s1);
	ssize_t s2sz = _size_right(s2);
	ssize_t sz = s1sz < s2sz ? s1sz : s2sz;
	if(s1sz < 0 || s2sz < 0) {
		if(s1sz < 0) {
			LOG("[strncmp] overflow: %zd byte(s) (s1)\n", -s1sz);
		} else {
			LOG("[strncmp] overflow: %zd byte(s) (s2)\n", -s2sz);
		}
		__safec_print_stacktrace();
		return -1;
	}
	if(n > sz) {
		int val;
		int l1;
		int l2;

		LOG("[strncmp] invalid len: %zu vs %zu\n", n, sz);
		__safec_print_stacktrace();

		val = strncmp(s1, s2, sz);
		l1 = INTERCEPTOR(strlen)(s1);
		l2 = INTERCEPTOR(strlen)(s2);
		if(l1 == l2) {
			return val;
		} else if(l1 < l2) {
			return !val ? -1 : val;
		} else { /* l1 > l2 */
			return !val ? 1 : val;
		}
	} else {
		return REAL(strncmp)(s1, s2, n);
	}
}

/* memory functions */
void* INTERCEPTOR(memcpy)(void* dest, const void* src, size_t n)
{
	ssize_t dstsz = _size_right(dest);
	ssize_t srcsz = _size_right(src);
	size_t len = n;
	if(srcsz < 0 || dstsz < 0) {
		if(srcsz < 0) {
			LOG("[memcpy] overflow: %zu byte(s) (src)\n", -srcsz);
		} else {
			LOG("[memcpy] overflow: %zu byte(s) (dest)\n", -dstsz);
		}
		__safec_print_stacktrace();
		return dest;
	}
	if(dstsz < n) {
		LOG("[memcpy] invalid len: %zu vs %zd (dest)\n", n, dstsz);
		__safec_print_stacktrace();
		len = dstsz;
	}
	if(srcsz < len) {
		LOG("[memcpy] invalid len: %zu vs %zd\n (src)", n, srcsz);
		__safec_print_stacktrace();
		len = srcsz;
	}
	return REAL(memcpy)(dest, src, len);
}

int INTERCEPTOR(memcmp)(const void* s1, const void* s2, size_t n)
{
	ssize_t s1sz = _size_right(s1);
	ssize_t s2sz = _size_right(s2);
	ssize_t sz = s1sz < s2sz ? s1sz : s2sz;
	if(s1sz < 0 || s2sz < 0) {
		if(s1sz < 0) {
			LOG("[memcmp] overflow: %zu byte(s) (s1)\n", -s1sz);
		} else {
			LOG("[memcmp] overflow: %zu byte(s) (s2)\n", -s2sz);
		}
		return -1;
	}
	if(n > sz) {
		int val;
		int l1;
		int l2;

		LOG("[memcmp] invalid len: %zu vs %zu\n", n, sz);
		__safec_print_stacktrace();

		val = REAL(memcmp)(s1, s2, sz);
		l1 = INTERCEPTOR(strlen)(s1);
		l2 = INTERCEPTOR(strlen)(s2);
		if(l1 == l2) {
			return val;
		} else if(l1 < l2) {
			return !val ? -1 : val;
		} else { /* l1 > l2 */
			return !val ? 1 : val;
		}
	} else {
		return REAL(memcmp)(s1, s2, n);
	}
}

void* INTERCEPTOR(memset)(void* s, int c, size_t n)
{
	ssize_t sz = _size_right(s);
	size_t len = n;
	if(sz < 0) {
		LOG("[memset] overflow: %zu byte(s)\n", -sz);
		__safec_print_stacktrace();
		return s;
	}
	if(sz < n) {
		LOG("[memset] invalid len: %zu vs %zd\n", n, sz);
		__safec_print_stacktrace();
		len = sz;
	}
	return REAL(memset)(s, c, len);
}

/* UNIX IO */
ssize_t INTERCEPTOR(read)(int fd, void* buf, size_t count)
{
	int err = errno;
	ssize_t bufsz = _size_right(buf);
	size_t len = count;
	if(bufsz < 0) {
		LOG("[read] overflow: %zu byte(s)\n", -bufsz);
		__safec_print_stacktrace();
		return 0;
	}
	if(bufsz < len) {
		LOG("[read] invalid size: %zu vs %zu\n", count, bufsz);
		len = bufsz;
		__safec_print_stacktrace();
	}
	errno = err;
	return REAL(read)(fd, buf, len);
}

ssize_t INTERCEPTOR(write)(int fd, const void* buf, size_t count)
{
	int err = errno;
	ssize_t bufsz = _size_right(buf);
	size_t len = count;
	if(bufsz < 0) {
		LOG("[write] overflow: %zu byte(s)\n", -bufsz);
		__safec_print_stacktrace();
		return 0;
	}
	if(bufsz < len) {
		LOG("[write] invalid size: %zu vs %zu\n", count, bufsz);
		len = bufsz;
		__safec_print_stacktrace();
	}
	errno = err;
	return REAL(write)(fd, buf, len);
}
