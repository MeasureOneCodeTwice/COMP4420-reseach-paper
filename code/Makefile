CC=g++
CFLAGS=--std=c++20 -ggdb

main : main.o k_funnel.o buffer.o funnelsort.o testlib.o
	$(CC) $(CFLAGS) main.o funnelsort.o testlib.o k_funnel.o buffer.o -o main


%.o : src/%.cpp 
	$(CC) $(CFLAGS) -c $(<) -o $(@)  


clean :
	rm main *.o
