Writeup by Ethan Chan 1801654130

Things I learned: I re-learned how to have the kernel assign a dynamically allocated port number to a socket.

a) Yes I used code from Beej’s Guide to Network Programming and referenced some code from Wikipedia for forking.

b) Yes I completed this stage

c) My code should be able to work between different CPU architectures because they both exchange data through sockets and store them in char buffers. Even if different memory allocation space were different for each char, ranging from 8 bits, 16 bits, or 32 bits, they would still be able to share their resources.