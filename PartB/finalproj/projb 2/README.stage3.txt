Writeup by Ethan Chan 1801654130

Things I learned: I re-learned how to have the kernel assign a dynamically allocated port number to a socket.

a) Yes I used code from Beej’s Guide to Network Programming and referenced some code from Wikipedia for forking.

b) Yes I completed this stage  code will exit if ICMP not sent within 10seconds. Wait till program finishes before opening files

c) you need to rewrite the source to create a correct ICMP message

d) We are separating addresses to provide for anonymity

e) outgoing packets are encapsulated