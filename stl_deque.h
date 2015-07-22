#ifndef __STL_DEQUE_H
#define __STL_DEQUE_H

#include "iterator.h"
#include "stl_alloc.h"
#include "stl_uninitialized.h"
#include "stl_construct.h"
__IP_BEGIN
inline size_t __deque_buf_size(size_t n, size_t sz)
{
	return n != 0 ? (n) : (sz < 512 ? size_t(512 / sz) : size_t(1));
}
template <typename T, typename Ref, typename Ptr, size_t BufSiz>
struct __deque_iterator
{
	typedef __deque_iterator<T, T&, T*, BufSiz>	iterator;
	typedef __deque_iterator<T, const T&, const T*, BufSiz> const_iterator;
	static size_t buffer_size()
	{
		return __deque_buf_size(BufSiz, sizeof(T));
	}
	
	typedef random_access_iterator_tag	iterator_category;
	typedef T	value;
	typedef Ptr	pointer;
	typedef Ref	reference;
	typedef size_t	size_type;
	typedef ptrdiff_t	difference_type;
	typedef T**	map_pointer;

	typedef __deque_iterator	self;

	T*	cur;
	T*	first;
	T*	last;
	map_pointer	node;

	void set_node(map_pointer new_node)
	{
		node = new_node;
		first = *new_node;
		last = first + difference_type(buffer_size());
	}
	reference operator*() const
	{
		return *cur;
	}
	pointer operator->() const
	{
		return &(operator*());
	}
	difference_type operator-(const self& x)
	{
		return difference_type(buffer_size()) * (node - x.node - 1) + (cur - first) + (x.last - x.cur);
	}
	self& operator++()
	{
		++cur;
		if (cur == last)
		{
			set_node(node + 1);
			cur = first;
		}
		return *this;
	}
	self& operator++(int)
	{
		self tmp = *this;
		++ *this;
		return tmp;
	}
	self& operator--()
	{
		if (cur == first)
		{
			set_node(node - 1);
			cur = last;
		}
		-- cur;
		return *this;
	}
	self& operator--(int)
	{
		self tmp = *this;
		-- *this;
		return tmp;
	}
	self& operator+=(difference_type n)
	{
		difference_type offset = n + (cur - first);
		if (offset >= 0 && offset < difference_type(buffer_size()))
			cur += n;
		else
		{
			difference_type node_offset = offset > 0 ? offset / difference_type(buffer_size()) : \
										  -difference_type((-offset - 1) / buffer_size()) - 1;
			set_node(node + node_offset);
			cur = first + (offset - node_offset * difference_type(buffer_size()));
		}
		return *this;
	}
	self operator+(difference_type n) const
	{
		self tmp = *this;
		return tmp += n;
	}
	self& operator-=(difference_type n)
	{
		return *this += -n;
	}
	self operator-(difference_type n) const
	{
		self tmp = *this;
		return tmp -= n;
	}
	reference operator[](difference_type n) const
	{
		return *(*this + n);
	}
	bool operator==(const self& x) const
	{
		return cur == x.cur;
	}
	bool operator!=(const self& x) const
	{
		return !(*this == x);
	}
	bool operator<(const self& x) const
	{
		return (node == x.node) ? (cur < x.cur) : (node < x.node);
	}
};

template <typename T, typename Alloc=alloc, size_t BufSiz = 0>
class deque
{
public:
	typedef T	value_type;
	typedef T*	pointer;
	typedef size_t	size_type;
	
	typedef __deque_iterator<T, T&, T*, BufSiz>	iterator;
	typedef __deque_iterator<T, const T&, const T*, BufSiz> const_iterator;
	
protected:
	typedef pointer* map_pointer;
	typedef simple_alloc<value_type, Alloc>	data_allocator;
	typedef simple_alloc<pointer, Alloc>	map_allocator;
protected:
	iterator start;
	iterator finish;
	map_pointer map;
	size_type map_size;
protected:
	size_type initial_map_size()
	{
		return 8;
	}
	size_type max(size_type a, size_type b)
	{
		return a >= b ? a : b;
	}
	pointer allocate_node()
	{
		return data_allocator::allocate(buffer_size());
	}
	void deallocate_node(pointer node)
	{
		data_allocator::deallocate(node, buffer_size());
	}
	void create_map_and_nodes(size_type num_elements);
	void destroy_map_and_nodes();
	void fill_initialize(size_type n, const value_type& value);
public:
	deque() : start(), finish(), map(0), map_size(0)
	{
		create_map_and_nodes(0);
	}
	deque(int n, const value_type& value)
	{
		fill_initialize(n, value);
	}
	~deque()
	{}
	iterator begin()
	{
		return start;
	}
	iterator end()
	{
		return finish;
	}
	static size_type buffer_size()
	{
		return __deque_buf_size(BufSiz, sizeof(T));
	}
};
template <typename T, class Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::create_map_and_nodes(size_type num_elements)
{
	size_type num_nodes = num_elements / buffer_size() + 1;

	map_size = max(initial_map_size(), num_nodes + 2);
	map = map_allocator::allocate(map_size);

	map_pointer nstart = map + (map_size - num_nodes) / 2;
	map_pointer nfinish = nstart + num_nodes - 1;
	map_pointer cur;
	__STL_TRY
	{
		for (cur = nstart; cur <= nfinish; ++ cur)
			*cur = allocate_node();
	}catch(...)
	{
		for (map_pointer n = nstart; n < cur; ++ n)
			deallocate_node(*n);
		map_allocator::deallocate(map, map_size);
		throw;
	}
	start.set_node(nstart);
	finish.set_node(nfinish);
	start.cur = start.first;
	finish.cur = finish.first + num_elements % buffer_size();
}
template <typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::fill_initialize(size_type n, const value_type& value)
{
	create_map_and_nodes(n);
	map_pointer cur;
	__STL_TRY
	{
		for (cur = start.node; cur < finish.node; ++ cur)
			uninitialized_fill(*cur, *cur + buffer_size(), value);
		uninitialized_fill(finish.first, finish.cur, value);
	}catch(...)
	{
		for (map_pointer n = cur; n < cur; ++ n)
			destroy(*n, *n + buffer_size());
		destroy_map_and_nodes();
		throw;
	}
}
template <typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::destroy_map_and_nodes()
{
	for (map_pointer cur = start.node; cur < finish.node; ++ cur)
		deallocate_node(*cur);
	map_allocator::deallocate(map, map_size);
}
__IP_END

#endif	//stl_deque.h
