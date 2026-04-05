CC=g++
# CFLAGS=-Wall

main : main.o k_funnel.o buffer.o
	$(CC) $(CFLAGS) main.o k_funnel.o buffer.o -o main


%.o : %.cpp 
	$(CC) $(CFLAGS) -c $(<) -o $(@)  


clean :
	rm main *.o
