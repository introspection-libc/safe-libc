#ifndef __LIBC_H__
#define __LIBC_H__

#define	gets	__introspection_gets
#define	fgets	__introspection_fgets

#define	strlen	__introspection_strlen
#define	strnlen	__introspection_strnlen
#define	strcpy	__introspection_strcpy
#define	strncpy	__introspection_strncpy
#define	strcat	__introspection_strcat
#define	strncat	__introspection_strncat
#define	strcmp	__introspection_strcmp
#define	strncmp	__introspection_strncmp

#define	memcpy	__introspection_memcpy
#define	memcmp	__introspection_memcmp
#define	memset	__introspection_memset

#define	read	__introspection_read
#define	write	__introspection_write

#ifdef __cplusplus
extern "C" {
#endif
char* __introspection_gets(char* s);
#ifdef __cplusplus
}
#endif


#endif
