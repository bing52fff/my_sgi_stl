#ifndef __STL_INTERNAL_ALLOC_H
#define __STL_INTERNAL_ALLOC_H

#include <stdlib.h>
#include <string.h>
#if 0
#	include <new.h>
#	define	__THROW_BAD_ALLOC	throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#	include <stdio.h>
#	define __THROW_BAD_ALLOC fprintf(stderr, "out of memory\n");exit(1)
#endif
__IP_BEGIN
template <int inst>
class __malloc_alloc_template
{
private:
	static void* oom_malloc(size_t sz);
	static void* oom_realloc(void *p, size_t old_sz, size_t new_sz);
	static void (*__malloc_oom_handler)();

public:
	static void* allocate(size_t sz)
	{
		void *result;
		result = malloc(sz);
		if (NULL == result)
		{
			result = oom_malloc(sz);
		}
		return result;
	}
	static void* reallocate(void *p, size_t old_sz, size_t new_sz)
	{
		void *result = realloc(p, new_sz);
		if (NULL == result)
			result = oom_realloc(p, old_sz, new_sz);
		return result;
	}
	static void deallocate(void *p, size_t sz)
	{
		free(p);
	}
	static void (*set_malloc_handler(void (*f)()))()
	{
		void (*old)() = __malloc_oom_handler;
		__malloc_oom_handler = f;
		return old;
	}
};
template <int inst>
void (*__malloc_alloc_template<inst>::__malloc_oom_handler)() = 0;
template <int inst>
void* __malloc_alloc_template<inst>::oom_malloc(size_t sz)
{
	void *result;
	void (*my_malloc_handler)() = __malloc_oom_handler;
	if (NULL == my_malloc_handler)
		__THROW_BAD_ALLOC;
	for (; ;)
	{
		(*my_malloc_handler)();
		result = malloc(sz);
		if (result)
			return result;
	}
}
template <int inst>
void* __malloc_alloc_template<inst>::oom_realloc(void *p, size_t old_sz, size_t new_sz)
{
	void *result;
	void (*my_malloc_handler)() = __malloc_oom_handler;
	if (NULL == my_malloc_handler)
		__THROW_BAD_ALLOC;
	for (;;)
	{
		(*my_malloc_handler)();
		result = realloc(p, new_sz);
		if (result)
			return result;
	}
}

typedef __malloc_alloc_template<0> malloc_alloc;
//interface
template <typename T, typename Alloc>
class simple_alloc
{
public:
	static T* allocate(size_t n)
	{ return (T*)(0 == n ? Alloc::allocate(1) : Alloc::allocate(n * sizeof(T))); }
	static T* allocate(void)
	{ return (T*)(Alloc::allocate(sizeof(T))); }
	static void deallocate(T *p, size_t n)
	{ Alloc::deallocate(p, n*sizeof(T)); }
	static void deallocate(T *p)
	{ Alloc::deallocate(p, sizeof(T)); }	
};

//second alloctr
template <bool threads, int inst>
class __default_alloc_template
{
private:
	enum{__ALIGN = 8};
	enum{__MAX_SIZE = 128};
	enum{__NFREELISTS = __MAX_SIZE / __ALIGN};
private:
	union obj
	{
		union obj *free_list_link;
		char client[1];
	};
	static obj* volatile free_list[__NFREELISTS];
	static char *start_free;
	static char *end_free;
	static size_t heap_size;

