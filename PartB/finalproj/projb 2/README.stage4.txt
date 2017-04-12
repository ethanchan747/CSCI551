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

c) i)load balanced because of the mod. 
ii) yes
iii) when all the pings are directed in a specific subnet