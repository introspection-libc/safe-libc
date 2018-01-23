#include <unistd.h>

extern char* __softboundcets_load_base_shadow_stack(int);
extern char* __softboundcets_load_bound_shadow_stack(int);

size_t _size_right(const char* p)
{
	const char* base = __softboundcets_load_base_shadow_stack(1);
	const char* bound = __softboundcets_load_bound_shadow_stack(1);
	if(base > p)
		return 0;
	if(p > bound)
		return 0;
	return bound - p;
}
