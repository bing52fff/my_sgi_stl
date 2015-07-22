all:list_test.cpp
	g++ deque_test.cpp -o deque_test -g

.PHONY:clean
clean:
	rm deque_test
