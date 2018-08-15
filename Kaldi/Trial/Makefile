testbson: 
	g++ -o testbson testbson.c -I\
	    -Ilibbson-1.0 -lbson-1.0
testredis: testredis.c
	g++ -o $@ $@.c -lhiredis -levent -std=c++14
testthread: testthread.c
	g++ -o $@ $@.c -std=c++14 -pthread
