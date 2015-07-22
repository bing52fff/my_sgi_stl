#ifndef __STL_UNINITIALIZED_H
#define __STL_UNINITIALIZED_H

#include "iterator.h"
#include "ipdef.h"
#include "algobase.h"

__IP_BEGIN
//uninitialized_fill_n
template <typename ForwardIterator, typename T, typename Size>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __true_type)
{
	return fill_n(first, n, x);
}
template <typename ForwardIterator, typename T, typename Size>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type)
{
	ForwardIterator cur = first;
	__STL_TRY{
		for (; n > 0; -- n, ++ cur)
			construct(&*cur, x);
		return cur;
	}
	__STL_UNWIND(destroy(first, cur));
}
template <typename ForwardIterator, typename T, typename Size, typename T1>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*)
{
	typedef typename __type_traits<T1>::is_POD_type is_POD;
	return __uninitialized_fill_n_aux(first, n, x, is_POD());
}
template <typename ForwardIterator, typename T, typename Size>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x)
{
	return __uninitialized_fill_n(first, n, x, value_type(first));
}

//uninitialized_fill
template <typename ForwardIterator, typename T>
inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T&x, __true_type)
{
	fill(first, last, x);
}
template <typename ForwardIterator, typename T>
inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T&x, __false_type)
{
	ForwardIterator cur;
	for (cur = first; cur != last; ++ cur)
		construct(&*cur, x);
}
template <typename ForwardIterator, typename T, typename T1>
inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T&x, T1*)
{
	typedef typename __type_traits<T1>::is_POD_type is_POD;
	__uninitialized_fill_aux(first, last, x, is_POD());
}
template <typename ForwardIterator, typename T>
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x)
{
	__uninitialized_fill(first, last, x, value_type(first));
}

//uninitialized_copy
template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator
__uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type)
{
	return copy(first, last, result);
}
template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator 
__uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type)
{
	ForwardIterator cur = result;
	__STL_TRY
	{
		for (; first != last; ++ first)
			construct(&*cur, *first);
		return cur;
	}
	__STL_UNWIND(destroy(result, cur));
}
template <typename InputIterator, typename ForwardIterator, typename T>
inline ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T*)
{
	typedef typename __type_traits<T>::is_POD_type is_POD;
	return __uninitialized_copy_aux(first, last, result, is_POD());
}
template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
{
	return __uninitialized_copy(first, last, result, value_type(result));
}

__IP_END

#endif	//stl_uninitialized.h
