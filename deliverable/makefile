CC=gcc

default: sendfile recvfile

all: sendfile recvfile

sendfile.o: src/sendfile.c
	$(CC) -c src/sendfile.c

recvfile.o: src/recvfile.c
	$(CC) -c src/recvfile.c

sendfile: sendfile.o
	$(CC) -o sendfile sendfile.o

recvfile: recvfile.o
	$(CC) -o recvfile recvfile.o

clean: 
	rm -f *.o
	rm -f recvfile
	rm -f sendfile