#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h> //nice! we need this!
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h> //_exit, fork
//Ethan's additions
#include <iostream>
#include <fstream>
#include <sys/types.h> /* pid_t */
#include <sys/wait.h>  /* waitpid */
//Things to learn
//File I/O, fork(), socket programming
#include <pthread.h>
#include <netdb.h>
#include <string>
#include <deque>
#include <sys/select.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h> //added for project B
#include <ifaddrs.h>
#include <string.h>
#include <cstdlib>
#include <vector>
#include <map>
#include <iomanip>
#include "aes-test.h"

using namespace std; //include this right after headers

int main(int argc, char *argv[])
{
    int statusraw;
    int rawsockbind;
    struct addrinfo *rawinfo;
    struct addrinfo rawhints;
    char nameit[] = "eth1";
    char IP_OF_ETH1[NI_MAXHOST];
    int rawsockfd;
    char test[NI_MAXHOST];
    char salmon[] = "eth0";
    find_ipaddr(salmon,test,NULL);
    printf("\taddress: <%s>\n", test);
    
    find_ipaddr(nameit, IP_OF_ETH1, NULL); //retrieving ip address of eth1
    
    memset(&rawhints, 0, sizeof rawhints);
    rawhints.ai_family = AF_INET; // use IPv4
    rawhints.ai_socktype = SOCK_RAW; // RAW stream sockets
    
    statusraw = getaddrinfo(IP_OF_ETH1, NULL, &rawhints, &rawinfo);
    if (statusraw != 0){
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(statusraw));
        return 1;
    }
    
    rawsockfd = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP); //created raw socket IP header created for us
    
    if(rawsockfd == -1){
        perror("create raw error");
        cout << "raw socket" << endl;
        return 1;
    }
    
    
    rawsockbind = bind(rawsockfd, rawinfo->ai_addr,rawinfo->ai_addrlen); //binding raw socket
    
    
    if(rawsockbind == -1){
        perror("bind error");
        cout << "raw socket" << endl;
        return 1;
    }
    
    
    
    return 1;
}