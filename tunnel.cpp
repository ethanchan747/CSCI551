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
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
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
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

/**************************************************************************
 * tun_alloc: allocates or reconnects to a tun/tap device. 
 * copy from from simpletun.c, used with permission of the author
 * refer to http://backreference.org/2010/03/26/tuntap-interface-tutorial/ for more info 
 **************************************************************************/

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
    
    /* /Users/ethanchan/Desktop/Graduate Year/Fall 2014/CSCI551/Project/sample_tunnel (1).c */
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
	/* Now read data coming from the tunnel */
        int nread = read(tun_fd,buffer,sizeof(buffer));
	if(nread < 0) 
	{
	    perror("Reading from tunnel interface");
	    close(tun_fd);
	    exit(1);
	}
	else
	{
        
        struct ip * tun_IP;      //stores ICMP message in tun_ICMP
        struct in_addr tun_src;  //test
        struct in_addr tun_dest;
        char tun_ip_src[40];
        char tun_ip_dest[40];
        struct icmp * tun_ICMP;
        int hlenl;
        
        
        tun_IP = (struct ip *)buffer;
        tun_src = tun_IP->ip_src;
        inet_ntop(AF_INET, &tun_src, tun_ip_src, sizeof(tun_ip_src));
        
        printf("source is:%s\n", tun_ip_src);
        
        tun_dest = tun_IP->ip_dst;
        inet_ntop(AF_INET, &tun_dest, tun_ip_dest, sizeof(tun_ip_dest));
        
        
        printf("destination is:%s\n", tun_ip_dest);

        
        tun_IP->ip_src = tun_dest;
        tun_IP->ip_dst = tun_src;
        
        
        tun_IP = (struct ip *)buffer;
        tun_src = tun_IP->ip_src;
        inet_ntop(AF_INET, &tun_src, tun_ip_src, sizeof(tun_ip_src));
        
        printf("source is:%s\n", tun_ip_src);
        
        tun_dest = tun_IP->ip_dst;
        inet_ntop(AF_INET, &tun_dest, tun_ip_dest, sizeof(tun_ip_dest));
        
        
        printf("destination is:%s\n", tun_ip_dest);
        
        
        hlenl = tun_IP->ip_hl;
        //hlenl=(unsigned short )* buffer&0x00ff;

        printf("length = %d\n", hlenl);
        
        if(sizeof(char) == 2){
        tun_ICMP = (struct icmp *)(buffer + hlenl*2);
            printf("char = 16");
        }
        if(sizeof(char) == 1){
            tun_ICMP = (struct icmp *)(buffer + hlenl*4);
            printf("char = 8");
        }
        if(sizeof(char) == 4){
            tun_ICMP = (struct icmp *)(buffer + hlenl);
            printf("char = 32");

        }
        
        
        /*
        cout << "router says source is :" << tun_ip_src << endl;
        cout << "router says destination is :" << tun_ip_dest << endl;
        cout << "router says type is: " << tun_utype << endl;
        cout << "received checksum is: " << tun_rICMP->icmp_cksum << endl;
        */

        tun_ICMP->icmp_type = 0;
        
        /*
        strcpy(buffer,(char *)tun_ICMP);
        strcat(buffer,(char *)tun_rICMP);
        */
        printf("type is:%c\n", tun_ICMP->icmp_type);
        
	    printf("Read a packet from tunnel, packet length:%d\n", nread);

        int nwrite = write(tun_fd,buffer,sizeof(buffer));
        
        if(nwrite < 0)
        {
            perror("Writing to tunnel interface");
            close(tun_fd);
            exit(1);
        }

	    /* Do whatever with the data, function to manipulate the data here */

	    /*
	     * For project A, you will need to add code to forward received packet
	     * to router via UDP socket.
	     * And when you get icmp echo reply packet from router, you need to write
	     * it back to the tunnel interface
	     */
	            printf("done");
	}
    }
    
    printf("done");
}

int main(int argc, char** argv)
{
	/*
	 * For a real proja, you will want to do some setup here.
	 */
	tunnel_reader();
}
