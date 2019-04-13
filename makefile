compile:
	gcc -c -Wall compiler.c
	gcc -c -Wall ir_debug.c
	g++ -g -Wall proj5.cpp compiler.o ir_debug.o
test:
	./a.out < tests/$(arg)
