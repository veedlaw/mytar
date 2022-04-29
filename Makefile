CC=gcc
CFLAGS=-Wall -Wextra 

mytar: mytar.o
	$(CC) $(CFLAGS) -o mytar mytar.o 
