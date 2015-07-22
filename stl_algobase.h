#ifndef __STL_ALGOBASE_H
#define __STL_ALGOBASE_H

#include "iterator.h"

__IP_BEGIN
//fill
template <typename ForwardIterator,  typename T>
inline void fill(ForwardIterator first, ForwardIterator last, const T& value)
{
	for (; first != last; ++ first)
		*first = value;
}

//fill_n
template <typename OutputIterator, typename T, typename Size>
inline OutputIterator fill_n(OutputIterator first, Size n, const T& value)
{
	for (; n > 0; -- n, ++ first)
		*first = value;
	return first;
}

//copy
template <typename RandomAccessIterator, typename OutputIterator, typename Distance>
inline OutputIterator
__copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance*)
{
	for (Distance n = last - first; n > 0; -- n, ++ first, ++ result)
		*result = *first;
	return result;
}
template <typename T>
inline T* __copy_t(const T* first, const T* last, T* result, __true_type)
{
	memmove(result, first, (last - first)*sizeof(T));
	return result + (last - first);
}
template <typename T>
inline T* __copy_t(const T* first, const T* last, T* result, __false_type)
{
	return __copy_d(first, last, result, (ptrdiff_t)0);
}
template <typename InputIterator, typename OutputIterator>
inline OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, input_iterator_tag)
{
	for (; first != last; ++ first, ++ result)
		*result = *first;
	return result;
}
template <typename RandomAccessIterator, typename OutputIterator>
inline OutputIterator __copy(RandomAccessIterator first, RandomAccessIterator last,
						OutputIterator result, random_access_iterator_tag)
{
	return __copy_d(first, last, result, distance_type(first));
}
template <typename InputIterator, typename OutputIterator>
struct __copy_dispatch
{
	OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result)
	{
		return __copy(first, last, result, iterator_category(first));
	}
};
template <typename T>
struct __copy_dispatch<T*, T*>
{
	T* operator()(T* first, T* last, T* result)
	{
		typedef typename __type_traits<T>::has_trivial_assignment_operator t;
		return __copy_t(first, last, result, t());
	}
};
template <typename T>
struct __copy_dispatch<const T*, T*>
{
	T* operator()(const T* first, const T* last, T* result)
	{
		typedef typename __type_traits<T>::has_trivial_assignment t;
		return __copy_t(first, last, result, t());
	}
};
template <typename InputIterator, typename OutputIterator>
inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
{
	return __copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
}
inline char* copy(const char *first, const char *last, char *result)
{
	memmove(result, first, last - first);
	return result + (last - first);
}
inline wchar_t* copy(const wchar_t *first, const wchar_t *last, wchar_t *result)
{
	memmove(result, first, last - first);
	return result + (last - first);
}

//copy_backward
template <typename B1, typename B2>
inline B2 __copy_backward(B1 first, B1 last, B2 result)
{
	for (; first != last; -- last, -- result)
		*result = *last;
	return result;
}
template <typename T>
inline T* __copy_backward_t(const T* first, const T* last, T* result, __true_type)
{
	memmove(result - (last - first), first, (last - first)*sizeof(T));
	return result - (last - first);
}
template <typename T>
inline T* __copy_backward_t(const T* first, const T* last, T* result, __false_type)
{
	return __copy_backward(first, last, result);
}
template <typename B1, typename B2>
struct __copy_backward_dispatch
{
	B2 operator()(B1 first, B1 last, B2 result)
	{
		return __copy_backward(first, last, result);
	}
};
template <typename T>
struct __copy_backward_dispatch<T*, T*>
{
	T* operator()(T* first, T* last, T* result)
	{
		typedef typename __type_traits<T>::has_trivial_assignment_operator t;
		return __copy_backward_t(first, last, result, t());
	}
};
template <typename T>
struct __copy_backward_dispatch<const T*, T*>
{
	T* operator()(const T* first, const T* last, T* result)
	{
		typedef typename __type_traits<T>::has_trivial_assignment_operator t;
		return __copy_backward_t(first, last, result, t());
	}
};
template <typename B1, typename B2>
inline B2 copy_backward(B1 first, B1 last, B2 result)
{
	return __copy_backward_dispatch<B1, B2>()(first, last, result);
}
inline char* copy_backward(const char* first, const char* last, char* result)
{
	memmove(result, first, last - first);
	return result + (last - first);
}
inline wchar_t* copy_backward(const wchar_t* first, const wchar_t* last, wchar_t* result)
{
	memmove(result, first, last - first);
	return result + (last - first);
}
__IP_END

#endif	//algobase.h
