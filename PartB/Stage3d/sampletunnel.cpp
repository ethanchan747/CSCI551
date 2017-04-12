/* sample.cpp */
/*
 * sample code to open tunnel interface and to read data from it.
 */

/*
* This code "USC CSci551 FA2014 Projects A and B" is
* Copyright (C) 2014 by Zi Hu.
* All rights reserved.
*
* This program is released ONLY for the purposes of Fall 2014 CSci551
* students who wish to use it as part of their project assignments.
* Use for another other purpose requires prior written approval by
* Zi Hu.
*
* Use in CSci551 is permitted only provided that ALL copyright notices
* are maintained and that this code is distinguished from new
* (student-added) code as much as possible.  We new services to be
* placed in separate (new) files as much as possible.  If you add
* significant code to existing files, identify your new code with
* comments.
*
* As per class assignments, use of any code OTHER than this provided
* code requires explicit approval, ahead of time, by the professor.
*
*/


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

/**************************************************************************
 * tun_alloc: allocates or reconnects to a tun/tap device. 
 * copy from from simpletun.c, used with permission of the author
 * refer to http://backreference.org/2010/03/26/tuntap-interface-tutorial/ for more info 
 **************************************************************************/

using namespace std; //include this right after headers


uint16_t ip_checksum(void* vdata,size_t length) {
    // Cast the data pointer to one that can be indexed.
    char* data=(char*)vdata;
    
    // Initialise the accumulator.
    uint32_t acc=0xffff;
    
    // Handle complete 16-bit blocks.
    for (size_t i=0;i+1<length;i+=2) {
        uint16_t word;
        memcpy(&word,data+i,2);
        acc+=ntohs(word);
        if (acc>0xffff) {
            acc-=0xffff;
        }
    }
    
    // Handle any partial block at the end of the data.
    if (length&1) {
        uint16_t word=0;
        memcpy(&word,data+length-1,1);
        acc+=ntohs(word);
        if (acc>0xffff) {
            acc-=0xffff;
        }
    }
    
    // Return the checksum in network byte order.
    return htons(~acc);
}


int find_ipaddr(char *name, char *IP_address)//note char *IP_address must be size [NI_MAXHOST];
{
    
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];
    
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }
    
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;
        if(strcmp(ifa->ifa_name, name) != 0)
            continue;
        
        family = ifa->ifa_addr->sa_family;
        if(family == AF_INET){
            s = getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),host, NI_MAXHOST,NULL,0,NI_NUMERICHOST);
            if (s != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }
        }
        
    }
    
    if(IP_address != NULL){
        strcpy(IP_address, host);
        printf("\tnetwork interfaces' address is: <%s>\n", IP_address);
    }
    return 1;
}


