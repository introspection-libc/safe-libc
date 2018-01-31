#include <cpuid.h>
#include <stdio.h>
#include <limits.h>

static int has_mpx = 0;

void __attribute__((constructor)) __safec_init_mpx(void)
{
	unsigned int a, b, c, d;
	if (!__get_cpuid(0x7, &a, &b, &c, &d)) {
		fprintf(stderr, "Warning: CPUID request 0x7 unsupported!\n");
		has_mpx = 0;
		return;
	}
	has_mpx = b & bit_MPX;
	if(!has_mpx) {
		fprintf(stderr, "Warning: MPX is unsupported on this CPU\n");
	}
}

ssize_t _size_right(void* p)
{
	if((ssize_t) __builtin___bnd_get_ptr_ubound(p) == -1) {
		/* No bounds information available */
		return LONG_MAX;
	}

	if(!has_mpx) {
		return LONG_MAX;
	}

	size_t size = (size_t) (__builtin___bnd_get_ptr_ubound(p) + 1) - (size_t) p;
	return (ssize_t) size;
}
