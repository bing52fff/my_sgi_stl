#ifndef __STL_LIST_H
#define __STL_LIST_H

#include "ipdef.h"
#include "memory"
#include "iterator.h"

#include <iostream>
using namespace std;

__IP_BEGIN
template <typename T>
struct __list_node
{
	typedef __list_node<T> self;
	typedef void* void_pointer;
	void_pointer prev;
	void_pointer next;
	T data;
};

template <typename T, typename Ref, typename Ptr>
struct __list_iterator
{
	typedef __list_iterator<T, T&, T*> iterator;
	typedef __list_iterator<T, Ref, Ptr> self;
	typedef __list_node<T>* link_type;

	typedef bidirectional_iterator_tag iterator_category;
	typedef T value_type;
	typedef Ptr pointer;
	typedef Ref reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	
	link_type node;

	__list_iterator(link_type x): node(x)
	{}
	__list_iterator()
	{}
	__list_iterator(const iterator& x): node(x.node)
	{}

	bool operator==(const self& x) const
	{
		return node == x.node;
	}
	bool operator!=(const self& x) const
	{
		return node != x.node;
	}
	reference operator*()
	{
		return (*node).data;
	}
	pointer operator->()
	{
		return &(operator*());
	}
	self& operator++()
	{
		node = (link_type)(node->next);
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
		node = (link_type)(node->prev);
		return *this;
	}
	self& operator--(int)
	{
		self tmp = *this;
		-- *this;
		return tmp;
	}
};

template <typename T, typename Alloc = alloc>
class list
{
protected:
	typedef __list_node<T> list_node;
	typedef __list_node<T>* link_type;
	typedef simple_alloc<list_node, Alloc> list_node_allocator;
public:
	typedef __list_iterator<T, T&, T*> iterator;
protected:
	link_type node;
protected:
	void transfer(iterator position, iterator first, iterator last)
	{
		if (position != last)
		{
			(link_type(first.node->prev))->next = last.node;
			(link_type(last.node->prev))->next = position.node;
			(link_type(position.node->prev))->next = first.node;
			link_type tmp = (link_type)(last.node->prev);
			last.node->prev = first.node->prev;
			first.node->prev = position.node->prev;
			position.node->prev = tmp;
		}
	}
public:
	list()
	{
		empty_initialize();
	}
	iterator begin()
	{
		return iterator((link_type)node->next);
	}
	iterator end()
	{
		return iterator(node);
	}
	bool empty()
	{
		return begin() == end();
	}
	void swap(list<T, Alloc> &x)
	{
		if (empty() || x.empty())
			return ;
		link_type tmp = node;
		node = x.node;
		x.node = tmp;
	}
	iterator insert(iterator position, const T& x)
	{
		link_type tmp = create_node(x);
		tmp->prev = position.node->prev;
		tmp->next = position.node;
		(link_type(position.node->prev))->next = tmp;
		position.node->prev = tmp;
		return tmp;
	}
	void push_front(const T& x)
	{
		insert(begin(), x);
	}
	void push_back(const T& x)
	{
		insert(end(), x);
	}
	iterator erase(iterator position)
	{
		link_type next_node = (link_type)(position.node->next);		
		link_type prev_node = (link_type)(position.node->prev);
		next_node->prev = prev_node;
		prev_node->next = next_node;
		destroy_node(position.node);
		return iterator(next_node);
	}
	void pop_front()
	{
		erase(begin());
	}
	void pop_back()
	{
		erase(-- end());
	}
	void clear()
	{
		link_type cur = (link_type)node->next;
		while (cur != node)
		{
			destroy_node(cur --);
		}
		node->next = node;
		node->prev = node;
	}
	void remove(const T& value)
	{
		iterator first = begin();
		while (first != end())
		{
			iterator next = first;
			++ next;
			if (*first == value)
				erase(first);
			first = next;
		}
	}
	void unique()
	{
		iterator first = begin();
		if (first == end())
			return ;
		iterator next = first;
		while (++ next != end())
		{
			if (*first == *next)
				erase(next);
			else
				first = next;
			next = first;
		}
	}
	void splice(iterator position, list<T, Alloc>& x)
	{
		if (!x.empty())
			transfer(position, x.begin(), x.end());
	}
	void splice(iterator position, list<T, Alloc>&, iterator i)
	{
		iterator j = i;
		++ j;
		if (position == i || position == j)
			return ;
		transfer(position, i, j);
	}
	void splice(iterator position, list<T, Alloc>&, iterator first, iterator last)
	{
		if (first != last)
			transfer(position, first, last);
	}
	void merge(list<T, Alloc>& x)
	{
		iterator first1 = begin();
		iterator first2 = x.begin();
		iterator last1 = end();
		iterator last2 = x.end();
		while (first1 != last1 && first2 != last2)
		{
			if (*first2 < *first1)
			{
				iterator next = first2;
				transfer(first1, first2, ++ next);
				first2 = next;
			}
			else
				++ first1;
		}
		if (first2 != last2)
			transfer(last1, first2, last2);
	}
	void reverse()
	{
		if (node->next == node || ((link_type)node->next)->next == node)
			return ;
		iterator first = begin();
		iterator old;
		++ first;
		while (first != end())
		{
			old = first;
			++ first;
			transfer(begin(), old, first);
		}
	}
	void sort()
	{
		if (node->next == node || ((link_type)node->next)->next == node)
			return ;
		list<T, Alloc> carry;
		list<T, Alloc> counter[64];
		int fill = 0;
		while (!empty())
		{
			carry.splice(carry.begin(), *this, begin());
			int i = 0;
			while (i < fill && !counter[i].empty())
			{
				counter[i].merge(carry);
				carry.swap(counter[i++]);
			}
			carry.swap(counter[i]);
			if (i == fill)
				++ fill;
		}
		for (int i = 1; i < fill; ++ i)
			counter[i].merge(counter[i-1]);
		swap(counter[fill-1]);
	}
protected:
	void empty_initialize()
	{
		node = get_node();
		node->next = node;
		node->prev = node;
	}
	link_type get_node()
	{
		return list_node_allocator::allocate();
	}
	void put_node(link_type p)
	{
		list_node_allocator::deallocate(p);
	}
	link_type create_node(const T& x)
	{
		link_type p = get_node();
		construct(&p->data, x);
		return p;
	}
	void destroy_node(link_type p)
	{
		destroy(&p->data);
		put_node(p);
	}
};
__IP_END

#endif	//stl_list.h
