//
//  play.cpp
//  
//
//  Created by Ethan Chan on 9/25/14.
//
/*Errors to fix: I need to wait for ALL children to finish until I exit parent. Also I need to store the individual pid_t pid for each child? perhaps make a vector or (?) of pids? Or can I use getpid() within each child to get its pid? need to make cstring str1 infinitely long?

 Notes: we are using IPv4, don't forget to free socks and res, sendt(inclues own socket, address of other's destination socket, we NEED to for PART B set routernum = num, num we read in from config_file
    In stage 2 we added the tunneling part and therefore increased msg and buf size to 2048. Search for TODO
    Maybe later we need this but since children exitng when parents exits its ok
    
    We have received successfully 4 ICMP messages from tun1. We now need to send them to the router, have the router send them back with type changed, and write these results to a file. How do we see what is in the ICMP message? All I get is an E.
 
    If you do the math, you'll see where these 84 byetes come from: 20 are for the IP header, 8 for the ICMP header, and 56 are the payload of the ICMP echo message as you can see when you run the ping command:
 
 
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


using namespace std; //include this right after headers


/**************************************************************************
 * tun_alloc: allocates or reconnects to a tun/tap device.
 * copy from from simpletun.c, used with permission of the author
 * refer to http://backreference.org/2010/03/26/tuntap-interface-tutorial/ for more info
 **************************************************************************/

