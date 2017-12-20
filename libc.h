#ifndef __LIBC_H__
#define __LIBC_H__

#define	gets	__safe_gets
#define	fgets	__safe_fgets

#define	strlen	__safe_strlen
#define	strnlen	__safe_strnlen
#define	strcpy	__safe_strcpy
#define	strncpy	__safe_strncpy
#define	strcat	__safe_strcat
#define	strncat	__safe_strncat
#define	strcmp	__safe_strcmp
#define	strncmp	__safe_strncmp

#define	read	__safe_read
#define	write	__safe_write

#include <stdio.h>

char* __safe_gets(char* s);
char* __safe_fgets(char* s, int size, FILE* stream);

size_t __safe_strlen(const char* s);
size_t __safe_strnlen(const char* s, size_t maxlen);

char* __safe_strcpy(char* dest, const char* src);
char* __safe_strncpy(char* dest, const char* src, size_t n);

char* __safe_strcat(char* dest, const char* src);
char* __safe_strncat(char* dest, const char* src, size_t n);

int __safe_strcmp(const char* s1, const char* s2);
int __safe_strncmp(const char* s1, const char* s2, size_t n);

ssize_t __safe_read(int fd, void* buf, size_t count);
ssize_t __safe_write(int fd, const void* buf, size_t count);

#endif
