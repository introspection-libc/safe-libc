#include <stdio.h>

ssize_t _size_right(void* p) {
	return (ssize_t) (__builtin___bnd_get_ptr_ubound(p) + 1) - (ssize_t) p;
}