int tun_alloc(char *dev, int flags) /* char* is name of the interface (tun1)*/
{
    struct ifreq ifr;
    int fd, err;
    char *clonedev = (char*)"/dev/net/tun";
    
    if( (fd = open(clonedev , O_RDWR)) < 0 )
    {
        perror("Opening /dev/net/tun");
        return fd;
    }
    
    //Users/ethanchan/Desktop/Graduate Year/Fall 2014/CSCI551/Project/sample_tunnel (1).c
    
    memset(&ifr, 0, sizeof(ifr));
    
    ifr.ifr_flags = flags; /* IFF_TUN or IFF_TAP plus maybe IFF_NO_PI */
    
    /* if a device name was specified, put it in the structure; otherwise,
     * the kernel will try to allocate the "next" device of the
     * specified type */
    if (*dev)
    {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ); /* IFNAMESIZ is probably name of tunnle name */
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


int tunnel_reader(int sockfd, struct sockaddr_in router_addr, ofstream *ofile) // sockfd of Proxy is well known
{
    char tun_name[IFNAMSIZ];
    char tun_buf[2048];
    char router_buf[2048];
    int tun_fd, n; //choosing between tunnel interface or socket interface
    fd_set readfds;
    struct timeval tv;
    int rv;


    
    struct ip * tun_ICMP;      //stores ICMP message in tun_ICMP
    struct in_addr tun_src;  //test
    struct in_addr tun_dest;
    char tun_ip_src[40];
    char tun_ip_dest[40];
    struct icmp * tun_rICMP;
    u_char tun_utype;
    int hlenlr, hlenlt;
    
    struct ip * router_ICMP;      //stores ICMP message in tun_ICMP
    struct in_addr router_src;  //test
    struct in_addr router_dest;
    char router_ip_src[40];
    char router_ip_dest[40];
    struct icmp * router_rICMP;
    u_char router_utype;
    
    struct sockaddr_in their_addr;        //stores origns of address from router msg
    socklen_t addr_len = sizeof(their_addr);
    
    /* Connect to the tunnel interface (make sure you create the tunnel interface first) */
    strcpy(tun_name, "tun1");
    tun_fd = tun_alloc(tun_name, IFF_TUN | IFF_NO_PI);
    
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
    
    /* code used for setting up for select() function */
    //clearing the read set
    FD_ZERO(&readfds);
    
    //adding our descriptors to the set
    FD_SET(tun_fd, &readfds);
    FD_SET(sockfd, &readfds);
    
    if(tun_fd > sockfd){
        n = tun_fd + 1;
    }
    else if(tun_fd <= sockfd){
        n = sockfd + 1;
    }
    
    //timerouts = 10 sec plus 50,000 microseconds = 10.5 seconds
    tv.tv_sec = 10;
    tv.tv_usec = 500000;
    
    rv = select(n, &readfds, NULL, NULL, &tv);

    
    while(rv) // rv can be equal to -1 (error), 0 (timerout), or other received data
    {
        
        if(FD_ISSET(tun_fd, &readfds)){
            /* Now read data coming from the tunnel */
            int nread = read(tun_fd,tun_buf,sizeof(tun_buf));
            
            if(nread < 0)
                {
                perror("Reading from tunnel interface");
                close(tun_fd);
                exit(1);
                }
            else
                {
                printf("Read a packet from tunnel, packet length:%d\n", nread);
                    
                    //This is the print out of the ICMP message

                    tun_ICMP = (struct ip *)tun_buf;
                    tun_src = tun_ICMP->ip_src;
                    inet_ntop(AF_INET, &tun_src, tun_ip_src, sizeof(tun_ip_src));
                    
                    tun_dest = tun_ICMP->ip_dst;
                    inet_ntop(AF_INET, &tun_dest, tun_ip_dest, sizeof(tun_ip_dest));
                    
                    
                    hlenlt = tun_ICMP->ip_hl;
                    tun_rICMP = (struct icmp *)(tun_buf + hlenlt*4);
                    tun_rICMP->icmp_type = 8;
                    tun_utype = tun_rICMP->icmp_type;
                    
                    /*
                    
                    cout << "source in tun_buf is :" << tun_ip_src << endl;
                    cout << "dest in tun_buf is :" << tun_ip_dest << endl;
                    cout << "type is: " << tun_utype << endl;
                    
                     */
                     
                    *ofile << "ICMP from tunnel, src: " << tun_ip_src << ", dst: " << tun_ip_dest << ", type is: ";
                    
                    if(tun_utype == 8){
                        *ofile << "8\n";
                    }
                    
                    //TODO if more than one router we need to send to ALL (use for loop)
                    sendto(sockfd, tun_buf, 2048, 0, (struct sockaddr *)&router_addr, sizeof(router_addr));

                    //TODO
                /* Do whatever with the data, function to manipulate the data here */
            
                /*
                 * For project A, you will need to add code to forward received packet
                 * to router via UDP socket.
                 * And when you get icmp echo reply packet from router, you need to write
                 * it back to the tunnel interface
                 */
            
                }
        }
        if(FD_ISSET(sockfd, &readfds)) {
            recvfrom(sockfd, router_buf, 2048, 0, (struct sockaddr *)&their_addr, &addr_len);

            
            router_ICMP = (struct ip *)router_buf;
            router_src = router_ICMP->ip_src;
            inet_ntop(AF_INET, &router_src, router_ip_src, sizeof(router_ip_src));
            
            router_dest = router_ICMP->ip_dst;
            inet_ntop(AF_INET, &router_dest, router_ip_dest, sizeof(router_ip_dest));

            
            hlenlr = router_ICMP->ip_hl;
            router_rICMP = (struct icmp *)(router_buf + hlenlr*4);
            router_utype = router_rICMP->icmp_type;

            /*
            cout << "source is router :" << router_ip_src << endl;
            cout << "dest is tun1 :" << router_ip_dest << endl;
            cout << "type is: " << router_utype << endl;
             */
             
            *ofile << "ICMP from port: " << ntohs(their_addr.sin_port) << ", src: " << router_ip_src << ", dst: " << router_ip_dest << ", type is: ";
            
            if(router_utype == 0){
                *ofile << "0\n";
            }
            

            //TODO write to tunnel
            int nwrite = write(tun_fd,router_buf,sizeof(router_buf));
            if(nwrite < 0)
            {
                perror("Writing to tunnel interface");
                close(tun_fd);
                exit(1);
            }
        }
        
        //we get new values for rv
        FD_ZERO(&readfds);
        
        //adding our descriptors to the set
        FD_SET(tun_fd, &readfds);
        FD_SET(sockfd, &readfds);
        
        if(tun_fd > sockfd){
            n = tun_fd + 1;
        }
        else if(tun_fd <= sockfd){
            n = sockfd + 1;
        }
        
        //timerouts = 10 sec plus 50,000 microseconds = 20.5 seconds
        tv.tv_sec = 10;
        tv.tv_usec = 500000;
        
        rv = select(n, &readfds, NULL, NULL, &tv);
        //we get new values for rv
        
        
    }

    if (rv == -1) {
        perror("select"); // error occured in select()
    }
    //cout << "end of tun_read()" << endl;
    ofile->close();
    return 1;
}


int main(int argc, char** argv)
{
    ofstream ofile;
    deque<pid_t> my_deq;
    string str1;     //should make this cstring infinitely long maybe string
    string str2 = "stage 1";
    string str4 = "stage 2";
    int stage1 = 0;
    int stage2 = 0;
    char str3[15];
    
    int routernum = 1;  //keep this as one for now
    //int num;    //should be removed and replaced with routernum
    pid_t pid;
    int proxy_port;
    int one = 1;
    
    if(argc < 2){
        cout << "usage: ./proja config_file" << endl;
        return 1;
    }
    
    ifstream ifile (argv[1]);
    getline(ifile,str1,'#');
    getline(ifile,str1);
    
    /*
    cout << "\nThis is the comment of the config_file: \n" << str1 << endl;
    */
     
    getline(ifile,str1);
    
    if( str1 == str2 ) {
       cout << "\nWe are in Stage 1 " << endl;
        stage1 = 1;
    }
    else if(str1 == str4){
        cout << "\nWe are in Stage2 " << endl;
        stage2 = 1;
    }
    
       getline(ifile,str1,' ');
        ifile.getline(str3,15);
       
       //num = atoi(str3);        //replace num with routernum
    /*
    cout << "The number of routers is: " << num << "\n" << endl; //replace num with router num
     */
    
    ifile.close();
    
    /* Setting up dynamic UDP port for Onion Proxy */
    struct addrinfo hints;
    struct addrinfo *res;
    int sockfd, sockbind, status, get;
    struct sockaddr_in *ipv4; //structure for ipv4
    socklen_t addrlen1 = sizeof(*ipv4);
    
    
    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    
    hints.ai_family = AF_INET; // use IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP stream sockets
    
    status = getaddrinfo("localhost", NULL, &hints, &res);
    if (status != 0){
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return 1;
    }
    
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    
    if(sockfd == -1){
        perror("socket error");
    }
    
    
    /* supposedly sets the port number to 0 and makes kernal assign a number */
    ipv4 = (struct sockaddr_in *)res->ai_addr;
    ipv4->sin_port = 0;
    /* supposedly sets the port number to 0 and makes kernal assign a number */
    
    
    sockbind = bind(sockfd, res->ai_addr, res->ai_addrlen);
    
    
    if(sockbind == -1){
        perror("bind error");
        return 1;
    }
    
    get = getsockname(sockfd, (struct sockaddr *)ipv4, &addrlen1);
    
    if(get == 0 && ipv4->sin_family == AF_INET && addrlen1 ==sizeof(*ipv4)){
        proxy_port = ntohs(ipv4->sin_port);
    }
    
    
    /*finished setting up dynamic UDP port for Onion Proxy */
    
    /*
    cout << "We have just finished setting up the Onion Proxy:" << endl;
    cout << "1. Means message from Parent (Onion Proxy)" << endl;
    cout << "2. Means message from Child (Router)" << endl;
    cout << "12. Means both should print this statement \n" << endl;
    
    cout << "1. The proxy port number is: " << proxy_port << endl;
     */
     
    if(routernum){
        //forking methode copied from wikipedia
         pid = fork(); // after this function is called both parent AND child return from this function to this point
        routernum = routernum - 1;
        if(pid){ //if you're parent then push back the pid
        my_deq.push_back(pid);
        }
    }
    
    /*
    cout << "12. This is int routernum value: " << routernum << endl;
    */
     
     
    if (pid == -1) {
        // When fork() returns -1, an error happened.
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {

        // When fork() returns 0, we are in the child process.
        //printf("2. Hello from the child process!\n");
        
        // socket programming copied from page 19 of Beej's Guide to Network Programming
        
        /* Setting up dynamic UDP port for Onion Router */
        struct addrinfo hintsr;
        struct addrinfo *resr;
        int sockfdr, sockbindr, dynamic_portr, statusr, getr;
        struct sockaddr_in *ipv4r; //structure for ipv4
        socklen_t addrlen1r = sizeof(*ipv4r);
        char msg[2048];
        int child = (int) getpid();
        sprintf(msg, "%d",child);   //stores PID in msg succes
        
        /* Stage 2 variables */
        struct sockaddr_in their_addr;
        char buf[2048];
        socklen_t addr_len = sizeof(their_addr);
        int recv_bytes = 0;
        
        fd_set writefds;
        struct timeval tv;
        int rv, n;
        
        memset(&hintsr, 0, sizeof hintsr); // make sure the struct is empty
        
        hintsr.ai_family = AF_INET; // use IPv4
        hintsr.ai_socktype = SOCK_DGRAM; // UDP stream sockets
        
        statusr = getaddrinfo("localhost", NULL, &hintsr, &resr);
        if (statusr != 0){
            fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(statusr));
            return 1;
        }
        
        sockfdr = socket(resr->ai_family, resr->ai_socktype, resr->ai_protocol);
        
        if(sockfdr == -1){
            perror("socket error");
        }
        
        
        /* supposedly sets the port number to 0 and makes kernal assign a number */
        ipv4r = (struct sockaddr_in *)resr->ai_addr;
        ipv4r->sin_port = 0;
        /* supposedly sets the port number to 0 and makes kernal assign a number */
        
        
        sockbindr = bind(sockfdr, resr->ai_addr, resr->ai_addrlen);
        
        
        if(sockbindr == -1){
            perror("bind error");
            return 1;
        }
        
        
        getr = getsockname(sockfdr, (struct sockaddr *)ipv4r, &addrlen1r);
        
        if(getr == 0 && ipv4r->sin_family == AF_INET && addrlen1r ==sizeof(*ipv4r)){
            dynamic_portr = ntohs(ipv4r->sin_port);
        }
        
        /*
        cout << "2. My dynamic port is: " << dynamic_portr << endl;
        cout << "2. Onion Proxy's port port is: " << proxy_port << endl;
        */
         
        ofstream ofiler;
        if(stage1){
        ofiler.open("stage1.router1.out");
        }
        else if(stage2){
            ofiler.open("stage2.router1.out");
        }
        ofiler << "router: " << one << ", pid: " << msg << ", port: " << dynamic_portr << "\n";
        
        
        sendto(sockfdr, msg, 2048, 0, res->ai_addr, res->ai_addrlen);  // res is the local variable of the addrinfo res from Onion router but we send from our own sock "sockfdr"
        
        if(stage1){
        one = one + 1;
        /*finished setting up dynamic UDP port for Onion Router */
        /* Sending "I'm up" message to Onion Proxy */

        /* Sending "I'm up" message to Onion Proxy */
        
        //cout << "2. End of Stage 1 of Router" << endl;
        goto stage1done;
        }
        /* starting Stage 2 */
        //cout << "2. Stage 2 of Router Begin" << endl;
        
        
        FD_ZERO(&writefds);
        
        //adding our descriptors to the set
        FD_SET(sockfdr, &writefds);
        
        n = sockfdr + 1;
        
        //timerouts = 15 sec plus 50,000 microseconds = 20.5 seconds
        tv.tv_sec = 11;
        tv.tv_usec = 500000;
        
        rv = select(n, &writefds, NULL, NULL, &tv);
        
        
        while(rv){  /* reads forever */
        
            if(FD_ISSET(sockfdr, &writefds)){
                
            recv_bytes = recvfrom(sockfdr, buf, 2048, 0, (struct sockaddr *)&their_addr, &addr_len);
            
            
            if(recv_bytes){
                //strcpy(msg, "ping back"); //TODO
                
                //cout << "router got message" << endl;
                struct ip * tun_ICMP;      //stores ICMP message in tun_ICMP
                struct in_addr tun_src;  //test
                struct in_addr tun_dest;
                char tun_ip_src[40];
                char tun_ip_dest[40];
                struct icmp * tun_rICMP;
                u_char tun_utype;
                int hlenl;

                
                tun_ICMP = (struct ip *)buf;
                tun_src = tun_ICMP->ip_src;
                inet_ntop(AF_INET, &tun_src, tun_ip_src, sizeof(tun_ip_src));
                
                tun_dest = tun_ICMP->ip_dst;
                inet_ntop(AF_INET, &tun_dest, tun_ip_dest, sizeof(tun_ip_dest));
                
                /*
                cout << "router says source is :" << tun_ip_src << endl;
                cout << "router says destination is :" << tun_ip_dest << endl;
                */
                 
                tun_ICMP->ip_src = tun_dest;
                tun_ICMP->ip_dst = tun_src;
                
                
                hlenl = tun_ICMP->ip_hl;
                tun_rICMP = (struct icmp *)(buf + hlenl*4); // char = 8 so mult by 4
                tun_utype = tun_rICMP->icmp_type;
                
                ofiler << "ICMP from port: " << ntohs(their_addr.sin_port) << ", src: " << tun_ip_src << ", dst: " << tun_ip_dest << ", type: ";
                
                if(tun_utype == 8){
                    //cout << "router says type is 8" << endl;
                    ofiler << "8\n";
                }

                
                tun_rICMP->icmp_type = 0;
                tun_utype = tun_rICMP->icmp_type;

                if(tun_utype == 0){
                    //cout << "router sends type as 0" << endl;
                }

/*
                strcpy(buf,(char *)tun_ICMP);
                strcat(buf,(char *)tun_rICMP);
*/
                sendto(sockfdr, buf, sizeof(buf), 0, res->ai_addr, res->ai_addrlen); // send message back to proxy
                recv_bytes = 0;
            }
            }
            
            FD_ZERO(&writefds);
            
            //adding our descriptors to the set
            FD_SET(sockfdr, &writefds);
            
            n = sockfdr + 1;
            
            //timerouts = 15 sec plus 50,000 microseconds = 20.5 seconds
            tv.tv_sec = 11;
            tv.tv_usec = 500000;
            
            rv = select(n, &writefds, NULL, NULL, &tv);
            
            
        }
        
        stage1done:
        ofiler.close();         //
        _exit(EXIT_SUCCESS);  // exit() is unreliable here, so _exit must be used
    }
   else {
       
       // When fork() returns a positive number, we are in the parent process
       // and the return value is the PID of the newly created child process.
       //printf("Hello from the parent process!\n");
       
       int bytes_rec = 0;
       struct sockaddr_in their_addr;
       socklen_t addr_len = sizeof(their_addr);
       char router_buf[2048];   //we changed this in Stage 2 for reading data from router
       int router_port;
       //int getr;
       
       if(stage1){
       ofile.open("stage1.proxy.out");
       }
       if(stage2){
        ofile.open("stage2.proxy.out");
       }
       
       while(!bytes_rec){   // this waits for the I'm up message before calling tunnel_reader to receive/send packets from tun1
           int number = 1;
           bytes_rec = recvfrom(sockfd, router_buf, 2048, 0, (struct sockaddr *)&their_addr, &addr_len);
           
           router_port = ntohs(their_addr.sin_port);
           
           ofile << "proxy port: " << proxy_port << endl;
           ofile << "router: " << number << ", pid: " << router_buf << ", port: " << router_port << endl;
           number = number + 1;
       }
       
       /*
       cout << "1. Router's port number is: " << router_port << endl;

       cout << "1. Message from router (router's PID) is: " << router_buf << endl;
       */
        
        /*
       getr = getsockname(sockfd, (struct sockaddr *)&their_addr, &addr_len);
       
       if(getr == 0 && their_addr.sin_family == AF_INET && addr_len ==sizeof(their_addr)){
           router_port = ntohs(their_addr.sin_port);
       }
       */
       
       //router_port = ntohs(their_addr.sin_port); // getting port number of router
       
       /* end of Stage 1 */
       //cout << "1. End of Stage 1 of Onion Proxy" << endl;
       
       if(stage1){
           ofile.close();
       }
       
       else if(stage2){
       /* starting Stage 2 */
       //cout << "1. Stage 2 of Onion Proxy Begin" << endl;
       
       /*
       char oktesting[2048];
       int whatever = 123456;
       sprintf(oktesting, "%d",whatever);
       
       
       sendto(sockfd, oktesting, 2048, 0, (struct sockaddr *)&their_addr, addr_len);
       */
       
       
       //will need to change for multiple routers pass a reference to vector of sockaddr
           
           cout << "\nAfter sending an ICMP message, wait until 'end of program' is printed to terminal before reading the output files stage2.proxy.out and stage2.router1.out" << endl;
           cout << "\nProgram will exit if ICMP message is not sent within a 10 second window\n" << endl;
           tunnel_reader(sockfd, their_addr, &ofile);
       }
        //sleep(2); this is a "fake, cheap" way of doing waitpid. How do I wait for all children to finish?
        /*int stat;
        pid_t end;
       while(!my_deq.empty()){
           end = my_deq.front();
           cout << "1. PID's in deq " << end << endl;       //Maybe later we need this but since children exitng when parents exits its ok
           (void)waitpid(end, &stat, 0);
           my_deq.pop_front();
       }
         */
         //(void)waitpid(pid, &stat, 0);
       
       int stat;
       pid_t end;
       while(!my_deq.empty()){
           end = my_deq.front();
           //cout << "1. PID's in deq " << end << endl;       //Maybe later we need this but since children exitng when parents exits its ok
           (void)waitpid(end, &stat, 0);
           my_deq.pop_front();
       }
       
       
    }
    cout << "\nend of program" << endl;
    return EXIT_SUCCESS;
}
//helloaasdfasdfa1234