#Name: Ethan Chan
#email: ethancha@usc.edu

CC = g++
CFLAGS = -Wall
CSOCK = -lsocket -lnsl -lresolv

all: proja

proja: proja.o
	sudo $(CC) $(CFLAGS) -o proja proja.o

proja.o: proja.cpp
	sudo $(CC) $(CFLAGS) -c proja.cpp

clean:
	sudo rm -f *.o proja *~