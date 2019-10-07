all: arbitrary_dim_array.hpp functionality_test.cpp
	g++ functionality_test.cpp -o test_exec
	./test_exec
	rm -rf test_exec

checkmem: arbitrary_dim_array.hpp functionality_test.cpp
	g++ $^ -o test_exec
	valgrind ./test_exec
	rm -rf test_exec
