#include <cpuid.h>
#include <stdio.h>
#include <limits.h>

ssize_t _size_right(void* p)
{
    ssize_t upper_bounds = (ssize_t) __builtin___bnd_get_ptr_ubound(p);
	if (upper_bounds == -1) {
		/* No bounds information available */
		return LONG_MAX;
	}

	size_t size = (size_t) (upper_bounds + 1) - (size_t) p;
    return (ssize_t) size;
}