int tun_alloc(char *dev, int flags) 
{
    struct ifreq ifr;
    int fd, err;
    char *clonedev = (char*)"/dev/net/tun";

    if( (fd = open(clonedev , O_RDWR)) < 0 ) 
    {
	perror("Opening /dev/net/tun");
	return fd;
    }

    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = flags;

    if (*dev) 
    {
	strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }

    if( (err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0 ) 
    {
	perror("ioctl(TUNSETIFF)");
	close(fd);
	return err;
    }

    strcpy(dev, ifr.ifr_name);
    return fd;
}


int tunnel_reader()
{
    //New variables added
    fd_set readfds;
    struct timeval tv;
    struct addrinfo *rawinfo;
    struct addrinfo rawhints;
    char nameit[] = "eth1";
    char IP_OF_ETH1[NI_MAXHOST];
    int rawsockfd, rawsockbind, statusraw, n, recv_bytes, statusr;
    
    find_ipaddr(nameit, IP_OF_ETH1); //retrieving ip address of eth1
    
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

    //New variables added
    
    char tun_name[IFNAMSIZ];
    char buffer[2048];

    /* Connect to the tunnel interface (make sure you create the tunnel interface first) */
    strcpy(tun_name, "tun1");
    int tun_fd = tun_alloc(tun_name, IFF_TUN | IFF_NO_PI); 

    if(tun_fd < 0)
    {
	perror("Open tunnel interface");
	exit(1);
    }


    /*
     * This loop reads packets from the tunnle interface.
     *
     * You will need to REWRITE this loop into a select loop,
     * so that it can talk both to the tun interface,
     * AND to the router.
     *
     * You will also probably want to do other setup in the
     * main() routine.
     */
    while(1) 
    {
        
        FD_ZERO(&readfds);
        
        //adding our descriptors to the set
        FD_SET(tun_fd, &readfds);
        FD_SET(rawsockfd, &readfds);

        if(tun_fd > rawsockfd){
            n = tun_fd + 1;
        }
        else if(tun_fd <= rawsockfd){
            n = rawsockfd + 1;
        }
        
        tv.tv_sec = 10;
        tv.tv_usec = 500000;
        
        select(n, &readfds, NULL, NULL, &tv);
        
	/* Now read data coming from the tunnel */
        
         if(FD_ISSET(tun_fd, &readfds)){
        int nread = read(tun_fd,buffer,sizeof(buffer));
	if(nread < 0) 
	{
	    perror("Reading from tunnel interface");
	    close(tun_fd);
	    exit(1);
	}
	else if(nread == 84)
	{
	    printf("Read a packet from tunnel, packet length:%d\n", nread);
        
        
        struct ip * tun_ICMP;      //stores ICMP message in tun_ICMP
        //struct in_addr tun_src;  //test
        struct in_addr tun_dest;
        //char tun_ip_src[NI_MAXHOST];
        char tun_ip_dest[NI_MAXHOST];
        struct icmp * tun_rICMP;
        //u_char tun_utype;
        int hlenl;
        
        
        //set up ICMP message
        tun_ICMP = (struct ip *)buffer;
        hlenl = tun_ICMP->ip_hl;
        tun_rICMP = (struct icmp *)(buffer + hlenl*4);
        
        struct iovec iov[1];
        iov[0].iov_base= tun_rICMP; //message is the ICMP header + payload (tun_rICMP or icmp)
        iov[0].iov_len = 64; // *4 because of char buffer? well no, iov_len is measured in bytes and a char is a byte
        //set up ICMP message
        
        //setting up destination IP address
        struct addrinfo *ipdest2;
        struct addrinfo ipdest1;
        
        memset(&ipdest1, 0, sizeof ipdest1); // make sure the struct is empty
        
        ipdest1.ai_family = AF_INET; // use IPv4
        ipdest1.ai_socktype = SOCK_RAW; // RAW stream sockets
        
        tun_dest = tun_ICMP->ip_dst;
        inet_ntop(AF_INET, &tun_dest, tun_ip_dest, sizeof(tun_ip_dest));
        
        statusr = getaddrinfo(tun_ip_dest, NULL, &ipdest1, &ipdest2);
        if (statusr != 0){
            fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(statusr));
            return 1;
        }
        
        struct sockaddr_in *eth1 = (struct sockaddr_in *) rawinfo->ai_addr;
        struct sockaddr_in rawout;
        rawout = *eth1; //currently we are trying test send this message back to our own raw socket and it works! let's see what we sent below
        //setting up destination IP address
        
        struct msghdr message;
        memset(&message, 0, sizeof(message));
        message.msg_name= ipdest2->ai_addr; //struct sockaddr_in *  (either &rawout or ipdest2->ai_addr
        message.msg_namelen= sizeof(struct sockaddr_in); //size of ai_addr in bytes (use sizeof() returns in bytes)
        message.msg_iov=iov;
        message.msg_iovlen=1;
        message.msg_control=0;
        message.msg_controllen=0;
        
        if(sendmsg(rawsockfd, &message, 0) == -1){
            cout << "error in sending raw sock" << endl;
        }

    }
        
	    /* Do whatever with the data, function to manipulate the data here */

	    /*
	     * For project A, you will need to add code to forward received packet
	     * to router via UDP socket.
	     * And when you get icmp echo reply packet from router, you need to write
	     * it back to the tunnel interface
	     */
	    
	}
    if(FD_ISSET(rawsockfd, &readfds)){
        cout << "message waiting at rawsock" << endl;
        
        
        char recbuf[84];
        struct iovec iovr[1];
        iovr[0].iov_base= recbuf; //message is the ICMP header + payload
        iovr[0].iov_len = sizeof(recbuf);
        
        struct msghdr msgh;
        memset(&msgh,0, sizeof(msgh));
        msgh.msg_iov=iovr;
        msgh.msg_iovlen=1;
        msgh.msg_control=0;
        msgh.msg_controllen=0;
        
        recv_bytes = recvmsg(rawsockfd,&msgh,0); //bTODO this is all I have to
        
        if(recv_bytes){
        
            cout << "read message successfully" << endl;
            
        //struct sockaddr_in *recvaddr;
       
        struct ip * tun_ICMP;      //stores ICMP message in tun_ICMP
        struct in_addr tun_src;  //test
        struct in_addr tun_dest;
        //struct sockaddr_in eth0;
        char tun_ip_src[NI_MAXHOST];
        char tun_ip_dest[NI_MAXHOST];
        struct icmp * tun_rICMP;
        u_char tun_utype =0;
        int hlenl;
       
            tun_ICMP = (struct ip *)recbuf;
            tun_src = tun_ICMP->ip_src;
            inet_ntop(AF_INET, &tun_src, tun_ip_src, sizeof(tun_ip_src));
            
            inet_pton(AF_INET,"10.0.2.15",&tun_ICMP->ip_dst);
            
            tun_dest = tun_ICMP->ip_dst;
            inet_ntop(AF_INET, &tun_dest, tun_ip_dest, sizeof(tun_ip_dest));
            
            cout << "router says source is :" << tun_ip_src << endl;
            cout << "router says destination is: " << tun_ip_dest << endl;
            
            hlenl = tun_ICMP->ip_hl;
            tun_rICMP = (struct icmp *)(recbuf + hlenl*4); // char = 8 so mult by 4
            tun_utype = tun_rICMP->icmp_type;
            
            cout << "this is the icmp type";
            
            //tun_utype = 0;
            
            if(tun_utype == 8){
                
                cout << " 8" << endl;
            }
            if(tun_utype == 0){
                cout << " 0" << endl;
            }
            
            
            tun_ICMP->ip_sum = 0;
            tun_ICMP->ip_sum = ip_checksum(tun_ICMP,20);
            
            int nwrite = write(tun_fd,recbuf,sizeof(recbuf));
            if(nwrite < 0)
            {
                perror("Writing to tunnel interface");
                close(tun_fd);
                exit(1);
            }
            
            
            
        }
        
        
    }
        
    }
}

int main(int argc, char** argv)
{
	/*
	 * For a real proja, you will want to do some setup here.
	 */
	tunnel_reader();
}
