Writeup by Ethan Chan 1801654130

************************************* IMPORTANT! **************************************
After sudo ./proja config_file is ran, wait until message “end of program” is printed on the terminal before accessing the stage2.proxy.out and stage2.router1.out files. This will be approximately 11 seconds after the last ping was sent.

Program will exit if ICMP message is not sent within a 10 second window

Also BEFORE running sudo ./proja config_file
YOU MUST RUN:
sudo ip tuntap add dev tun1 mode tunsudo ifconfig tun1 10.5.51.2/24 up
************************************* IMPORTANT! **************************************

Things I learned: I learned how to use the select() function and how to create and use tunnels.

a) Yes I used code from Beej’s Guide to Network Programming and I used code from Unix Network Programming by W. Richard Stevens.

b) Yes I completed this stage

c) My code could run across some difficulties if the proxy and the router were implemented on different CPU architectures because the data could store the IP header and ICMP header differently due to the different architectures. The length of the IP header would still be the same but depending on the bits assigned to a char, (8 bits, 16 bits, 32 bits) I would have to multiply the length of the IP header accordingly to match the size of that char. Say a char was 8 bits, that would mean 1 IP header length would be equal to 4 chars so to balance the equation we would multiply 1 IP header length unit by 4.