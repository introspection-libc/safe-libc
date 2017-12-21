#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <signal.h>

ssize_t _size_right(const void*);

/* link with --export-dynamic (-E) */
#include <execinfo.h>

#define	STACKTRACE_SIZE 1024
static void* stacktrace[STACKTRACE_SIZE];

static void __safec_print_stacktrace(void)
{
	int size = backtrace(stacktrace, STACKTRACE_SIZE);
	fprintf(stderr, "[backtrace] %d frames\n", size);
	if(size > 0) {
		int i;
		char** symbols = backtrace_symbols(stacktrace, size);
		if(symbols) {
			for(i = 0; i < size; i++)
			fprintf(stderr, "#%d %s\n", i, symbols[i]);
			free(symbols);
		} else {
			for(i = 0; i < size; i++)
			fprintf(stderr, "#%d %p\n", i, stacktrace[i]);
		}
	}
}

static void __safec_abort(void)
{
	__safec_print_stacktrace();
	raise(SIGABRT);
}

// stdio
char* __safe_gets(char* s)
{
	ssize_t bufsz = _size_right(s);
	if(bufsz <= 1) {
		fprintf(stderr, "[gets] buffer too small to store any data (%zd)\n", bufsz);
		__safec_print_stacktrace();
		if(bufsz > 0)
			*s = 0;
		return NULL;
	}
	return fgets(s, bufsz, stdin);
}

char* __safe_fgets(char* s, int size, FILE* stream)
{
	ssize_t bufsz = _size_right(s);
	int len = size;
	if(bufsz < 0) {
		fprintf(stderr, "[fgets] overflow: %zd byte(s)\n", -bufsz);
		__safec_abort();
	}
	if(bufsz < size) {
		fprintf(stderr, "[fgets] invalid size: %d vs %zu\n", size, bufsz);
		if(bufsz > INT_MAX) {
			fprintf(stderr, "[fgets] integer overflow\n");
			len = INT_MAX;
		} else {
			len = (int) bufsz;
		}
		__safec_print_stacktrace();
	}
	return fgets(s, len, stream);
}

// strings
size_t __safe_strlen(const char* s)
{
	ssize_t bufsz = _size_right(s);
	if(bufsz < 0) {
		fprintf(stderr, "[strlen] overflow: %zd byte(s)\n", -bufsz);
		return 0;
	}
	return strnlen(s, bufsz);
}

size_t __safe_strnlen(const char* s, size_t maxlen)
{
	ssize_t bufsz = _size_right(s);
	size_t len = maxlen;
	if(bufsz < 0) {
		fprintf(stderr, "[strnlen] overflow: %zd byte(s)\n", -bufsz);
		return 0;
	}
	if(bufsz < maxlen) {
		fprintf(stderr, "[strnlen] invalid maxlen: %zu vs %zu\n", maxlen, bufsz);
		len = bufsz;
		__safec_print_stacktrace();
	}
	return strnlen(s, len);
}

char* __safe_strcpy(char* dest, const char* src)
{
	size_t srcsz = __safe_strlen(src);
	ssize_t dstsz = _size_right(dest);
	char* result;

	if(dstsz < 0) {
		fprintf(stderr, "[strcpy] overflow: %zd byte(s) (dest)\n", -dstsz);
		__safec_abort();
	}
	if(dstsz < srcsz) {
		fprintf(stderr, "[strcpy] overflow: %zu vs %zu\n", srcsz, dstsz);
		__safec_print_stacktrace();
		result = strncpy(dest, src, dstsz);
		dest[dstsz - 1] = 0;
		return result;
	} else {
		// enough space
		return strcpy(dest, src);
	}
}

char* __safe_strncpy(char* dest, const char* src, size_t n)
{
	ssize_t dstsz = _size_right(dest);
	size_t len = n;

	if(dstsz < 0) {
		fprintf(stderr, "[strncpy] overflow: %zd byte(s) (dest)\n", -dstsz);
		__safec_abort();
	}
	if(dstsz < n) {
		fprintf(stderr, "[strncpy] overflow: %zu vs %zu\n", n, dstsz);
		len = dstsz;
		__safec_print_stacktrace();
	}

	return strncpy(dest, src, len);
}

