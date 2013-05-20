vm : main.o
	gcc -o vm main.o
main.o : main.c
	gcc -c main.c
clean :
	rm main.o
