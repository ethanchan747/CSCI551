Writeup by Ethan Chan 1801654130

a) Yes I used code from Beej’s Guide to Network Programming and I used code from Unix Network Programming by W. Richard Stevens.

b) Yes I completed this stage. The program will exit if ICMP/TCP not sent within 20seconds.

c) The RST packets sends a Reset bit which is designed to permit a station to abort a TCP connection with another station which could cause our TCP connection to be lost.

d) RST packets determines a field in the flags portion of the TCP header and ICMP and UDP packets do not have an TCP header so the kernel does not generate a RST packet for ICMP or UDP packets