char* __safe_strcat(char* dest, const char* src)
{
	ssize_t dstsz = _size_right(dest);
	size_t dstln = __safe_strlen(dest);
	size_t srcln = __safe_strlen(src);
	if(dstsz < 0) {
		fprintf(stderr, "[strcat] overflow: %zd byte(s) (dest)\n", -dstsz);
		__safec_abort();
	}
	if(dstln + srcln + 1 > dstsz) {
		size_t n = dstsz - dstln - 1;
		fprintf(stderr, "[strcat] overflow: %zu vs %zu\n", dstln + srcln + 1, dstsz);
		__safec_print_stacktrace();
		return strncat(dest, src, n);
	} else {
		// enough space
		return strcat(dest, src);
	}
}

char* __safe_strncat(char* dest, const char* src, size_t n)
{
	ssize_t dstsz = _size_right(dest);
	size_t dstln = __safe_strlen(dest);
	size_t len = n;
	if(dstsz < 0) {
		fprintf(stderr, "[strncat] overflow: %zd byte(s) (dest)\n", -dstsz);
		__safec_abort();
	}
	if(dstln + n + 1 > dstsz) {
		len = dstsz - dstln - 1;
		fprintf(stderr, "[strncat] invalid n: %zu vs %zu\n", n, len);
		__safec_print_stacktrace();
	}
	return strncat(dest, src, n);
}

int __safe_strcmp(const char* s1, const char* s2)
{
	ssize_t s1sz = _size_right(s1);
	ssize_t s2sz = _size_right(s2);
	ssize_t sz = s1sz < s2sz ? s1sz : s2sz;
	if(s1sz != s2sz) {
		int val = strncmp(s1, s2, sz);
		int l1 = __safe_strlen(s1);
		int l2 = __safe_strlen(s2);
		if(l1 == l2) {
			return val;
		} else if(l1 < l2) {
			return !val ? -1 : val;
		} else { // l1 > l2
			return !val ? 1 : val;
		}
	} else {
		return strncmp(s1, s2, sz);
	}
}

int __safe_strncmp(const char* s1, const char* s2, size_t n)
{
	ssize_t s1sz = _size_right(s1);
	ssize_t s2sz = _size_right(s2);
	ssize_t sz = s1sz < s2sz ? s1sz : s2sz;
	if(n > sz) {
		fprintf(stderr, "[strncmp] invalid len: %zu vs %zu\n", n, sz);
		int val = strncmp(s1, s2, sz);
		int l1 = __safe_strlen(s1);
		int l2 = __safe_strlen(s2);
		if(l1 == l2) {
			return val;
		} else if(l1 < l2) {
			return !val ? -1 : val;
		} else { // l1 > l2
			return !val ? 1 : val;
		}
	} else {
		return strncmp(s1, s2, n);
	}
}

// UNIX IO
ssize_t __safe_read(int fd, void* buf, size_t count)
{
	ssize_t bufsz = _size_right(buf);
	size_t len = count;
	if(bufsz < 0) {
		fprintf(stderr, "[read] overflow: %zd byte(s)\n", -bufsz);
		__safec_abort();
	}
	if(bufsz < len) {
		fprintf(stderr, "[read] invalid size: %zu vs %zu\n", count, bufsz);
		len = bufsz;
		__safec_print_stacktrace();
	}
	return read(fd, buf, len);
}

ssize_t __safe_write(int fd, const void* buf, size_t count)
{
	ssize_t bufsz = _size_right(buf);
	size_t len = count;
	if(bufsz < 0) {
		fprintf(stderr, "[write] overflow: %zd byte(s)\n", -bufsz);
		__safec_abort();
	}
	if(bufsz < len) {
		fprintf(stderr, "[write] invalid size: %zu vs %zu\n", count, bufsz);
		len = bufsz;
		__safec_print_stacktrace();
	}
	return write(fd, buf, len);
}
