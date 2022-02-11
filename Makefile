memsim: memsim.o
	g++ memsim.o -o memsim

memsim.o: memsim.cpp
	g++ -c memsim.cpp

clean:
	rm *.o memsim