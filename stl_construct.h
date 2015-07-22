#ifndef __STL_CONSTRUCT_H
#define __STL_CONSTRUCT_H

#include <new>
#include "iterator.h"
#include "ipdef.h"

__IP_BEGIN
template <typename T1, typename T2>
inline void construct(T1 *p, const T2 &value)
{
	new(p) T1(value);
}

template <typename T>
inline void destroy(T *p)
{
	p->~T();
}

//partical specifical
template <typename ForwardIterator>
inline void __destroy_aux(ForwardIterator, ForwardIterator, __true_type)
{}

template <typename ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type)
{
	for (; first != last; ++first)
	{
		destroy(&*first);
	}
}

template <typename ForwardIterator, typename T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*)
{
	typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
	__destroy_aux(first, last, trivial_destructor());
}

template <typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last)
{
	__destroy(first, last, value_type(first));
}

inline void destory(char*, char*)
{}

inline void destory(wchar_t*, wchar_t*)
{}
__IP_END

#endif	//stl_construct.h
