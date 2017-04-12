#Name: Ethan Chan
#email: ethancha@usc.edu

CC=g++
CFLAGS=-c -g -Wall
LDFLAGS=-lcrypto
SOURCES= projc8.cpp aes-test.cpp
OBJECTS=$(SOURCES:.cpp=.o)

HW1=projc8
$(HW1):$(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@
		     
clean:
	rm $(OBJECTS)
	rm $(HW1)