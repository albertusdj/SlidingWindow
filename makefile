sendfile : sendfile.o
			gcc sendfile.c -o sendfile

sendfile.o : sendfile.c
			gcc -c sendfile.c -o sendfile.o

recvfile : recvfile.o
			gcc recvfile.c -o recvfile

recvfile.o : recvfile.c
			gcc -c recvfile.c -o recvfile.o



clean :
			-rm -f sendfile.o
			-rm -f sendfile
			-rm -f recvfile.o
			-rm -f recvfile