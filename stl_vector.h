#ifndef __STL_VECTOR_H
#define __STL_VECTOR_H

#include "memory"
#include "iterator.h"
#include "ipdef.h"

__IP_BEGIN
template <typename T, typename Alloc = alloc>
class vector
{
public:
	typedef T value_type;
	typedef value_type* pointer;
	typedef value_type* iterator;
	typedef const value_type* const_iterator;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t	size_type;
	typedef ptrdiff_t	difference_type;
	typedef ip::reverse_iterator<iterator> reverse_iterator;
	typedef ip::reverse_iterator<const_iterator> const_reverse_iterator;
protected:
	typedef simple_alloc<value_type, Alloc> data_allocator;
private:
	iterator start;
	iterator finish;
	iterator end_of_storage;
	void insert_aux(iterator position, const T& x);
	void deallocate()
	{
		if (start)
			data_allocator::deallocate(start, end_of_storage - start);
	}
	void fill_initialized(size_type n, const T& value)
	{
		start = allocate_and_fill(n, value);
		finish = start + n;
		end_of_storage = finish;
	}
	iterator allocate_and_fill(size_type n, const T& value)
	{
		iterator result = data_allocator::allocate(n);
		uninitialized_fill_n(result, n, value);
		return result;
	}
public:
	iterator begin() {return (start);}
	iterator end() {return (finish);}
	const_iterator begin() const {return start;}
	const_iterator end() const {return finish;}
	reverse_iterator rbegin() { return reverse_iterator(end()); }
	reverse_iterator rend() {return reverse_iterator(begin());}
	const_reverse_iterator rbegin() const {return const_reverse_iterator(end());}
	const_reverse_iterator rend() const {return const_reverse_iterator(begin());}

	size_type size() const {return (size_type(finish - start));}
	size_type capacity() const {return (size_type)(end_of_storage - start);}

	bool empty() const {return begin() == end();}
	reference operator[](size_type n) {return *(begin() + n);}
	
	vector(): start(0), finish(0), end_of_storage(0){}
	vector(size_type n, const T& value) {fill_initialized(n, value);}
	~vector()
	{
		destroy(start, finish);
		deallocate();
	}
	
	reference front() {return *(begin());}
	reference back() {return *(end() - 1);}
	void push_back(const T& x)
	{
		if (finish != end_of_storage)
		{
			construct(finish, x);
			++ finish;
		}
		else
			insert_aux(end(), x);
	}
	void pop_back()
	{
		if (start != finish)
		{
			-- finish;
			destroy(finish);
		}
	}
	iterator erase(iterator first, iterator last)
	{
		iterator i = copy(last, finish, first);
		destroy(i, finish);
		finish -= (last - first);
		return first;
	}
	iterator erase(iterator position)
	{
		if (position + 1 != end())
			copy(position + 1, finish, position);
		--finish;
		destroy(finish);
		return position;
	}
	void clear()
	{
		erase(begin(), end());
	}

	void insert(iterator position, size_type n, const T& x)
	{
		if (0 == n)
			return ;
		T x_copy = x;
		const size_type elem_after = finish - position;
		iterator old_finish = finish;

		if (n <= size_type(end_of_storage - finish))
		{
			if (elem_after > n)
			{
				uninitialized_copy(finish - n, finish, finish);
				finish += n;
				copy_backward(position, old_finish - n, old_finish);
				fill(position, old_finish - n, x_copy);
			}
			else
			{
				uninitialized_fill_n(finish, n - elem_after, x_copy);
				finish += (n - elem_after);
				uninitialized_copy(position, old_finish, finish);
				finish += elem_after;
				fill(position, old_finish, x_copy);
			}
		}
		else
		{
			const size_type old_size = size();
			const size_type len = old_size + (old_size > n ? old_size : n);
			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;
			__STL_TRY
			{
				new_finish = uninitialized_copy(start, position, new_start);
				new_finish = uninitialized_fill_n(new_finish, n, x_copy);
				new_finish = uninitialized_copy(position, finish, new_finish);
			}
			__STL_CATCH_ALL
			{
				destroy(new_start, new_finish);
				data_allocator::deallocate(new_start, len);
				throw;
			}
			destroy(start, finish);
			deallocate();

			start = new_start;
			finish = new_finish;
			end_of_storage = start + len;
		}
	}
};
template <typename T, typename Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const T& x)
{
	if (finish != end_of_storage)	
	{
		construct(finish, *(finish - 1));
		++ finish;
		copy_backward(position, finish - 2, finish - 1);
		T x_copy = x;
		*position = x_copy;
	}
	else
	{
		const size_type old_size = end_of_storage - start;
		const size_type new_size = old_size != 0 ? 2*old_size : 1;
		iterator new_start = data_allocator::allocate(new_size);
		iterator new_finish = new_start;
		__STL_TRY
		{
			new_finish = uninitialized_copy(start, position, new_start);
			construct(new_finish, x);
			++ new_finish;
			new_finish = uninitialized_copy(position, finish, new_finish);
		}
		__STL_CATCH_ALL
		{
			destroy(new_start, new_finish);
			data_allocator::deallocate(new_start, new_size);
			throw;
		}
		destroy(start, finish);
		deallocate();
		start = new_start;
		finish = new_finish;
		end_of_storage = start + new_size;
	}
}
__IP_END

#endif	//stl_vector.h