	static size_t ROUND_UP(size_t bytes)
	{
		return ((bytes + __ALIGN -1) & ~(__ALIGN - 1));
	}
	static size_t FREELIST_INDEX(size_t bytes)
	{
		return ((bytes+__ALIGN-1)/__ALIGN-1);
	}
	static void* refill(size_t n)
	{
		obj *result;
		obj* volatile *my_free_list;
		int nobjs = 20;
		char *chunk = chunk_alloc(n, nobjs);
		if (1 == nobjs)
			return chunk;
		result = (obj*)chunk;
		obj *current_obj, *next_obj;
		my_free_list = free_list + FREELIST_INDEX(n);
		*my_free_list = next_obj = (obj*)(chunk + n);
		int i = 0;
		for (; ; ++ i)
		{
			current_obj = next_obj;
			next_obj = (obj*)((char*)current_obj + n);
			if (nobjs - 1 == i)
			{
				current_obj->free_list_link = NULL;
				break;
			}
			else
			{
				current_obj->free_list_link = next_obj;
			}
		}
		return result;
	}
	static char* chunk_alloc(size_t n, int &nobjs)
	{
		const size_t total_size = n * nobjs;
		const size_t left_size = end_free - start_free;
		char *result;
		
		if (total_size <= left_size)
		{
			result = start_free;
			start_free += total_size;
			return result;
		}
		else if (n <= left_size)
		{
			nobjs = left_size / n;
			result = start_free;
			start_free += (n * nobjs);
			return result;
		}
		else
		{
			size_t bytes_to_get = 2 * total_size + ROUND_UP(heap_size >> 4);
			obj* volatile *my_free_list, *p;
			if (left_size > 0)
			{
				my_free_list = free_list + FREELIST_INDEX(left_size);
				p = (obj*)start_free;
				p->free_list_link = *my_free_list;
				(*my_free_list)->free_list_link = p;
			}
			start_free = (char*) malloc(bytes_to_get);
			if (NULL == start_free)
			{
				int i;
				for (i = n; i <= __MAX_SIZE; i += __ALIGN)
				{
					my_free_list = free_list + FREELIST_INDEX(n);
					if (NULL != *my_free_list)
					{
						start_free = (char*)*my_free_list;
						end_free = start_free + i;
						*my_free_list = (*my_free_list)->free_list_link;
						return chunk_alloc(n, nobjs);
					}
				}
				end_free = 0;
				start_free = (char*) malloc_alloc::allocate(bytes_to_get);
			}
			heap_size += bytes_to_get;
			end_free = start_free + bytes_to_get;
			return chunk_alloc(n, nobjs);
		}
	}
public:
	static void* allocate(size_t n)
	{
		obj *result;
		obj* volatile *my_free_list;

		if (n > (size_t)__MAX_SIZE)
			return malloc_alloc::allocate(n);
		my_free_list = free_list + FREELIST_INDEX(n);
		result = *my_free_list;
		if (NULL == result)
			return refill(ROUND_UP(n));
		*my_free_list = result->free_list_link;	
		return result;
	}
	static void* reallocate(void *p, size_t old_sz, size_t new_sz)
	{
		obj* volatile *my_free_list;
		void *result;
		if (old_sz > __MAX_SIZE && new_sz > __MAX_SIZE)
			return malloc_alloc::reallocate(p, old_sz, new_sz);
		if (ROUND_UP(new_sz) == ROUND_UP(old_sz))
			return p;
		result = allocate(new_sz);
		size_t copy_sz = new_sz > old_sz? old_sz: new_sz;
		memcpy(result, p, copy_sz);
		deallocate(p, old_sz);
		return result;
	}
	static void deallocate(void *p, size_t sz)
	{
		obj* volatile *my_free_list;
		if (sz > __MAX_SIZE)
			return malloc_alloc::deallocate(p, sz);
		my_free_list = free_list + FREELIST_INDEX(sz);
		obj *s = (obj*)p;
		s->free_list_link = *my_free_list;
		*my_free_list = s;
	}
};
template <bool threads, int inst>
typename __default_alloc_template<threads, inst>::obj* volatile
__default_alloc_template<threads, inst>::free_list[__NFREELISTS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
template <bool threads, int inst>
char* __default_alloc_template<threads, inst>::start_free = 0;
template <bool threads, int inst>
char* __default_alloc_template<threads, inst>::end_free = 0;
template <bool threads, int inst>
size_t __default_alloc_template<threads, inst>::heap_size = 0;

#ifdef __USE_MALLOC
	typedef malloc_alloc alloc;
#else
	typedef __default_alloc_template<0, 0> alloc;
#endif

__IP_END	//namespace ip

#endif	//stl_alloc.h
