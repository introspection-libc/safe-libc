#include <stdio.h>
#include <limits.h>

ssize_t _size_right(void* p) {
    if ((ssize_t) __builtin___bnd_get_ptr_ubound(p) == -1) {
        /* No bounds information available */
        return LONG_MAX;
    }
	size_t size = (size_t) (__builtin___bnd_get_ptr_ubound(p) + 1) - (size_t) p;
    return (ssize_t) size;
}
