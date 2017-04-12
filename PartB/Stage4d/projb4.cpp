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


using namespace std; //include this right after headers


/**************************************************************************
 * tun_alloc: allocates or reconnects to a tun/tap device.
 * copy from from simpletun.c, used with permission of the author
 * refer to http://backreference.org/2010/03/26/tuntap-interface-tutorial/ for more info
 **************************************************************************/

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

/*
int tunnel_reader(int sockfd, struct sockaddr_in router_addr1, struct sockaddr_in router_addr2, struct sockaddr_in router_addr3, struct sockaddr_in router_addr4, struct sockaddr_in router_addr5, struct sockaddr_in router_addr6,ofstream *ofile) // sockfd of Proxy is well known
{
        return 1;
}
*/

int main(int argc, char** argv)
{
    ofstream ofile;
    deque<pid_t> my_deq;
    string str1;     //should make this cstring infinitely long maybe string
    string str2 = "stage 1";
    string str4 = "stage 2";
    string str5 = "stage 3";
    string str6 = "stage 4";
    int stage1 = 0;
    int stage2 = 0;
    char str3[15];
    
    int routernum = 1;  //keep this as one for now
    int num;    //should be removed and replaced with routernum
    int anothernum;
    pid_t pid;
    int proxy_port;
    //int one = 1;
    
    if(argc < 2){
        cout << "usage: ./proja config_file" << endl;
        return 1;
    }
    
    
    ifstream ifile (argv[1]);
    //getline(ifile,str1,'#');
    
    //added code to be able to read multiple comments
    getline(ifile,str1);
    char comment[2048];
    strcpy(comment,str1.c_str());
    while(comment[0] == '#'){
        cout << "read a comment" << endl;
        getline(ifile,str1);
        strcpy(comment,str1.c_str());
    }
    
    //added code to be able to read multiple comments
    
    
    /*
     cout << "\nThis is the comment of the config_file: \n" << str1 << endl;
     */
    
    //getline(ifile,str1);
    
    if( str1 == str2 ) {
       cout << "\nWe are in Stage 1 " << endl;
        stage1 = 1;
    }
    else if(str1 == str4){
        cout << "\nWe are in Stage2 " << endl;
        stage2 = 1;
    }
    else if(str1 == str5){
        cout << "\nWe are in Stage3 " << endl;
        stage2 = 1;
    }
    else if(str1 == str6){
        cout << "\nWe are in Stage4 " << endl;
        stage2 = 1;
    }
       getline(ifile,str1,' ');
        ifile.getline(str3,15);
       
    num = atoi(str3);        //replace num with routernum
    
    cout << "The number of routers is: " << num << "\n" << endl; //replace num with router num
    
    routernum = num; //added for more routers
    anothernum = num;
    
    ifile.close();
    if(num == 0){
        cout << "no routers " << endl;
        return 1;
    }
    
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
     
    pid = 1; //declaring we are parent;
    
    while(routernum){
        //forking methode copied from wikipedia
        
        if(pid){
            pid = fork(); // after this function is called both parent AND child return from this function to this point
            
            if (pid == -1) {
                // When fork() returns -1, an error happened.
                perror("fork failed");
                exit(EXIT_FAILURE);
            }
            
            if(pid){ //only parent does this
                routernum = routernum - 1;
                //if you're parent then push back the pid
                my_deq.push_back(pid);
                cout << "succesfully created child process" << endl;
            }
            else if(!pid){ //if you are a child you break from loop and continues
                goto childcontinues;
            }
        }
    }
    
    /*
    cout << "12. This is int routernum value: " << routernum << endl;
    */
    
    
childcontinues:
        if (pid == 0) {

        // When fork() returns 0, we are in the child process.
        printf("2. Hello from the child process!\n");
        cout << "my number is " << routernum << endl;
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
        char numberr[5];
        sprintf(numberr, "%d",routernum);
        strcat(msg, " ");
        strcat(msg, numberr);   //adds router number to msg
        
        /* Stage 2 variables */
        struct sockaddr_in their_addr;
        char buf[2048];
        socklen_t addr_len = sizeof(their_addr);
        int recv_bytes = 0;
        
        fd_set readfds;
        struct timeval tv;
        int rv, n;
        int rawsockbind;
        struct addrinfo *rawinfo;
        struct addrinfo rawhints;
        int statusraw;
        char IP_OF_ETH1[NI_MAXHOST];
        int rawsockfd;
        char nameit[10];
        if(routernum == 1){
            strcpy (nameit, "eth1");
        }
        else if(routernum == 2){
            strcpy (nameit, "eth2");
        }
        else if(routernum == 3){
            strcpy (nameit, "eth3");
        }
        else if(routernum == 4){
            strcpy (nameit, "eth4");
        }
        else if(routernum == 5){
            strcpy (nameit, "eth5");
        }
        else if(routernum == 6){
            strcpy (nameit, "eth6");
        }
            
            
            
        
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
        
            char filename[40];
        
        ofstream ofiler;
        if(stage1){
            char stg1[40];
            strcat(stg1, "stage1.router");
            char routernumber[5];
            sprintf(routernumber, "%d",routernum);
            strcat(stg1,routernumber);
            strcat(stg1,".out");
            cout << "making outfile" << endl;
            cout << stg1 << endl;
            strcat(filename, stg1);
            //ofiler.open(stage1);
        }
        else if(stage2){
            char stg2[40];
            strcat(stg2, "stage4.router");
            char routernumber[5];
            sprintf(routernumber, "%d",routernum);
            strcat(stg2,routernumber);
            strcat(stg2,".out");
            cout << "making outfile" << endl;
            cout << stg2 << endl;
            strcat(filename, stg2);
            //ofiler.open(stage2);
        }
            
        ofiler.open(filename);

        ofiler << "router: " << routernum << ", pid: " << child << ", port: " << dynamic_portr << "\n";
        
        ofiler.close();

            cout << "sent I'm up message" << endl;

            
        sendto(sockfdr, msg, 2048, 0, res->ai_addr, res->ai_addrlen);  // res is the local variable of the addrinfo res from Onion router but we send from our own sock "sockfdr"
        
            cout << "sent I'm up message" << endl;
            
        if(stage1){
        //one = one + 1;
        /*finished setting up dynamic UDP port for Onion Router */
        /* Sending "I'm up" message to Onion Proxy */

        /* Sending "I'm up" message to Onion Proxy */
        
        //cout << "2. End of Stage 1 of Router" << endl;
        goto stage1done;
        }
        /* starting Stage 2 */
        //cout << "2. Stage 2 of Router Begin" << endl;
        
        
        //PROJECT B code goes here

        
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
        

        //PROJECT B code goes here

        FD_ZERO(&readfds);
        
        //adding our descriptors to the set
        FD_SET(sockfdr, &readfds);
        FD_SET(rawsockfd, &readfds);
        
        if(sockfdr > rawsockfd){
            n = sockfdr + 1;
        }
        else if(sockfdr <= rawsockfd){
            n = rawsockfd + 1;
        }
        
        //timerouts = 15 sec plus 50,000 microseconds = 20.5 seconds
        tv.tv_sec = 600;
        tv.tv_usec = 500000;
        
        rv = select(n, &readfds, NULL, NULL, &tv);
        
        
        while(rv){  /* reads forever */
        
            if(FD_ISSET(sockfdr, &readfds)){   //If there is an input from socket that proxy sends to do this
                
                recv_bytes = recvfrom(sockfdr, buf, 2048, 0, (struct sockaddr *)&their_addr, &addr_len);
            
            
                if(recv_bytes){
                    //strcpy(msg, "ping back"); //TODO
                
                    //cout << "router got message" << endl;
                    struct ip * tun_ICMP;      //stores ICMP message in tun_ICMP
                    struct in_addr tun_src;  //test
                    struct in_addr tun_dest;
                    char tun_ip_src[NI_MAXHOST];
                    char tun_ip_dest[NI_MAXHOST];
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
                
                
                    string destnetwork = tun_ip_dest;
                    string ipofint = IP_OF_ETH1;
                    int a, b, c, d;
                    int e, f, g, h;
                
                    sscanf(destnetwork.c_str(), "%d.%d.%d.%d", &a, &b, &c, &d);
                
                    sscanf(ipofint.c_str(), "%d.%d.%d.%d", &e, &f, &g, &h);
                
                    if(((a == e) && (b == f) && (c == g))){   // open brackets this means that this ICMP message is directed to eth1's network
                
                        cout << "message is directed to router" << endl;
                        
                        
                        tun_ICMP->ip_src = tun_dest;
                        tun_ICMP->ip_dst = tun_src;
                
                
                        hlenl = tun_ICMP->ip_hl;
                        tun_rICMP = (struct icmp *)(buf + hlenl*4); // char = 8 so mult by 4
                        tun_utype = tun_rICMP->icmp_type;
                
                        ofiler.open(filename, std::fstream::app);
                        
                        ofiler << "ICMP from port: " << ntohs(their_addr.sin_port) << ", src: " << tun_ip_src << ", dst: " << tun_ip_dest << ", type: ";
                
                        if(tun_utype == 8){
                            //cout << "router says type is 8" << endl;
                            ofiler << "8\n";
                        }

                        ofiler.close();
                        
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
                    else if(!((a == e) && (b == f) && (c == g))){ // packet is not destined to eth1 network
                        
                        cout << "message is directed to MIT" << endl;
                        cout << "address of MIT is: " << tun_ip_dest << endl;
                        
                        
                        hlenl = tun_ICMP->ip_hl;
                        tun_rICMP = (struct icmp *)(buf + hlenl*4); // char = 8 so mult by 4
                        tun_utype = tun_rICMP->icmp_type;
                        
                        ofiler.open(filename, std::fstream::app);

                        
                        ofiler << "ICMP from port: " << ntohs(their_addr.sin_port) << ", src: " << tun_ip_src << ", dst: " << tun_ip_dest << ", type: ";
                        
                        if(tun_utype == 8){
                            //cout << "router says type is 8" << endl;
                            ofiler << "8\n";
                        }
                        
                        ofiler.close();
                        
                        //setting IP_HDRINCL
                        
                        int hdrincl=0;
                        if (setsockopt(rawsockfd,IPPROTO_IP,IP_HDRINCL,&hdrincl,sizeof(hdrincl))==-1) {
                            cout << "error in setsockopt" << endl;
                        }
                        
                        
                        /*
                        char interface[] = "eth1";
                        find_ipaddr(interface,NULL,&eth1);
                        tun_ICMP->ip_src = eth1.sin_addr;
                        */
                        /*
                        struct icmphdr req;
                        req.type=8;
                        req.code=0;
                        req.checksum=0;
                        req.un.echo.id=htons(0x1234);
                        req.un.echo.sequence=htons(1);
                        req.checksum=ip_checksum(&req,8);
                        */
                        /*
                        struct icmphdr *req1;
                        req1 = (struct icmphdr *)(buf + hlenl*4);
                        req1->checksum =0;
                        
                        cout << req1->code << endl;
                        cout << req1->checksum << endl;
                        cout << req1->un.echo.id << endl;
                        cout << req1->un.echo.sequence << endl;
                        req1->checksum=ip_checksum(req1,8);
                        
                        struct icmphdr req;
                        req.type=req1->type;
                        req.code=req1->code;
                        req.checksum=0;
                        req.un.echo.id=req1->un.echo.id;
                        req.un.echo.sequence=req1->un.echo.sequence;
                        req.checksum=ip_checksum(&req,8);
                        
                        cout << "icmp sent has type ";
                        
                        if(tun_rICMP->icmp_type == 8)
                        cout << "8" << endl;
                        */
                        
                        
                        /*
                        tun_rICMP->icmp_cksum = 0;
                        tun_rICMP->icmp_cksum = ip_checksum(tun_rICMP,28);
                        */
                        
                        
                        //write to raw socket bTODO
                        struct iovec iov[1];
                        iov[0].iov_base= tun_rICMP; //message is the ICMP header + payload (tun_rICMP or icmp)
                        iov[0].iov_len = 64; // *4 because of char buffer? well no, iov_len is measured in bytes and a char is a byte
                        
                        struct addrinfo *ipdest2;
                        struct addrinfo ipdest1;
                        
                        memset(&ipdest1, 0, sizeof ipdest1); // make sure the struct is empty
                        
                        ipdest1.ai_family = AF_INET; // use IPv4
                        ipdest1.ai_socktype = SOCK_RAW; // RAW stream sockets
                        
                        
                        
                        statusr = getaddrinfo(tun_ip_dest, NULL, &ipdest1, &ipdest2);
                        if (statusr != 0){
                            fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(statusr));
                            return 1;
                        }
                        
                        
                        
                        struct sockaddr_in *eth1 = (struct sockaddr_in *) rawinfo->ai_addr;
                        struct sockaddr_in rawout;
                        rawout = *eth1; //currently we are trying test send this message back to our own raw socket and it works! let's see what we sent below
                        
                        
                        
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
                }
                
            }
            if(FD_ISSET(rawsockfd, &readfds)){ //if rawsockfd has something to read from

                cout << "packet arrived at raw socket" << endl;

                //bTODO below one liner!
                
                char recbuf[2048];
                struct iovec iovr[1];
                iovr[0].iov_base= recbuf; //message is the ICMP header + payload
                iovr[0].iov_len = sizeof(recbuf);
                
                struct msghdr msgh;
                memset(&msgh,0, sizeof(msgh));
                msgh.msg_iov=iovr;
                msgh.msg_iovlen=1;
                msgh.msg_control=0;
                msgh.msg_controllen=0;
                
                recv_bytes = recvmsg(rawsockfd,&msgh,0); //bTODO this is all I have to understand! we also need to add a IP header?
                
                
                if(recv_bytes){
                    cout << "router got message" << endl;
                    
                    //struct sockaddr_in *recvaddr;
                    struct ip * tun_ICMP;      //stores ICMP message in tun_ICMP
                    struct in_addr tun_src;  //test
                    struct in_addr tun_dest;
                    struct sockaddr_in eth0;
                    char tun_ip_src[NI_MAXHOST];
                    char tun_ip_dest[NI_MAXHOST];
                    struct icmp * tun_rICMP;
                    u_char tun_utype =0;
                    int hlenl;
                    
                    /*/this is testing to see if i received ONLY an ICMP message
                    
                    tun_rICMP = (struct icmp *) recbuf;
                    tun_utype = tun_rICMP->icmp_type;
                    
                    cout << "THIS IS THE TYPE :" << (int)tun_utype << endl;
                    
                    if(tun_utype == 8){
                        
                        cout << " THE REAL TYPE IS 8" << endl;
                    }
                    if(tun_utype == 0){
                        cout << " THE REAL TYPE IS 0" << endl;
                    }
                    
                    
                    this is testing to see if i received ONLY an ICMP message
                    */
                     
                    tun_ICMP = (struct ip *)recbuf;
                    tun_src = tun_ICMP->ip_src;
                    inet_ntop(AF_INET, &tun_src, tun_ip_src, sizeof(tun_ip_src));
                
                    tun_dest = tun_ICMP->ip_dst;
                    inet_ntop(AF_INET, &tun_dest, tun_ip_dest, sizeof(tun_ip_dest));
                    
                     cout << "router says source is :" << tun_ip_src << endl;
                     cout << "router says destination is: " << tun_ip_dest << endl;
                    
                    string destnetwork = tun_ip_dest;
                    string ipofint = IP_OF_ETH1;
                    int a, b, c, d;
                    int e, f, g, h;
                    
                    sscanf(destnetwork.c_str(), "%d.%d.%d.%d", &a, &b, &c, &d);
                    
                    sscanf(ipofint.c_str(), "%d.%d.%d.%d", &e, &f, &g, &h);
                    
                    if(((a == e) && (b == f) && (c == g))){
                    
                        cout << "packet is destined to router" << endl;
                        
                        hlenl = tun_ICMP->ip_hl;
                        tun_rICMP = (struct icmp *)(recbuf + hlenl*4); // char = 8 so mult by 4
                        tun_utype = tun_rICMP->icmp_type;
                    
                        cout << "this is the icmp type";
                    
                        //tun_utype = 0;
                        ofiler.open(filename, std::fstream::app);

                        ofiler << "ICMP from raw sock, src: " << tun_ip_src << ", dst: " << tun_ip_dest << ", type: ";
                        
                        if(tun_utype == 8){
                        
                            cout << " 8" << endl;
                        }
                        if(tun_utype == 0){
                            cout << " 0" << endl;
                            
                            ofiler << "0\n";
                        }
                    
                        ofiler.close();
                        

                    
                        
                    
                        /*
                        recvaddr = (struct sockaddr_in *) msgh.msg_name;
                        tun_src = recvaddr->sin_addr;
                        inet_ntop(AF_INET, &tun_src, tun_ip_src, sizeof(tun_ip_src));
                    
                        cout << "router says source is :" << tun_ip_src << endl;
                    
                        */
                    
                        //skip for now
                        //TODOb to create an IP header
                        memset(&eth0, 0, sizeof eth0);
                        char interface[] = "eth0";
                        char abs[NI_MAXHOST];
                        find_ipaddr(interface,abs);
                        cout << "this is eth0's IP: " << abs << endl;
                        inet_pton(AF_INET, abs, &(eth0.sin_addr));

                        tun_ICMP->ip_dst = eth0.sin_addr; // only change destination to eth0, keep source the same (eth0.sin_addr)
                        
                        //tun_ICMP->ip_src = tun_dest;
                        //tun_ICMP->ip_dst = tun_src;
                        
                        
                        //tun_rICMP->icmp_type = 0;
                        
                        /*
                        tun_rICMP->icmp_cksum = 0;
                        tun_rICMP->icmp_cksum = ip_checksum(tun_rICMP,28);
                        */
                        
                        tun_ICMP->ip_sum = 0;
                        tun_ICMP->ip_sum = ip_checksum(tun_ICMP,20);
                        
                        /*
                         tun_rICMP->icmp_type = 0;
                         tun_utype = tun_rICMP->icmp_type;
                
                        if(tun_utype == 0){
                            //cout << "router sends type as 0" << endl;
                        }
                        strcpy(buf,(char *)tun_ICMP);
                        strcat(buf,(char *)tun_rICMP);
                        */
                        sendto(sockfdr, recbuf, sizeof(recbuf), 0, res->ai_addr, res->ai_addrlen); //send message back to proxy
                    }
                    recv_bytes = 0;
                    
                    
                }
                
            }
            FD_ZERO(&readfds);
            
            //adding our descriptors to the set
            FD_SET(sockfdr, &readfds);
            FD_SET(rawsockfd, &readfds);
            
            if(sockfdr > rawsockfd){
                n = sockfdr + 1;
            }
            else if(sockfdr <= rawsockfd){
                n = rawsockfd + 1;
            }
            
            //timerouts = 15 sec plus 50,000 microseconds = 20.5 seconds
            tv.tv_sec = 600;
            tv.tv_usec = 500000;
            
            rv = select(n, &readfds, NULL, NULL, &tv);
            
        }//end of while
        
        stage1done:
        //ofiler.close();         //
        _exit(EXIT_SUCCESS);  // exit() is unreliable here, so _exit must be used
    }
   else if(pid) {
       
       // When fork() returns a positive number, we are in the parent process
       // and the return value is the PID of the newly created child process.
       printf("Hello from the parent process!\n");
       
       
       int bytes_rec = 1;
       struct sockaddr_in their_addr;   //create individual their_addr for each router
       socklen_t addr_len = sizeof(their_addr);
       
       struct sockaddr_in their_addr1;   //create individual their_addr for each router
       //socklen_t addr_len1 = sizeof(their_addr1);
       struct sockaddr_in their_addr2;   //create individual their_addr for each router
       //socklen_t addr_len2 = sizeof(their_addr2);
       struct sockaddr_in their_addr3;   //create individual their_addr for each router
       //socklen_t addr_len3 = sizeof(their_addr3);
       struct sockaddr_in their_addr4;   //create individual their_addr for each router
       //socklen_t addr_len4 = sizeof(their_addr4);
       struct sockaddr_in their_addr5;   //create individual their_addr for each router
       //socklen_t addr_len5 = sizeof(their_addr5);
       struct sockaddr_in their_addr6;   //create individual their_addr for each router
       //socklen_t addr_len6 = sizeof(their_addr6);
       
       char router_buf[2048];   //we changed this in Stage 2 for reading data from router
       int router_port;
       //int getr;
       char tun_name[IFNAMSIZ];
       char tun_buf[2048];
       //char router_buf[2048];
       int tun_fd, n; //choosing between tunnel interface or socket interface
       fd_set readfds;
       struct timeval tv;
       int rv;
       
       //struct sockaddr_in router_addr;
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
       
       //struct sockaddr_in their_addr;        //stores origns of address from router msg
       //socklen_t addr_len = sizeof(their_addr);
       
       if(stage1){
       ofile.open("stage1.proxy.out");
       }
       if(stage2){
        ofile.open("stage4.proxy.out");
       }
       
       cout << "Parent checkpoint" << endl;
       
       ofile << "proxy port: " << proxy_port << endl;
       
       while((bytes_rec) && num){   //  this waits for the I'm up message before calling tunnel_reader to receive/send packets from tun1
           //int number = 1;
           
           cout << "entered while loop" << endl;
           
           //char pidnum[10];
           char *routerid;
           char *space;
           
           //pidnum = strtok(router_buf," ");
           //routerid = strtok(NULL," ");
           

           
           bytes_rec = recvfrom(sockfd, router_buf, 2048, 0, (struct sockaddr *)&their_addr, &addr_len);
           
           if(bytes_rec < 0){
               cout << "error in router up message" << endl;
           }
           

           
           router_port = ntohs(their_addr.sin_port);
           
           cout << "router's msg: " << router_buf <<  endl;
           space = strchr(router_buf,' ');
           *space = '\0';
           space = space + 1;
           routerid = space;
           
           if(strcmp(routerid,"1") == 0){
               cout << "router 1 YOOHHOOO" << endl;
               their_addr1 = their_addr;
               //addr_len1 = addr_len;
           }
           else if(strcmp(routerid,"2") == 0){
               cout << "router 2 YOOHHOOO" << endl;
               their_addr2 = their_addr;
               //addr_len2 = addr_len;
           }
           else if(strcmp(routerid,"3") == 0){
               cout << "router 3 YOOHHOOO" << endl;
               their_addr3 = their_addr;
               //addr_len3 = addr_len;
           }
           else if(strcmp(routerid,"4") == 0){
               cout << "router 4 YOOHHOOO" << endl;
               their_addr4 = their_addr;
               //addr_len4 = addr_len;
           }
           else if(strcmp(routerid,"5") == 0){
               cout << "router 5 YOOHHOOO" << endl;
               their_addr5 = their_addr;
               //addr_len5 = addr_len;
           }
           else if(strcmp(routerid,"6") == 0){
               cout << "router 6 YOOHHOOO" << endl;
               their_addr6 = their_addr;
               //addr_len6 = addr_len;
           }
           
           
           
           cout << "router pid: " << router_buf << endl;
           cout << "router number: " << routerid << endl;
           
           ofile << "router: " << routerid << ", pid: " << router_buf << ", port: " << router_port << endl;
           
           num = num - 1;
           //number = number + 1;
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
           ofile.close();
       /*
       char oktesting[2048];
       int whatever = 123456;
       sprintf(oktesting, "%d",whatever);
       
       
       sendto(sockfd, oktesting, 2048, 0, (struct sockaddr *)&their_addr, addr_len);
       */
       
       
       //will need to change for multiple routers pass a reference to vector of sockaddr
           
           cout << "\nAfter sending an ICMP message, wait until 'end of program' is printed to terminal before reading the output files stage2.proxy.out and stage2.router1.out" << endl;
           cout << "\nProgram will exit if ICMP message is not sent within a 10 second window\n" << endl;
           
           
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
           tv.tv_sec = 600;
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
                   else// if(nread == 84) //cheat method
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
                       ofile.open("stage4.proxy.out",std::fstream::app);

                       ofile << "ICMP from tunnel, src: " << tun_ip_src << ", dst: " << tun_ip_dest << ", type is: ";
                       
                       if(tun_utype == 8){
                           ofile << "8\n";
                       }
                       ofile.close();
                       
                       string destnetwork = tun_ip_dest;
                       
                       string net1 = "192.168.201.2";
                       string net2 = "192.168.202.2";
                       string net3 = "192.168.203.2";
                       string net4 = "192.168.204.2";
                       string net5 = "192.168.205.2";
                       string net6 = "192.168.206.2";
                       
                       int a, b, c, d;
                       int e, f, g, h;
                       sscanf(destnetwork.c_str(), "%d.%d.%d.%d", &a, &b, &c, &d);
                       
                       int set = 1;
                       int direction;
                       uint32_t hostdestip = ntohl(tun_dest.s_addr);
                       direction = (hostdestip % anothernum);
                       direction = direction + 1;
                       
                       sscanf(net1.c_str(), "%d.%d.%d.%d", &e, &f, &g, &h);
                       if((a == e) && (b == f) && (c == g) && (anothernum > 0)){
                           sendto(sockfd, tun_buf, 2048, 0, (struct sockaddr *)&their_addr1, sizeof(their_addr1));
                           set = 0;
                       }
                       sscanf(net2.c_str(), "%d.%d.%d.%d", &e, &f, &g, &h);
                       if((a == e) && (b == f) && (c == g) && (anothernum > 1)){
                           sendto(sockfd, tun_buf, 2048, 0, (struct sockaddr *)&their_addr2, sizeof(their_addr2));
                           set = 0;
                       }
                       sscanf(net3.c_str(), "%d.%d.%d.%d", &e, &f, &g, &h);
                       if((a == e) && (b == f) && (c == g) && (anothernum > 2)){
                           sendto(sockfd, tun_buf, 2048, 0, (struct sockaddr *)&their_addr3, sizeof(their_addr3));
                           set = 0;
                       }
                       sscanf(net4.c_str(), "%d.%d.%d.%d", &e, &f, &g, &h);
                       if((a == e) && (b == f) && (c == g) && (anothernum > 3)){
                           sendto(sockfd, tun_buf, 2048, 0, (struct sockaddr *)&their_addr4, sizeof(their_addr4));
                           set = 0;
                       }
                       sscanf(net5.c_str(), "%d.%d.%d.%d", &e, &f, &g, &h);
                       if((a == e) && (b == f) && (c == g) && (anothernum > 4)){
                           sendto(sockfd, tun_buf, 2048, 0, (struct sockaddr *)&their_addr5, sizeof(their_addr5));
                           set = 0;
                       }
                       sscanf(net6.c_str(), "%d.%d.%d.%d", &e, &f, &g, &h);
                       if((a == e) && (b == f) && (c == g) && (anothernum > 5)){
                           sendto(sockfd, tun_buf, 2048, 0, (struct sockaddr *)&their_addr6, sizeof(their_addr6));
                           set = 0;
                       }
                       if((direction == 1) && set){
                          sendto(sockfd, tun_buf, 2048, 0, (struct sockaddr *)&their_addr1, sizeof(their_addr1));
                       }
                       if((direction == 2) && set){
                           sendto(sockfd, tun_buf, 2048, 0, (struct sockaddr *)&their_addr2, sizeof(their_addr2));
                       }
                       if((direction == 3) && set){
                           sendto(sockfd, tun_buf, 2048, 0, (struct sockaddr *)&their_addr3, sizeof(their_addr3));
                       }
                       if((direction == 4) && set){
                           sendto(sockfd, tun_buf, 2048, 0, (struct sockaddr *)&their_addr4, sizeof(their_addr4));
                       }
                       if((direction == 5) && set){
                           sendto(sockfd, tun_buf, 2048, 0, (struct sockaddr *)&their_addr5, sizeof(their_addr5));
                       }
                       if((direction == 6) && set){
                           sendto(sockfd, tun_buf, 2048, 0, (struct sockaddr *)&their_addr6, sizeof(their_addr6));
                       }
                       
                       
                       
                       
                           //TODO if more than one router we need to send to ALL (use for loop)
                           //sendto(sockfd, tun_buf, 2048, 0, (struct sockaddr *)&router_addr, sizeof(router_addr));
                           
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
                       
                       cout << "Proxy received a packet" << endl;
                       
                       router_ICMP = (struct ip *)router_buf;
                       router_src = router_ICMP->ip_src;
                       inet_ntop(AF_INET, &router_src, router_ip_src, sizeof(router_ip_src));
                       
                       router_dest = router_ICMP->ip_dst;
                       inet_ntop(AF_INET, &router_dest, router_ip_dest, sizeof(router_ip_dest));
                       
                       
                       hlenlr = router_ICMP->ip_hl;
                       router_rICMP = (struct icmp *)(router_buf + hlenlr*4);
                       //router_rICMP = router_rICMP + 0x00000001
                       router_utype = router_rICMP->icmp_type;
                       
                       /*
                        cout << "source is router :" << router_ip_src << endl;
                        cout << "dest is tun1 :" << router_ip_dest << endl;
                        cout << "type is: " << router_utype << endl;
                        */
                       
                       ofile.open("stage4.proxy.out",std::fstream::app);
                       
                       ofile << "ICMP from port: " << ntohs(their_addr.sin_port) << ", src: " << router_ip_src << ", dst: " << router_ip_dest << ", type is: ";
                       
                       if(router_utype == 0){
                           ofile << "0\n";
                           cout << "SOMEHOW IT WORKED AND SENT TYPE 0!" << endl;
                       }
                       
                       ofile.close();
                       
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
                   tv.tv_sec = 600;
                   tv.tv_usec = 500000;
                   
                   rv = select(n, &readfds, NULL, NULL, &tv);
                   //we get new values for rv
                   
                   
               }
               
               if (rv == -1) {
                   perror("select"); // error occured in select()
               }
               //cout << "end of tun_read()" << endl;
              // ofile.close();

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
           cout << "ending parent" << endl;
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