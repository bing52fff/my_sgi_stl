#ifndef __STL_ITERATOR_H
#define __STL_ITERATOR_H

#include "ipdef.h"

__IP_BEGIN
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

template <typename Category,
		  typename T,
		  typename Distance,
		  typename Pointer = T*,
		  typename Reference = T&>
struct iterator
{
	typedef Category iterator_category;
	typedef T	value_type;
	typedef Distance difference_type;
	typedef Pointer pointer;
	typedef Reference reference;
};

template <typename Iterator>
struct iterator_traits
{
	typedef typename Iterator::iterator_category iterator_category;
	typedef typename Iterator::value_type value_type;
	typedef typename Iterator::difference_type difference_type;
	typedef typename Iterator::pointer pointer;
	typedef typename Iterator::reference reference;
};

template <typename T>
struct iterator_traits<T*>
{
	typedef random_access_iterator_tag iterator_category;
	typedef T value_type;
	typedef ptrdiff_t difference_type;
	typedef T* pointer;
	typedef T& reference;
};

template <typename T>
struct iterator_traits<const T*>
{
	typedef random_access_iterator_tag iterator_category;
	typedef T value_type;
	typedef ptrdiff_t difference_type;
	typedef const T* pointer;
	typedef const T& reference;
};

template <typename Iterator>
inline typename iterator_traits<Iterator>::value_type* value_type(const Iterator&)
{
	typedef typename iterator_traits<Iterator>::value_type value_type;
	return static_cast<value_type*>(0);
}

template <typename Iterator>
inline typename iterator_traits<Iterator>::distance_type* distance_type(const Iterator&)
{
	return static_cast<typename iterator_traits<Iterator>::distance_type*>(0);
}

////////////////////////////////////////////////////////////////////////////////////////
template <typename Iterator>
class reverse_iterator
{
public:
	typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
	typedef typename iterator_traits<Iterator>::value_type value_type;
	typedef typename iterator_traits<Iterator>::difference_type difference_type;
	typedef typename iterator_traits<Iterator>::pointer pointer;
	typedef typename iterator_traits<Iterator>::reference reference;
protected:
	Iterator current;
	typedef reverse_iterator<Iterator> self;
public:
	reverse_iterator() {}
	explicit reverse_iterator(const Iterator& x): current(x) {}
	reverse_iterator(const self& x): current(x.current) {}

public:
	Iterator base() const {return current;}
	reference operator*() const
	{
		Iterator tmp = current;
		return *--tmp;
	}
	pointer operator->() const
	{
		return &(operator*());
	}
	self& operator++()
	{
		--current;
		return *this;
	}
	self& operator++(int)
	{
		self tmp = *this;
		--current;
		return tmp;
	}
	self& operator--()
	{
		++current;
		return *this;
	}
	self& operator--(int)
	{
		self tmp = *this;
		++ current;
		return tmp;
	}
};
template <typename Iterator>
inline bool operator!=(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y)
{
	return x.base() != y.base();
}
template <typename Iterator>
inline bool operator==(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y)
{
	return x.base() == y.base();
}
__IP_END

#endif	//stl_iterator.h
