Writeup by Ethan Chan 1801654130

a) Yes I used code from Beej’s Guide to Network Programming and I used code from Unix Network Programming by W. Richard Stevens.

b) Yes I completed this stage. The program will exit if ICMP/TCP not sent within 20seconds.

c) If different packets from one flow took different paths of different lengths the packets sent could arrive out of order and after 3 out of order packet arrivals, TCP will reduce its sending window size using slow start.

d) There will be still a chance of out of order arrival but the out of order arrivals will decrease because the paths are of the same size and occur in one machine.

e)Now that we have multiple machines involved where the path to each machine requires different number of hops we once again increase the likelihood of out of order arrivals.