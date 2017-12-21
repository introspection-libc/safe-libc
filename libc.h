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

char* __safe_gets(char* s);

#endif
