#Name: Ethan Chan
#email: ethancha@usc.edu

CC = g++
CFLAGS = -Wall
CSOCK = -lsocket -lnsl -lresolv

all: projb

projb: projb.o
	sudo $(CC) $(CFLAGS) -o projb projb.o

projb.o: projb.cpp
	sudo $(CC) $(CFLAGS) -c projb.cpp

clean:
	sudo rm -f *.o projb *~
