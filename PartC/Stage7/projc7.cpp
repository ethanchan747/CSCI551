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
 ***********************************************************************************************************
 ProjA completed
 Errors to fix in Project A: We need to be able to filter multiple comment lines say #dasdf and #asdfas
 Also we need to be able to create mupltiple routers now
 Error was using writefd as my variable but instead put it in the readfds for select(readfds,NULL,NULL...)
 
 ******************************************************************************************************
 ProjB NOTE: WE NEED TO DO "sudo ./projb config_file inorder to use Raw sockets
 
 
 
 Yes we create two sockets for the router because we need the dynamic port as raw sockets don't have dynamic port numbers! Send one for UDP up and then for communicating with proxy.
 
 is declaring the port destination necessary for raw sockets?
 
 
 
 10/19/2014
    8:15am
        eth0
        link/ether 08:00:27:d4:21:f9 brd ff:ff:ff:ff:ff:ff
        inet 10.0.2.15
        inet6 fe80::a00:27ff:fed4:21f9

    8:30am
        eth0
        link/ether 08:00:27:d4:21:f9 brd ff:ff:ff:ff:ff:ff
        inet 10.0.2.15
        inet6 fe80::a00:27ff:fed4:21f9
 
 
 Perhaps I can include the settup up traffic lines the Makefile?rm 
 Replace everything that has "if stage 2" with "if stage 3"
 and replace everything that has "stage2.router1.out" and everything with "stage2.proxy.out"
 
 if(routernum){
 //forking methode copied from wikipedia
 pid = fork(); // after this function is called both parent AND child return from this function to this point
 routernum = routernum - 1;
 if(pid){ //if you're parent then push back the pid
 my_deq.push_back(pid);
 }
 }
 search this section for creating multiple routers
 
 creating a raw socket
 int fd4;
 fd4 = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); for IPV4
 
 
 retreiving network interfaces
 http://www.linuxhowtos.org/manpages/3/getifaddrs.htm
 
 
 What I know: I can successfully send a packet from my raw socket, my raw socket is successfully bound to
 Bind UDP socket and RAW sockets to same IP address
 Make my own checksum code.
 
 cout << endl << "Here is the information for the IP header" << endl;
 cout << "ip _hl: " << tun_ICMP->ip_hl << endl;
 cout << "ip_v: " << tun_ICMP->ip_v << endl;
 cout << "ip_tos: " << tun_ICMP->ip_tos << endl;
 cout << "ip_len: " << tun_ICMP->ip_len << endl;
 cout << "ip_id: " << tun_ICMP->ip_id << endl;
 cout << "ip_off: " << tun_ICMP->ip_off << endl;
 cout << "ip_ttl: " << tun_ICMP->ip_ttl << endl;
 cout << "ip_p: " << tun_ICMP->ip_p << endl;
 cout << "ip_sum: " << tun_ICMP->ip_sum << endl;
 
 cout << endl << "Here is the information for the IP header" << endl;
 cout << "icmp_type: " << tun_rICMP->icmp_type << endl;
 cout << "icmp_code: " << tun_rICMP->icmp_code << endl;
 cout << "icmp_cksum: " << tun_rICMP->icmp_cksum << endl;
 cout << "icmp_hun.ih_pptr: " << tun_rICMP->icmp_hun.ih_pptr << endl;
 cout << "icmp_hun.ih_gwaddr.s_addr: " << tun_rICMP->icmp_hun.ih_gwaddr.s_addr << endl;
 cout << "icmp_hun.ih_idseq.icd_id: " << tun_rICMP->icmp_hun.ih_idseq.icd_id << endl;
 cout << "icmp_hun.ih_idseq.icd_seq: " << tun_rICMP->icmp_hun.ih_idseq.icd_seq << endl;
 cout << "icmp_hun.ih_void: " << tun_rICMP->icmp_hun.ih_void << endl;
 cout << "icmp_dun.id_ts.its_otime: " << tun_rICMP->icmp_dun.id_ts.its_otime << endl;
 cout << "icmp_dun.id_ts.its_rtime: " << tun_rICMP->icmp_dun.id_ts.its_rtime << endl;
 cout << "icmp_dun.id_ts.its_ttime: " << tun_rICMP->icmp_dun.id_ts.its_ttime << endl;
 //cout << "icmp_dun.id_ip.idi_ip: " << tun_rICMP->icmp_dun.id_ip.idi_ip << endl;
 cout << "icmp_dun.id_mask: " << tun_rICMP->icmp_dun.id_mask << endl;
 cout << "icmp_dun.id_data: " << tun_rICMP->icmp_dun.id_data << endl;
 **************************************************************************************************
 Stage3 done: NOTE when it intially runs for the first time though one packet is loss, why?
 
 Stage4
 When we read from IP header, change the source from network to host byte order before taking the mod
 
 Stage 5
 make sure IP addreses of routers are printed correctly
 Make sure number of hops are printed out correctly
Email TA/Prof about stage5.sampleoutpu2
 – #include <cstdlib> make sure in final code have most up to date headers
 be careful of network byte order
 
 
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
#include <cstdlib>
#include <vector>
#include <map>
#include <iomanip>
#include "aes-test.h"

using namespace std; //include this right after headers


/**************************************************************************
 * tun_alloc: allocates or reconnects to a tun/tap device.
 * copy from from simpletun.c, used with permission of the author
 * refer to http://backreference.org/2010/03/26/tuntap-interface-tutorial/ for more info
 **************************************************************************/

struct pseudo_tcphdr
{
    struct in_addr source, dest;
    u_char reserve;
    u_char protocol;
    u_short length;
};

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

u_short tcp_checksum(struct tcphdr * tcp_hdr, int tcp_length, struct ip * ip_info){ // returns answer but also inputs answer
    u_short original = tcp_hdr->check;
    
    tcp_hdr->check = 0;
    struct pseudo_tcphdr tcppseudo;
    memset(&tcppseudo, 0, sizeof(tcppseudo));
    tcppseudo.source = ip_info->ip_src; //tun_src;
    tcppseudo.dest = ip_info->ip_dst; //tun_dest;
    tcppseudo.protocol = IPPROTO_TCP;
    tcppseudo.length = htons(tcp_length); //ip_len is in bytes, hlen is in 32 bits
    char pseud_and_hdr[2048];
    memset(&pseud_and_hdr,0,2048);
    memcpy(pseud_and_hdr,&tcppseudo,sizeof(tcppseudo));
    memcpy(pseud_and_hdr+sizeof(tcppseudo),tcp_hdr, tcp_length); // cpy pseudo header and all of tcp into a buffer.
    //tun_ICMP->ip_len - hlenl*4
    int total_length = tcp_length + sizeof(tcppseudo);
    cout << endl << endl << "sent out raw1g" << endl << endl;
    //tcp_hdr->check = 0;
    cout << endl << endl << "sent out raw1h" << endl << endl;
    tcp_hdr->check = ip_checksum(pseud_and_hdr,total_length);
    //req.checksum=ip_checksum(&req,8);
    cout << endl << endl << "sent out raw2" << endl << endl;
    u_short generated = tcp_hdr->check;
    
    cout << endl << endl << "This is original checksum is: " << original << endl << endl;
    cout << endl << endl << "This is generated checksum is: " << generated << endl << endl;
    
    return generated;
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

int key_to_hex_buf(unsigned char* key, char* buf, int len)
{
    int index = 0;
    for(int i=0; i< len ;i++)
    {
        index += sprintf(buf+index, "%02x", key[i]);
    }
    index += sprintf(buf+index, "\n");
    return index;
}


int main(int argc, char** argv)
{
    
    uint16_t proxyid = 0x0001;
    //generate's random key
    //uint8_t key[16];
    //memset(key, 0,16);
    uint16_t uasdf = 0x0051;
    cout << "this is a testing hex" << hex << uasdf << endl;
    unsigned char aes_key[16];
    memset(aes_key, 0,16);
    srand ( time(NULL) );
    cout <<"random numbers" << endl;
    for(int i=0; i< 16; i++)
    {
        //aes_key[i] = 0xf1;
        aes_key[i] = rand() % 256;
        cout << (int)aes_key[i] << " ";
        //cout << setw(2) << setfill('0') << hex << aes_key[i];

    }
    cout << endl;
    //memset(aes_key, 0, 16);
    //memcpy(aes_key, key, 16);
    unsigned char rk1[16];
    memset(rk1, 0,16);
    for(int i=0; i< 16; i++)
    {
        rk1[i]=aes_key[i] ^ ((unsigned char)0x01);
    }
    unsigned char rk2[16];
    memset(rk2, 0,16);
    for(int i=0; i< 16; i++)
    {
        rk2[i]=aes_key[i] ^ ((unsigned char)0x02);
    }
    unsigned char rk3[16];
    memset(rk3, 0,16);
    for(int i=0; i< 16; i++)
    {
        rk3[i]=aes_key[i] ^ ((unsigned char)0x03);
    }
    unsigned char rk4[16];
    memset(rk4, 0,16);
    for(int i=0; i< 16; i++)
    {
        rk4[i]=aes_key[i] ^ ((unsigned char)0x04);
    }
    unsigned char rk5[16];
    memset(rk5, 0,16);
    for(int i=0; i< 16; i++)
    {
        rk5[i]=aes_key[i] ^ ((unsigned char)0x05);
    }
    unsigned char rk6[16];
    memset(rk6, 0,16);
    for(int i=0; i< 16; i++)
    {
        rk6[i]=aes_key[i] ^ ((unsigned char)0x06);
    }
    deque<unsigned char *> keyorder;
    keyorder.push_back(rk1);
    keyorder.push_back(rk2);
    keyorder.push_back(rk3);
    keyorder.push_back(rk4);
    keyorder.push_back(rk5);
    keyorder.push_back(rk6);
        
    ofstream ofile;
    deque<pid_t> my_deq;
    deque<int> next_router;
    string str1;     //should make this cstring infinitely long maybe string
    string str2 = "stage 1";
    string str4 = "stage 2";
    string str5 = "stage 3";
    string str6 = "stage 4";
    int stage1 = 0;
    int stage2 = 0;
    char str3[15];
    string str7;
    char str8[15];
    
    int routernum = 1;  //keep this as one for now
    int num;    //should be removed and replaced with routernum
    //int anothernum;
    pid_t pid;
    int proxy_port;
    int hops;
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
    
    getline(ifile,str7,' '); // get minitor_hops
    ifile.getline(str8,15); // get number of hops
    
    hops = atoi(str8); //hops number
    
    
    cout << "The number of routers is: " << num << "\n" << endl; //replace num with router num
    
    cout << "The number of hops is: " << hops << endl;
    
    routernum = num; //added for more routers
    //anothernum = num;
    int routnum10 = num;
    
    if( routnum10 <6){
        for(int bf = 6 - routnum10; bf > 0; bf--){
            keyorder.pop_back();
        }
    }
    
    ifile.close();
    if(num == 0 || hops == 0){
        cout << "no routers or no hops" << endl;
        return 1;
    }
    
    for(int t = 0; t < num; t++){ // assignes numbers 1 to num and creates keys
        next_router.push_back(t+1);
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
        int rawsockbind, rawtcpbind;
        struct addrinfo *rawinfo;
        struct addrinfo rawhints;
        int statusraw;
        char IP_OF_ETH1[NI_MAXHOST];
        int rawsockfd, rawtcp;
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
            strcat(stg2, "stage5.router");
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

            ofiler << "router: " << routernum << ", pid: " << child << ", port: " << dynamic_portr;
        
        ofiler.close();

            cout << "sent I'm up message" << endl;

            
        sendto(sockfdr, msg, 2048, 0, res->ai_addr, res->ai_addrlen);  // res is the local variable of the addrinfo res from Onion router but we send from our own sock "sockfdr"
        
            cout << "sent I'm up message" << endl;
            
            

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
            //Stage 7
            rawtcp = socket(AF_INET,SOCK_RAW,IPPROTO_TCP); //created raw socket IP header created for us
            
            if(rawtcp == -1){
                perror("create raw error");
                cout << "raw socket" << endl;
                return 1;
            }
            rawtcpbind = bind(rawtcp, rawinfo->ai_addr,rawinfo->ai_addrlen); //binding raw socket
            
            if(rawtcpbind == -1){
                perror("bind error");
                cout << "raw socket" << endl;
                return 1;
            }
            //Stage 7
            
            char myIP[NI_MAXHOST];
            struct in_addr printa;
            struct sockaddr_in *octo;
            octo = (struct sockaddr_in *)rawinfo->ai_addr;
            printa = octo->sin_addr;
            
            
            inet_ntop(AF_INET, &printa, myIP,sizeof(myIP));
            ofiler.open(filename,std::fstream::app);
            ofiler << ", IP: " << myIP << endl;
            /*
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
            */
            ofiler.close();
            
            
            //PROJECT B code goes here
            
            
            struct addrinfo h;
            struct addrinfo *info;
            uint16_t myid;
            unsigned char mykey[16];
            map<uint16_t,uint16_t> cidnext; // key = my circuit id, data = next port number
            map<uint16_t,uint16_t> cidreturn; // key = my circuit id, data = port I should relay back to
            map<uint16_t,uint16_t> cidreturnid; // key = my circuit id, data = circuit id of who I should relay back to
            //int flag1 =1;
            map<uint16_t,sockaddr_in> ipreturn; // key = my circuit id, data = traceback of sockaddr_in of IP of local host and port number of said router
            map<uint16_t,char*> ipreturnchar; // key = my circuit id, data = raw socket IP address


            
            
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
        
        
        //stage5 set up circuit

        FD_ZERO(&readfds);
        
        //adding our descriptors to the set
        FD_SET(sockfdr, &readfds);
        FD_SET(rawsockfd, &readfds);
        FD_SET(rawtcp, &readfds);

        //Stage 7
        if(sockfdr >= rawsockfd){
            if(sockfdr >= rawtcp){
                n = sockfdr + 1;
            }
            else if(rawtcp > sockfdr){
                n = rawtcp + 1;
            }
        }
        else if(rawsockfd > sockfdr){
            if(rawsockfd >= rawtcp){
                n = rawsockfd + 1;
            }
            else if(rawtcp > rawsockfd){
                n = rawtcp + 1;
            }
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

                    struct ip * IP;
                    struct in_addr src;
                    struct in_addr dest;
                    char IP_src[NI_MAXHOST];
                    char IP_dest[NI_MAXHOST];
                    
                    
                    IP = (struct ip *)buf;
                    src = IP->ip_src;
                    inet_ntop(AF_INET, &src, IP_src, sizeof(IP_src));
                    
                    dest = IP->ip_dst;
                    inet_ntop(AF_INET, &dest, IP_dest, sizeof(IP_dest));
                    
                    /*
                     cout << "router says source is :" << tun_ip_src << endl;
                     cout << "router says destination is :" << tun_ip_dest << endl;
                     */
                    
                    //if we recieve a minitor IP headermessage
                    if((strcmp(IP_src,"127.0.0.1") == 0) && (strcmp(IP_dest, "127.0.0.1") == 0) && (IP->ip_p == 253)){
                        int length = recv_bytes - sizeof(struct ip);
                        
                        ofile.open(filename,std::fstream::app);
                        ofile << "pkt from port: " << dec << ntohs(their_addr.sin_port) << ", length: " << length << ", contents: 0x";
                    
                        char minitorbuf[2048];
                        memset(minitorbuf, 0, sizeof(minitorbuf));
                        memcpy(minitorbuf, buf+sizeof(struct ip), length);
                        /* WRONG SEE BELOW
                        for(int i = 0; i < length; i++){
                            ofile << setw(2) << setfill('0') << hex << int(minitorbuf[i]);  //maybe in network order
                        }
                        ofile << endl;
                        ofile.close();
                        */
                        /* MUST NEED FOR EVERYTHING */
                        for(int i = 0; i < length; i++){
                            if(int(minitorbuf[i]) && 0x00008){
                                int getit = int(minitorbuf[i]);
                                getit = getit & 0x000000ff;
                                ofile << setw(2) << setfill('0') << hex << getit;
                            }
                            else if(!(int(minitorbuf[i]) && 0x00008)){
                                ofile << setw(2) << setfill('0') << hex << int(minitorbuf[i]);  //maybe in network order
                            }
                        }
                        
                        ofile << endl;
                        ofile.close();
                        
                        
                        uint16_t cid1 = (uint16_t)minitorbuf[1]; //upper
                        uint16_t cid2 = (uint16_t)minitorbuf[2]; //lower
                        cout << cid1 << endl;
                        cout << cid2 << endl;
                        cid1 = cid1 & 0x00ff;   //chars are only bytes so make sure
                        cid2 = cid2 & 0x00ff;
                            
                        cid1 = cid1*256;
                        cid1 = cid1 + cid2;
                            
                        uint16_t host = cid1; //convert from network to host basically switch lower with upper
                        
                        if(minitorbuf[0] == 0x65){
                            map<uint16_t,uint16_t>::iterator it;
                            it = cidnext.find(host);
                            if(it != cidnext.end() ){ // should be good
                                cout << "Bill exists! 0x65" << endl;
                                //do extend circuit function
                                /*
                                ofile.open(filename,std::fstream::app);
                                ofile << "mykey is: 0x";
                                for(int z = 0; z < 16; z++){
                                    uint16_t shade;
                                    memset(&shade,0,2);
                                    shade = (uint16_t)mykey[z];
                                    if((shade < 0x000f)){
                                        ofile << "0";
                                    }
                                    if(shade > 0x00ff){
                                        shade = shade && 0x00ff;
                                    }
                                    ofile << hex << shade; //rk1[z];
                                    
                                }
                                ofile << endl;
                                ofile.close();
                                 */
                                
                                uint16_t hostnext = cidnext[host]; //host byte order hostnext can be printed by dec <<
                                
                                char circuit[2048];
                                memset(circuit, 0, sizeof(circuit));
                                
                                uint8_t type = 0x65; // request type 1 byte
                                uint16_t hostc_id = myid;
                                uint16_t netwc_id = htons(hostc_id); //incoming circuit id 2bytes
                                /*
                                AES_KEY enc_key;
                                unsigned char *clear_crypt_text;
                                int clear_crypt_text_len;
                                
                                class_AES_set_decrypt_key(mykey, &enc_key);
                                unsigned char data[2048];
                                memset(data, 0, sizeof(data));
                                memcpy(data,minitorbuf+3,length-3);
                                
                                class_AES_decrypt_with_padding(data, length-3, &clear_crypt_text, &clear_crypt_text_len, &enc_key);
                                */
         
                                AES_KEY enc_key;
                                unsigned char *clear_crypt_text;
                                int clear_crypt_text_len;
                                unsigned char data[2048];
                                memset(data, 0, sizeof(data));
                                memcpy(data,minitorbuf+3,length-3);
                                
                                
                                class_AES_set_decrypt_key(mykey, &enc_key);
                                class_AES_decrypt_with_padding(data, length-3, &clear_crypt_text, &clear_crypt_text_len, &enc_key);
                                
                                cout << "past first decrypt" << endl;
                                //uint16_t netwport = htons(port1);
                                
                                ofile.open(filename,std::fstream::app); //puts decrypted version first
                                ofile << "fake-diffie-hellman, forwarding,  circuit incoming: 0x" << hex << int(host) << ", key: 0x";
                                for(int z = 0; z < clear_crypt_text_len; z++){
                                    uint16_t shade;
                                    memset(&shade,0,2);
                                    shade = (uint16_t)clear_crypt_text[z];
                                    if((shade < 0x000f)){
                                        ofile << "0";
                                    }
                                    if(shade > 0x00ff){
                                        shade = shade && 0x00ff;
                                    }
                                    ofile << hex << shade; //rk1[z];
                                    
                                }
                                for(int z = clear_crypt_text_len; z < length-3; z++){ //followed by rest of data
                                    uint16_t shade;
                                    memset(&shade,0,2);
                                    shade = (uint16_t)data[z];
                                    if((shade < 0x000f)){
                                        ofile << "0";
                                    }
                                    if(shade > 0x00ff){
                                        shade = shade && 0x00ff;
                                    }
                                    ofile << hex << shade; //rk1[z];
                                    
                                }
                                
                                ofile << endl;
                                
                                
                                ofile.close();

                                
                                int count = 0;
                                memcpy(circuit,buf,sizeof(struct ip));
                                count = sizeof(struct ip);
                                memcpy(circuit+count,&type,1);
                                count = count + 1;
                                memcpy(circuit+count,&netwc_id,2);
                                count = count + 2;
                                memcpy(circuit+count,clear_crypt_text,clear_crypt_text_len);
                                count = count + clear_crypt_text_len;
                                
                                
                                
                                cout << "this is the next port" << dec << hostnext << endl; //offially casts hostnext as an int dec
                                
                                
                                char hoppin[2048];
                                sprintf(hoppin, "%d",hostnext);
                                
                                
                                memset(&h, 0, sizeof h); // make sure the struct is empty
                                h.ai_family = AF_INET; // use IPv4
                                h.ai_socktype = SOCK_DGRAM; // UDP stream sockets
                                
                                
                                if ((status = getaddrinfo("localhost", hoppin, &h, &info)) != 0) {
                                    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
                                    exit(1);
                                }
                                
                                
                                sendto(sockfdr, circuit, count, 0, info->ai_addr, info->ai_addrlen);
                                
                            }
                            else { //working nicely besides sending part
                                cout << "Bill does not exist 0x65" << endl;
                                
                                cidnext[host] = 0x0000; //set port to zero and "unknown" for now
                                memset(mykey, 0, sizeof(mykey));
                                memcpy(mykey, minitorbuf+3, 16);
                                
                                cout << "this is mykey " << hex << mykey << endl;
                                
                                ofile.open(filename,std::fstream::app);
                                ofile << "fake-diffie-hellman, new circuit incoming: 0x" << hex << int(host);
                                ofile << ", key: 0x";
                                for(int z = 0; z < 16; z++){
                                    uint16_t shade;
                                    memset(&shade,0,2);
                                    shade = (uint16_t)mykey[z];
                                    if((shade < 0x000f)){
                                        ofile << "0";
                                    }
                                    if(shade > 0x00ff){
                                        shade = shade && 0x00ff;
                                    }
                                    ofile << hex << shade; //rk1[z];
                                    
                                }
                                ofile << endl;
                                ofile.close();
                                
                                
                            }
                            
                            
                        }
                        else if (minitorbuf[0] == 0x62) { //circuit-extend  type
                            
                            map<uint16_t,uint16_t>::iterator it;
                            it = cidnext.find(host);
                            if((it != cidnext.end()) && (cidnext[host] != 0x0000)){ // should be good
                                cout << "Bill exists! 0x62" << endl;
                                //do extend circuit function
                                
                                uint16_t hostnext = cidnext[host]; //host byte order hostnext can be printed by dec <<
                                
                                
                                
                                ofile.open(filename,std::fstream::app);
                                ofile << "forwarding extend circuit: incoming: 0x" << hex << int(host);
                                ofile << ", outgoing: 0x" << hex << int(myid);
                                ofile << " at " << dec << hostnext << endl; //my next hop
                                
                                ofile.close();
                                
                                char circuit[2048];
                                memset(circuit, 0, sizeof(circuit));
                                
                                uint8_t type = 0x62; // request type 1 byte
                                uint16_t hostc_id = myid;
                                uint16_t netwc_id = htons(hostc_id); //incoming circuit id 2bytes
                                
                                
                                AES_KEY enc_key;
                                unsigned char *clear_crypt_text;
                                int clear_crypt_text_len;
                                unsigned char data[2048];
                                memset(data, 0, sizeof(data));
                                memcpy(data,minitorbuf+3,length-3);
                                
                                class_AES_set_decrypt_key(mykey, &enc_key);
                                class_AES_decrypt_with_padding(data, length-3, &clear_crypt_text, &clear_crypt_text_len, &enc_key);
                                
                                cout << "Bill exists! 0x62 decrypt pass" << endl;

                                //uint16_t netwport = htons(port1); //used to be payload
                                
                                int count = 0;
                                memcpy(circuit,buf,sizeof(struct ip));
                                count = sizeof(struct ip);
                                memcpy(circuit+count,&type,1);
                                count = count + 1;
                                memcpy(circuit+count,&netwc_id,2);
                                count = count + 2;
                                memcpy(circuit+count,clear_crypt_text,clear_crypt_text_len);
                                count = count + clear_crypt_text_len;
                                
                                
                                
                                cout << "this is the next port" << dec << hostnext << endl; //offially casts hostnext as an int dec
                                
                                
                                char hoppin[2048];
                                sprintf(hoppin, "%d",hostnext);
                                
                                
                                memset(&h, 0, sizeof h); // make sure the struct is empty
                                h.ai_family = AF_INET; // use IPv4
                                h.ai_socktype = SOCK_DGRAM; // UDP stream sockets
                                
                                
                                if ((status = getaddrinfo("localhost", hoppin, &h, &info)) != 0) {
                                    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
                                    exit(1);
                                }
                                
                                
                                sendto(sockfdr, circuit, count, 0, info->ai_addr, info->ai_addrlen);
                                
                            }
                            else { //working nicely besides sending part
                                cout << "Bill does not exist 0x62" << endl;
                                
                                
                                AES_KEY enc_key;
                                unsigned char *clear_crypt_text;
                                int clear_crypt_text_len;
                                unsigned char data[2048];
                                memset(data, 0, sizeof(data));
                                memcpy(data,minitorbuf+3,length-3);
                                
                                
                                class_AES_set_decrypt_key(mykey, &enc_key);
                                class_AES_decrypt_with_padding(data, length-3, &clear_crypt_text, &clear_crypt_text_len, &enc_key);
                                
                                
                                uint16_t port1;
                                memcpy(&port1,clear_crypt_text,2);
                                
                                /*
                                 ofile.open(filename,std::fstream::app);
                                 ofile << "port1 is" << port1 << endl;
                                 ofile.close();
                                 */
                                myid = 0x0100*routernum + 0x0001;
                                
                                cidnext[host] = port1;   //incoming associated with next udp hop
                                cidreturn[myid] = ntohs(their_addr.sin_port); //outgoing associated with previous udp hop
                                cidreturnid[myid] = host;
                                ipreturn[myid] = their_addr;
                                
                                
                                ofile.open(filename,std::fstream::app);
                                ofile << "new extend circuit: incoming: 0x" << setw(2) << setfill('0') << hex << int(host);
                                ofile << ", outgoing: 0x" << hex << int(myid);
                                ofile << " at " << dec << port1 << endl;
                                
                                ofile.close();
                                
                                char circuit[2048];
                                memset(circuit, 0, sizeof(circuit));
                                
                                uint8_t type = 0x63; // request type 1 byte
                                uint16_t hostc_id = host;
                                uint16_t netwc_id = htons(hostc_id); //incoming circuit id 2bytes
                                
                                int count = 0;
                                memcpy(circuit,buf,sizeof(struct ip));
                                count = sizeof(struct ip);
                                memcpy(circuit+count,&type,1);
                                count = count + 1;
                                memcpy(circuit+count,&netwc_id,2);
                                count = count + 2;
                                
                                sendto(sockfdr, circuit, count, 0, (struct sockaddr *)&their_addr, sizeof(their_addr));
                                /*
                                 if(port1 == 0xffff){ // if you are the last
                                 flag1 = 0;
                                 break;
                                 }
                                 */
                            }
                            /*
                             ofile.open("stage2.proxy.out",std::fstream::app);
                             
                             ofile << "incoming extend- done circuit, incoming: 0x"
                             ofile << hex << int(minitorbuf[1]);
                             ofile << " from port: " << ntohs(their_addr.sin_port) << endl;
                             
                             ofile.close();
                             */
                        }
                        else if(minitorbuf[0] == 0x63){ // circuit-extend type
                            
                            uint16_t cid1 = (uint16_t)minitorbuf[1]; //upper
                            uint16_t cid2 = (uint16_t)minitorbuf[2]; //lower
                            cout << cid1 << endl;
                            cout << cid2 << endl;
                            cid1 = cid1*256;
                            cid1 = cid1 + cid2;
                            uint16_t host = cid1; //convert from network to host basically switch lower with upper
                            
                            uint16_t prevport = cidreturn[host]; // host should be myid host byte order
                            uint16_t previd = cidreturnid[host];
                            
                            
                            char circuit[25];
                            memset(circuit, 0, sizeof(circuit));
                            
                            uint8_t type = 0x63; // request type 1 byte
                            uint16_t hostc_id = previd;
                            uint16_t netwc_id = htons(hostc_id); //incoming circuit id 2bytes
                            
                            int count = 0;
                            memcpy(circuit,buf,sizeof(struct ip));
                            count = sizeof(struct ip);
                            memcpy(circuit+count,&type,1);
                            count = count + 1;
                            memcpy(circuit+count,&netwc_id,2);
                            count = count + 2;
                            
                            memset(&h, 0, sizeof h); // make sure the struct is empty
                            h.ai_family = AF_INET; // use IPv4
                            h.ai_socktype = SOCK_DGRAM; // UDP stream sockets
                            
                            ofile.open(filename,std::fstream::app);
                            ofile << "forwarding extend-done circuit: incoming: 0x" << hex << int(host);
                            ofile << ", outgoing: 0x" << setw(2) << setfill('0') << hex << previd;
                            ofile << " at " << dec << prevport << endl; //my next hop
                            
                            ofile.close();
                            
                            
                            cout << "this is the previous port" << dec << prevport << endl;
                            
                            
                            char hoppin[2048];
                            sprintf(hoppin, "%d",prevport);
                            
                            
                            if ((status = getaddrinfo("localhost", hoppin, &h, &info)) != 0) {
                                fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
                                exit(1);
                            }
                            
                            
                            sendto(sockfdr, circuit, count, 0, info->ai_addr, info->ai_addrlen);
                            
                            /*
                             flag1 = 0;  // we only have one circuit so we can exit after one circuit extend done is completed
                             break;
                             */
                            
                        }
                        else if(minitorbuf[0] == 0x61){
                            //decrypt
                            AES_KEY enc_key;
                            unsigned char *clear_crypt_text;
                            int clear_crypt_text_len;
                            unsigned char data[2048];
                            memset(data, 0, sizeof(data));
                            memcpy(data,minitorbuf+3,length-3);
                            
                            
                            class_AES_set_decrypt_key(mykey, &enc_key);
                            class_AES_decrypt_with_padding(data, length-3, &clear_crypt_text, &clear_crypt_text_len, &enc_key);
                            //decrpyt
                            
                            
                            map<uint16_t,uint16_t>::iterator it;
                            it = cidnext.find(host);
                            if(it != cidnext.end() ){ // should be good
                                cout << "Sara exists!" << endl;
                                uint16_t nextport = cidnext[host];
                                
                                if(nextport == 0xffff){ //Add for Stage 7
                                    
                                    //extra?
                                    struct ip realip;
                                    struct in_addr srcreal;
                                    struct in_addr destreal;
                                    char IP_srcreal[NI_MAXHOST];
                                    char IP_destreal[NI_MAXHOST];
                                    
                                    memcpy(&realip,clear_crypt_text,sizeof(struct ip));
                                    
                                    srcreal = realip.ip_src;
                                    inet_ntop(AF_INET, &srcreal, IP_srcreal, sizeof(IP_srcreal));
                                    
                                    destreal = realip.ip_dst;
                                    inet_ntop(AF_INET, &destreal, IP_destreal, sizeof(IP_destreal));
                                    //extra?
                                    
                                    //Stage 7
                                    u_char protocol;
                                    protocol = realip.ip_p;
                                    
                                    if(protocol == IPPROTO_ICMP){
                                        struct ip * tun_ICMP;      //stores ICMP message in tun_ICMP
                                        struct in_addr tun_src;  //test
                                        struct in_addr tun_dest;
                                        char tun_ip_src[NI_MAXHOST];
                                        char tun_ip_dest[NI_MAXHOST];
                                        struct icmp * tun_rICMP;
                                        u_char tun_utype;
                                        int hlenl;
                                    
                                    
                                        tun_ICMP = (struct ip *)(clear_crypt_text); //clear_crypt_text instead of minitorbuf+3
                                        tun_src = tun_ICMP->ip_src;
                                        inet_ntop(AF_INET, &tun_src, tun_ip_src, sizeof(tun_ip_src));
                                    
                                        tun_dest = tun_ICMP->ip_dst;
                                        inet_ntop(AF_INET, &tun_dest, tun_ip_dest, sizeof(tun_ip_dest));
                                    
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
                                            tun_rICMP = (struct icmp *)(clear_crypt_text + hlenl*4); // char = 8 so mult by 4
                                            tun_utype = tun_rICMP->icmp_type;
                                        
                                            ofiler.open(filename, std::fstream::app);
                                            /*
                                             ofiler << "ICMP from port: " << ntohs(their_addr.sin_port) << ", src: " << tun_ip_src << ", dst: " << tun_ip_dest << ", type: ";
                                        
                                             if(tun_utype == 8){
                                             //cout << "router says type is 8" << endl;
                                             ofiler << "8\n";
                                             }
                                             */
                                            uint16_t going = cidreturnid[myid];
                                        
                                            uint16_t icid;
                                            memcpy(&icid,buf+sizeof(struct ip)+1,2); //copying incoming circuit id
                                            uint16_t icidnet = htons(icid);
                                            ipreturnchar[myid] = IP_srcreal; //tun_ip_src;
                                        
                                            ofiler << "outgoing packet, circuit incoming: 0x" << hex << icidnet;
                                            ofiler << ", incoming src: " << tun_ip_src << ", outgoing src: " << myIP << ", dst: " << tun_ip_dest << endl;

                                            ofiler << "incoming packet, src: " << myIP << ", dst: " << tun_ip_src << ", outgoing circuit: 0x" << hex << cidreturn[myid] << endl;
                                        
                                        
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

                                            AES_KEY enc_key1;
                                            unsigned char *crypt_text;
                                            int crypt_text_len;
                                        
                                            class_AES_set_encrypt_key(mykey, &enc_key1);
                                            class_AES_encrypt_with_padding(clear_crypt_text, clear_crypt_text_len, &crypt_text, &crypt_text_len, &enc_key1);
                                        

                                            //memcpy(buf+sizeof(struct ip)+1,&going,sizeof(going));
                                            char sendbuf[2048];
                                            memset(sendbuf,0,2048);
                                            int increment = 0;
                                            memcpy(sendbuf,buf,sizeof(struct ip)); //copies minitor ip
                                            uint8_t typed = 0x64; //type = 64
                                            memcpy(sendbuf+sizeof(struct ip),&typed,1); //type
                                            memcpy(sendbuf+sizeof(struct ip)+1,&going,2); //circuit id
                                            memcpy(sendbuf+sizeof(struct ip)+3,crypt_text,crypt_text_len);
                                            increment = sizeof(struct ip)+3+ crypt_text_len;
                                        
                                            sendto(sockfdr, sendbuf, increment, 0, (struct sockaddr *)&their_addr, sizeof(their_addr)); // send message back to person who sent it to me
                                            recv_bytes = 0;
                                        }
                                        else if(!((a == e) && (b == f) && (c == g))){ // packet is not destined to eth1 network
                                        
                                            cout << "message is directed to MIT" << endl;
                                            cout << "address of MIT is: " << tun_ip_dest << endl;

                                        
                                            hlenl = tun_ICMP->ip_hl;
                                            tun_rICMP = (struct icmp *)(clear_crypt_text + hlenl*4); // char = 8 so mult by 4
                                            tun_utype = tun_rICMP->icmp_type;
                                        
                                            ofiler.open(filename, std::fstream::app);
                                        
                                            uint16_t icid;
                                            memcpy(&icid,buf+sizeof(struct ip)+1,2); //copying incoming circuit id

                                            ipreturnchar[myid] = IP_srcreal; //tun_ip_src;
                                        
                                            ofiler << "outgoing packet, circuit incoming: 0x" << hex << htons(icid);
                                            ofiler << ", incoming src: " << tun_ip_src << ", outgoing src: " << myIP << ", dst: " << tun_ip_dest << endl;
                                        
                                        
                                            ofiler.close();
                                        
                                            //setting IP_HDRINCL
                                        
                                            int hdrincl=0;
                                            if (setsockopt(rawsockfd,IPPROTO_IP,IP_HDRINCL,&hdrincl,sizeof(hdrincl))    ==-1) {
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
                                            if (statusr != 0){ //
                                                fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(statusr));
                                                return 1;
                                            }//
                                        
                                        
                                        
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
                                        
                                            if(sendmsg(rawsockfd, &message, 0) == -1){ //
                                                cout << "error in sending raw sock" << endl;
                                            } //
                                        
                                        }// packet is not destined to eth1 network
                                    }//if ICMP
                                    else if(protocol == IPPROTO_TCP){
                                        
                                        struct ip * tun_ICMP;      //stores ICMP message in tun_ICMP
                                        struct in_addr tun_src;  //test
                                        struct in_addr tun_dest;
                                        char tun_ip_src[NI_MAXHOST];
                                        char tun_ip_dest[NI_MAXHOST];
                                        /*
                                        struct icmp * tun_rICMP;
                                        u_char tun_utype;
                                        */
                                        int hlenl;
                                        
                                        
                                        tun_ICMP = (struct ip *)(clear_crypt_text); //clear_crypt_text instead of minitorbuf+3
                                        tun_src = tun_ICMP->ip_src;
                                        inet_ntop(AF_INET, &tun_src, tun_ip_src, sizeof(tun_ip_src));
                                        
                                        tun_dest = tun_ICMP->ip_dst;
                                        inet_ntop(AF_INET, &tun_dest, tun_ip_dest, sizeof(tun_ip_dest));
                                        
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
                                            
                                            /* Stage 7
                                            hlenl = tun_ICMP->ip_hl;
                                            tun_rICMP = (struct icmp *)(clear_crypt_text + hlenl*4); // char = 8 so mult by 4
                                            tun_utype = tun_rICMP->icmp_type;
                                            */
                                             
                                            /*
                                             ofiler << "ICMP from port: " << ntohs(their_addr.sin_port) << ", src: " << tun_ip_src << ", dst: " << tun_ip_dest << ", type: ";
                                             
                                             if(tun_utype == 8){
                                             //cout << "router says type is 8" << endl;
                                             ofiler << "8\n";
                                             }
                                             */
                                            uint16_t going = cidreturnid[myid];
                                            
                                            uint16_t icid;
                                            memcpy(&icid,buf+sizeof(struct ip)+1,2); //copying incoming circuit id
                                            uint16_t icidnet = htons(icid);
                                            ipreturnchar[myid] = IP_srcreal; //tun_ip_src;
                                            
                                            if(tun_ICMP->ip_p == IPPROTO_ICMP){
                                                ofiler.open(filename, std::fstream::app);

                                                ofiler << "outgoing packet, circuit incoming: 0x" << hex << icidnet;
                                                ofiler << ", incoming src: " << tun_ip_src << ", outgoing src: " << myIP << ", dst: " << tun_ip_dest << endl;
                                            
                                                ofiler << "incoming packet, src: " << myIP << ", dst: " << tun_ip_src << ", outgoing circuit: 0x" << hex << cidreturn[myid] << endl;
                                                ofiler.close();
                                            }
                                            if(tun_ICMP->ip_p == IPPROTO_TCP){
                                                struct tcphdr *tcp_hdr;
                                                int header = tun_ICMP->ip_hl;
                                                tcp_hdr = (struct tcphdr *)(clear_crypt_text + header*4);
                                                
                                                ofiler.open(filename, std::fstream::app);

                                                ofiler << "outgoing TCP packet, circuit incoming: 0x" << hex << htons(icid);
                                                ofiler << ", incoming src IP/port: " << tun_ip_src << "/" << tcp_hdr->source << ", outgoing src IP/port: " << myIP << ":" << tcp_hdr->source  << ", dst IP/port: " << tun_ip_dest << ":" << tcp_hdr->dest << ", seqno: " << tcp_hdr-> seq << ", ackno: " << tcp_hdr->ack_seq << endl;
                                                ofiler.close();
                                            }
    
                                            /* Stage 7
                                            tun_rICMP->icmp_type = 0;
                                            tun_utype = tun_rICMP->icmp_type;
                                            
                                            if(tun_utype == 0){
                                                //cout << "router sends type as 0" << endl;
                                            }
                                            Stage 7 */
                                            
                                            /*
                                             strcpy(buf,(char *)tun_ICMP);
                                             strcat(buf,(char *)tun_rICMP);
                                             */
                                            
                                            AES_KEY enc_key1;
                                            unsigned char *crypt_text;
                                            int crypt_text_len;
                                            
                                            class_AES_set_encrypt_key(mykey, &enc_key1);
                                            class_AES_encrypt_with_padding(clear_crypt_text, clear_crypt_text_len, &crypt_text, &crypt_text_len, &enc_key1);
                                            
                                            
                                            //memcpy(buf+sizeof(struct ip)+1,&going,sizeof(going));
                                            char sendbuf[2048];
                                            memset(sendbuf,0,2048);
                                            int increment = 0;
                                            memcpy(sendbuf,buf,sizeof(struct ip)); //copies minitor ip
                                            uint8_t typed = 0x64; //type = 64
                                            memcpy(sendbuf+sizeof(struct ip),&typed,1); //type
                                            memcpy(sendbuf+sizeof(struct ip)+1,&going,2); //circuit id
                                            memcpy(sendbuf+sizeof(struct ip)+3,crypt_text,crypt_text_len);
                                            increment = sizeof(struct ip)+3+ crypt_text_len;
                                            
                                            sendto(sockfdr, sendbuf, increment, 0, (struct sockaddr *)&their_addr, sizeof(their_addr)); // send message back to person who sent it to me
                                            recv_bytes = 0;
                                        }
                                        else if(!((a == e) && (b == f) && (c == g))){ // packet is not destined to eth1 network
                                            
                                            cout << "message is directed to MIT and is TCP" << endl;
                                            cout << "address of MIT is: " << tun_ip_dest << endl;
                                            /*
                                            struct tcphdr *tcp_hdr;
                                            hlenl = tun_ICMP->ip_hl;
                                            tcp_hdr = (struct tcphdr *)(clear_crypt_text + hlenl*4); // char = 8 so mult by 4
                                            */
                                            /*
                                            struct pseudo_tcphdr
                                            {
                                                struct in_addr source, dest;
                                                u_char reserve;
                                                u_char protocol;
                                                u_short length;
                                            }
                                            */

                                            
                                            /* Stage 7
                                            hlenl = tun_ICMP->ip_hl;
                                            tun_rICMP = (struct icmp *)(clear_crypt_text + hlenl*4); // char = 8 so mult by 4
                                            tun_utype = tun_rICMP->icmp_type;
                                            */
                                            cout << endl << endl << "sent out raw3" << endl << endl;

                                            
                                            uint16_t icid;
                                            memcpy(&icid,buf+sizeof(struct ip)+1,2); //copying incoming circuit id
                                            
                                            ipreturnchar[myid] = IP_srcreal; //tun_ip_src;
    
                                            cout << endl << endl << "sent out raw4" << endl << endl;

                                            inet_pton(AF_INET, myIP, &(tun_ICMP->ip_src));
                                            hlenl = tun_ICMP->ip_hl;
                                            struct tcphdr *tcp_hdr;
                                            int total_tcp = ntohs(tun_ICMP->ip_len) - hlenl*4;
                                            tcp_hdr = (struct tcphdr *)(clear_crypt_text + hlenl*4); // char = 8 so mult by 4
                                            
                                            tcp_checksum(tcp_hdr,total_tcp,tun_ICMP);
                                            
                                            ofiler.open(filename, std::fstream::app);

                                            ofiler << "outgoing TCP packet, circuit incoming: 0x" << hex << htons(icid);
                                            ofiler << ", incoming src IP/port: " << tun_ip_src << "/" << tcp_hdr->source << ", outgoing src IP/port: " << myIP << ":" << tcp_hdr->source  << ", dst IP/port: " << tun_ip_dest << ":" << tcp_hdr->dest << ", seqno: " << tcp_hdr-> seq << ", ackno: " << tcp_hdr->ack_seq << endl;
                                            
                                            
                                            ofiler.close();
                                            
                                            //setting IP_HDRINCL
                                            /*
                                            int hdrincl=0;
                                            if (setsockopt(rawsockfd,IPPROTO_IP,IP_HDRINCL,&hdrincl,sizeof(hdrincl))    ==-1) {
                                                cout << "error in setsockopt" << endl;
                                            }
                                            */
                                            
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
                                            iov[0].iov_base= tcp_hdr; //message is the ICMP header + payload (tun_rICMP or icmp)
                                            iov[0].iov_len = ntohs(tun_ICMP->ip_len) - hlenl*4; // *4 because of char buffer? well no, iov_len is measured in bytes and a char is a byte
                                            
                                            struct addrinfo *ipdest2;
                                            struct addrinfo ipdest1;
                                            
                                            memset(&ipdest1, 0, sizeof ipdest1); // make sure the struct is empty
                                            
                                            ipdest1.ai_family = AF_INET; // use IPv4
                                            ipdest1.ai_socktype = SOCK_RAW; // RAW stream sockets
                                            
                                            
                                            statusr = getaddrinfo(tun_ip_dest, NULL, &ipdest1, &ipdest2);
                                            if (statusr != 0){ //
                                                fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(statusr));
                                                return 1;
                                            }//
                                            
                                            
                                            
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
                                            
                                            cout << endl << endl << "sent out raw7" << endl << endl;
                                            
                                            if(sendmsg(rawtcp, &message, 0) == -1){ //
                                                cout << "error in sending raw sock" << endl;
                                            } //
                                            cout << endl << endl << "sent out raw8" << endl << endl;

                                        }// packet is not destined to eth1 network
                                    }//if TCP
                                }// last port
                                else if(nextport != 0xffff){// not last port
                                    ofile.open(filename,std::fstream::app);
                                    ofile << "relay encrypted packet, circuit incoming: 0x" << setw(2) << setfill('0') << hex << int(host);
                                    ofile << ", outgoing: 0x" << setw(2) << setfill('0') << hex << int(myid);
                                    ofile << endl;
                                    //ofile << " incoming src: " << IP_srcreal; //from source
                                    //ofile << ", outgoing src: " << myIP << ", dst: " << IP_destreal << endl;
                                    ofile.close();

                                    //ipreturnchar[myid] = IP_srcreal;
                                    
                                    char nbuffer[2048];
                                    int outnum = 0;
                                    memset(nbuffer, 0, sizeof(nbuffer));
                                    memcpy(nbuffer,buf,sizeof(struct ip)+1);   //copies minitor struct and 0x61;
                                    outnum = sizeof(struct ip)+1;
                                    uint16_t temp = ntohs(myid);
                                    memcpy(nbuffer+outnum,&temp,sizeof(temp)); //copies outgoing circuit id
                                    outnum = outnum + sizeof(temp);
                                    /*
                                    struct ip * min_IP;      //stores ICMP message in tun_ICMP
                                    
                                    min_IP = (struct ip *)(buf+sizeof(struct ip)+3);
                                    inet_pton(AF_INET, myIP, &(min_IP->ip_src)); //storing my address in mintor's source addrss
                                    min_IP->ip_sum = 0;
                                    min_IP->ip_sum = ip_checksum(min_IP,sizeof(struct ip));
                                    */
                                    //memcpy(nbuffer+outnum,buf+ sizeof(struct ip) +3,length -3);
                                    //outnum = outnum + (length -3);
                                    
                                    //memcpy(circuit+count,clear_crypt_text,clear_crypt_text_len);
                                    //count = count + clear_crypt_text_len;
                                    
                                    memcpy(nbuffer+outnum,clear_crypt_text,clear_crypt_text_len);
                                    outnum = outnum + clear_crypt_text_len;
                                    
                                    uint16_t hostnext = cidnext[host];
                                    
                                    char hoppin[2048];
                                    sprintf(hoppin, "%d",hostnext);
                                    
                                    
                                    if ((status = getaddrinfo("localhost", hoppin, &h, &info)) != 0) {
                                        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
                                        exit(1);
                                    }
                                    
                                    
                                    sendto(sockfdr, nbuffer, outnum, 0, info->ai_addr, info->ai_addrlen);
                            
                                    
                                } // not last port
                            } // Sara does  exist
                               
                        else{ //does not exist
                            cout << "Sara does not exists!" << endl;
                            ofile.open(filename,std::fstream::app);
                            ofile << "unknown incoming circuit: 0x" << hex << int(host);
                            //ofile << ", src: " << IP_srcreal << ", dst: " << IP_destreal << endl;
                            ofile.close();
                            }// does not exist
                        
                        } // if 0x51
                        else if(minitorbuf[0] == 0x64){
                            //encrypt
                            AES_KEY enc_key1;
                            unsigned char *crypt_text;
                            int crypt_text_len;
                            unsigned char data[2048];
                            memset(data, 0, sizeof(data));
                            memcpy(data,minitorbuf+3,length-3);
                            
                            class_AES_set_encrypt_key(mykey, &enc_key1);
                            class_AES_encrypt_with_padding(data, length-3, &crypt_text, &crypt_text_len, &enc_key1);
                            
                            //memcpy(circuit+increment,crypt_text,crypt_text_len); //payload
                            //increment = increment + crypt_text_len;
                            //encrypt

                            
                            
                            //
                            uint16_t incoming;
                            memcpy(&incoming, buf+sizeof(struct ip)+1, 2);
                            uint16_t incom = ntohs(incoming);
                            //only use for sending out
                            struct sockaddr_in previous;
                            previous = ipreturn[myid];
                            
                            /*
                            struct ip *realip;
                            struct in_addr srcreal;
                            struct in_addr destreal;
                            char IP_srcreal[NI_MAXHOST];
                            char IP_destreal[NI_MAXHOST];
                            
                            realip = (struct ip *)(minitorbuf+3);
                            
                            
                            srcreal = realip->ip_src;
                            inet_ntop(AF_INET, &srcreal, IP_srcreal, sizeof(IP_srcreal));
                            
                            destreal = realip->ip_dst;
                            inet_ntop(AF_INET, &destreal, IP_destreal, sizeof(IP_destreal));
                            
                            char *winner;
                            winner = ipreturnchar[myid];
                            */
                             
                            ofile.open(filename,std::fstream::app);
                            ofile << "relay reply encrypted packet, circuit incoming: 0x" << hex << incom;
                            ofile << ", outgoing: 0x" << setw(2) << setfill('0') << hex << cidreturnid[myid];
                            //ofile << ", src: " << IP_srcreal << ", incoming dst: " << IP_destreal << ", outgoing dst: " << winner << endl;
                            ofile << endl;
                            ofile.close();

                            
                            /*
                            char nbuffer[2048];
                            int outnum = 0;
                            memset(nbuffer, 0, sizeof(nbuffer));
                            memcpy(nbuffer,buf,sizeof(struct ip)+1);   //copies minitor struct and 0x51;
                            outnum = sizeof(struct ip)+1;
                            uint16_t temp = ntohs(myid);
                            memcpy(nbuffer+outnum,&temp,sizeof(temp)); //copies outgoing circuit id
                            outnum = outnum + sizeof(temp);
                            
                            struct ip * min_IP;      //stores ICMP message in tun_ICMP
                            
                            min_IP = (struct ip *)(buf+sizeof(struct ip)+3);
                            inet_pton(AF_INET, myIP, &(min_IP->ip_src)); //storing my address in mintor's source addrss
                            min_IP->ip_sum = ip_checksum(min_IP,sizeof(min_IP));
                            
                            memcpy(nbuffer+outnum,buf+ sizeof(struct ip) +3,length -3);
                            outnum = outnum + (length -3);
                            
                            uint16_t hostnext = cidnext[host];
                            
                            char hoppin[2048];
                            sprintf(hoppin, "%d",hostnext);
                            
                            
                            if ((status = getaddrinfo("localhost", hoppin, &h, &info)) != 0) {
                                fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
                                exit(1);
                            }

                            sendto(sockfdr, nbuffer, outnum, 0, info->ai_addr, info->ai_addrlen);
                            */
                            
                            
                            
                            char nbuffer[2048];
                            int outnum = 0;
                            memset(nbuffer, 0, sizeof(nbuffer));
                            
                            memcpy(nbuffer,buf,sizeof(struct ip)+1);   //copies minitor struct and 0x64;
                            outnum = sizeof(struct ip)+1;
                            uint16_t temp = ntohs(cidreturnid[myid]);
                            memcpy(nbuffer+outnum,&temp,sizeof(temp)); //copies outgoing circuit id
                            outnum = outnum + sizeof(temp);
                            
                            /* stage 6 comment out
                            struct ip * min_IP;      //stores ICMP message in tun_ICMP
                            
                            min_IP = (struct ip *)(buf+sizeof(struct ip)+3);
                            inet_pton(AF_INET, winner, &(min_IP->ip_dst)); //storing my address in mintor's source addrss
                            min_IP->ip_sum = 0;
                            min_IP->ip_sum = ip_checksum(min_IP,sizeof(struct ip));

                            
                            memcpy(nbuffer+outnum,buf+sizeof(struct ip) + 3, length -3);
                            outnum = outnum + (length - 3);
                            */
                            memcpy(nbuffer+outnum,crypt_text,crypt_text_len); //payload
                            outnum = outnum + crypt_text_len;
                            
                            sendto(sockfdr, nbuffer, outnum, 0, (struct sockaddr *)&previous, sizeof(previous));
                            
                        }
                    }//end of if minitor
                
                }//if reads bytes
            } // end of if read from sockfd
            if( FD_ISSET(rawsockfd, &readfds) || FD_ISSET(rawtcp, &readfds) ){ //if rawsockfd has something to read from

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
                
                if(FD_ISSET(rawtcp, &readfds)){
                    recv_bytes = recvmsg(rawtcp,&msgh,0); //bTODO this is all I have to understand! we also need to add a IP header?
                }
                   else if(FD_ISSET(rawsockfd, &readfds)){
                       recv_bytes = recvmsg(rawsockfd,&msgh,0); //bTODO this is all I have to understand! we also need to add a IP header?
                   }
                
                if(recv_bytes){
                    cout << "router got message" << endl;
                    
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
                        
                        if(tun_ICMP->ip_p == IPPROTO_ICMP){
                            hlenl = tun_ICMP->ip_hl;
                            tun_rICMP = (struct icmp *)(recbuf + hlenl*4); // char = 8 so mult by 4
                            tun_utype = tun_rICMP->icmp_type;
                    
                            cout << "this is the icmp type" << tun_utype;
                    
                        
                        //tun_utype = 0;
                        
                            ofiler.open(filename, std::fstream::app);
                            ofiler << "incoming packet, src: " << tun_ip_src << ", dst: " << tun_ip_dest << ", outgoing circuit: 0x" << hex << cidreturnid[myid] << endl;
                            ofiler.close();
                        }
                        if(tun_ICMP->ip_p == IPPROTO_TCP){
                            struct tcphdr *tcp_hdr;
                            int header = tun_ICMP->ip_hl;
                            tcp_hdr = (struct tcphdr *)(recbuf + header*4);
                            
                            ofiler.open(filename, std::fstream::app);
                            ofiler << "incoming TCP packet, src IP/port: " << tun_ip_src << ":" << tcp_hdr->source << ", outgoing src IP/port: " << myIP << ":" << tcp_hdr->source  << ", dst IP/port: " << tun_ip_dest << ":" << tcp_hdr->dest << ", seqno: " << tcp_hdr-> seq << ", ackno: " << tcp_hdr->ack_seq << ", outgoing circuit: 0x" << hex << cidreturnid[myid] << endl;
                            ofiler.close();

                        }
                        char circuit[2048];
                        memset(&circuit, 0, sizeof(circuit));
                        
                        struct ip minhdr;
                        memset(&minhdr, 0, sizeof(minhdr));
                        
                        inet_pton(AF_INET, "127.0.0.1", &(minhdr.ip_src)); //stores minitor IP in_addr
                        inet_pton(AF_INET, "127.0.0.1", &(minhdr.ip_dst));
                        
                        minhdr.ip_p = 253;
                        
                        uint8_t type = 0x64; // request type 1 byte
                        uint16_t back = cidreturnid[myid];
                        
                        int increment = 0;
                        memcpy(circuit, &minhdr, sizeof(minhdr));
                        increment = sizeof(minhdr);
                        memcpy(circuit+increment,&type,sizeof(type));
                        increment = increment + sizeof(type);
                        uint16_t netback = ntohs(back);
                        memcpy(circuit+increment,&netback,sizeof(netback));
                        increment = increment + sizeof(back);
                        
                        //only use this for sent to an not writing
                        struct sockaddr_in previous;
                        previous = ipreturn[myid];
                        
                        /* stage6 commented
                        char *outlet;
                        outlet = ipreturnchar[myid];
                        inet_pton(AF_INET, outlet, &(tun_ICMP->ip_dst));
                        tun_ICMP->ip_sum = 0;
                        tun_ICMP->ip_sum = ip_checksum(tun_ICMP,sizeof(struct ip));
                        */
                         //tun_ICMP->ip_dst = previous.sin_addr; //setting the dest IP as the next router's IP address
                        //tun_ICMP->ip_sum = 0;
                        //tun_ICMP->ip_sum = ip_checksum(tun_ICMP,sizeof(struct ip));
                        /*
                        ofiler.open(filename, std::fstream::app);
                        ofiler << "outgoing IP " << outlet << endl;
                        ofiler.close();
                        */
                        AES_KEY enc_key1;
                        unsigned char *crypt_text;
                        int crypt_text_len;
                        
                        class_AES_set_encrypt_key(mykey, &enc_key1);
                        class_AES_encrypt_with_padding((unsigned char*)recbuf, recv_bytes, &crypt_text, &crypt_text_len, &enc_key1);
                        
                        memcpy(circuit+increment,crypt_text,crypt_text_len); //payload
                        increment = increment + crypt_text_len;

                        /*
                        struct ip* change;
                        change = (struct ip*)(circuit + increment -recv_bytes);
                        change->ip_dst = previous.sin_addr; //setting the dest IP as the next router's IP address
                        change->ip_sum = 0;
                        change->ip_sum = ip_checksum(change,sizeof(struct ip));
                        */
                        
                    
                        /*
                        recvaddr = (struct sockaddr_in *) msgh.msg_name;
                        tun_src = recvaddr->sin_addr;
                        inet_ntop(AF_INET, &tun_src, tun_ip_src, sizeof(tun_ip_src));
                    
                        cout << "router says source is :" << tun_ip_src << endl;
                    
                        */
                    
                        //skip for now
                        //TODOb to create an IP header
                        /*
                        memset(&eth0, 0, sizeof eth0);
                        char interface[] = "eth0";
                        char abs[NI_MAXHOST];
                        find_ipaddr(interface,abs);
                        cout << "this is eth0's IP: " << abs << endl;
                        inet_pton(AF_INET, abs, &(eth0.sin_addr));

                        tun_ICMP->ip_dst = eth0.sin_addr; // only change destination to eth0, keep source the same (eth0.sin_addr)
                        */
                        
                        //tun_ICMP->ip_src = tun_dest;
                        //tun_ICMP->ip_dst = tun_src;
                        
                        
                        //tun_rICMP->icmp_type = 0;
                        
                        /*
                        tun_rICMP->icmp_cksum = 0;
                        tun_rICMP->icmp_cksum = ip_checksum(tun_rICMP,28);
                        */
                       /*
                        tun_ICMP->ip_sum = 0;
                        tun_ICMP->ip_sum = ip_checksum(tun_ICMP,20);
                        */
                        /*
                         tun_rICMP->icmp_type = 0;
                         tun_utype = tun_rICMP->icmp_type;
                
                        if(tun_utype == 0){
                            //cout << "router sends type as 0" << endl;
                        }
                        strcpy(buf,(char *)tun_ICMP);
                        strcat(buf,(char *)tun_rICMP);
                        */
                        sendto(sockfdr, circuit, increment, 0, (struct sockaddr *)&previous, sizeof(previous)); //send message back to proxy (struct sockaddr *)&their_addr, sizeof(their_addr)
                    }
                    recv_bytes = 0;
                    
                    
                }
                
            }
            FD_ZERO(&readfds);
            
            //adding our descriptors to the set
            FD_SET(sockfdr, &readfds);
            FD_SET(rawsockfd, &readfds);
            FD_SET(rawtcp, &readfds);
            
            //Stage 7
            if(sockfdr >= rawsockfd){
                if(sockfdr >= rawtcp){
                    n = sockfdr + 1;
                }
                else if(rawtcp > sockfdr){
                    n = rawtcp + 1;
                }
            }
            else if(rawsockfd > sockfdr){
                if(rawsockfd >= rawtcp){
                    n = rawsockfd + 1;
                }
                else if(rawtcp > rawsockfd){
                    n = rawtcp + 1;
                }
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
       
       map<uint16_t,char*> ipreturnchar; // key = my circuit id, data = raw socket IP address
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
        ofile.open("stage5.proxy.out");
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
           

           
           router_port = ntohs(their_addr.sin_port); //port address is stored
           
           cout << "router's msg: " << router_buf <<  endl;
           space = strchr(router_buf,' ');
           *space = '\0';
           space = space + 1;
           routerid = space;
           
           cout << "router pid: " << router_buf << endl;
           cout << "router number: " << routerid << endl;
           
           ofile << "router: " << routerid << ", pid: " << router_buf << ", port: " << router_port;
           
           if(strcmp(routerid,"1") == 0){
               cout << "router 1 YOOHHOOO" << endl;
               their_addr1 = their_addr;
               ofile << ", IP: 192.168.201.2" << endl;
               //addr_len1 = addr_len;
           }
           else if(strcmp(routerid,"2") == 0){
               cout << "router 2 YOOHHOOO" << endl;
               their_addr2 = their_addr;
               ofile << ", IP: 192.168.202.2" << endl;
               //addr_len2 = addr_len;
           }
           else if(strcmp(routerid,"3") == 0){
               cout << "router 3 YOOHHOOO" << endl;
               their_addr3 = their_addr;
               ofile << ", IP: 192.168.203.2" << endl;
               //addr_len3 = addr_len;
           }
           else if(strcmp(routerid,"4") == 0){
               cout << "router 4 YOOHHOOO" << endl;
               their_addr4 = their_addr;
               ofile << ", IP: 192.168.204.2" << endl;
               //addr_len4 = addr_len;
           }
           else if(strcmp(routerid,"5") == 0){
               cout << "router 5 YOOHHOOO" << endl;
               their_addr5 = their_addr;
               ofile << ", IP: 192.168.205.2" << endl;
               //addr_len5 = addr_len;
           }
           else if(strcmp(routerid,"6") == 0){
               cout << "router 6 YOOHHOOO" << endl;
               their_addr6 = their_addr;
               ofile << ", IP: 192.168.206.2" << endl;
               //addr_len6 = addr_len;
           }
           
           
           

           
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
           
           
           
        //Stage5 circuit setup sent to first router, and put next name as sequential next
           int direction; //first sending is perfect 0x520001(port number as is and take in without flipping)
           int r;
           int temphops = hops;
           unsigned char *store;
           int temp;
           srand (time(0));
           
           //int number1 = routnum10;
           
           while(routnum10 > 0){
               r = rand();
               direction = r % routnum10; //numbers 0 - num -1
               temp = next_router[direction];
               store = keyorder[direction];
               next_router.erase(next_router.begin() + direction); // erased this element
               keyorder.erase(keyorder.begin() + direction);
               next_router.push_back(temp); //first hop pushed back
               keyorder.push_back(store); //first hop pushed back
               routnum10 = routnum10 - 1;
           }
           
           
           /*
           if(number1 < 6){
               for(int bf = 6 - number1; bf > 0; bf--){
                   keyorder.pop_front();
               }
           }
           */
           direction = next_router[0]; //first hop
           unsigned char firstkey[16];
           memset(firstkey, 0, 16);
           memcpy(firstkey,keyorder[0],16);
           /*
           ofile.open("stage2.proxy.out",std::fstream::app);
           ofile << "size of char is :" << sizeof(tun_buf[0]) << endl;
           ofile.close();
            */
           
           //rk2[0] = 0x01;
           //direction = 3;// for now
           if((direction == 1) && (temphops)){
               int nrouter;
               ofile.open("stage5.proxy.out",std::fstream::app);
               for(int hop_t = 0; hop_t < hops; hop_t++){
                   nrouter = next_router[hop_t];
                   ofile << "hop: " << hop_t+1 << ", router: " << nrouter << endl;
               }
               ofile.close();
               
               
               char circuit[2048];
               memset(circuit, 0, sizeof(circuit));
               
               struct ip minhdr;
               memset(&minhdr, 0, sizeof(minhdr));
               
               inet_pton(AF_INET, "127.0.0.1", &(minhdr.ip_src)); //stores minitor IP in_addr
               inet_pton(AF_INET, "127.0.0.1", &(minhdr.ip_dst));
               
               minhdr.ip_p = 253;
               /*
                struct sockaddr_in sa;
                char str[INET_ADDRSTRLEN];
                // store this IP address in sa:
                inet_pton(AF_INET, "192.0.2.33", &(sa.sin_addr));
                */
               
               uint8_t type = 0x65; // fake-diffle-hellman
               uint16_t hostc_id = 0x0001;
               uint16_t netwc_id = htons(hostc_id); //incoming circuit id 2bytes
               
               int increment = 0;
               memcpy(circuit, &minhdr, sizeof(minhdr));
               increment = sizeof(minhdr);
               memcpy(circuit+increment,&type,sizeof(type));
               increment = increment + sizeof(type);
               memcpy(circuit+increment,&netwc_id,sizeof(netwc_id));
               increment = increment + sizeof(netwc_id);
               memcpy(circuit+increment,firstkey,sizeof(firstkey));
               increment = increment + sizeof(firstkey);
               /*
               ofile.open("stage5.proxy.out",std::fstream::app);
               ofile << "aes_key is: ";
               for(int z = 0; z < 16; z++){
                   uint16_t shade;
                   memset(&shade,0,2);
                   shade = (uint16_t)aes_key[z];
                   ofile << hex << shade;
               }
               ofile << endl;
               ofile.close();
               */
               ofile.open("stage5.proxy.out",std::fstream::app);
               ofile << "new-fake-diffie-hellman, router index: 1, circuit outgoing: 0x01, key: 0x";
               for(int z = 0; z < 16; z++){
                   uint16_t shade;
                   memset(&shade,0,2);
                   shade = (uint16_t)firstkey[z];
                   if((shade < 0x000f)){
                       ofile << "0";
                   }
                   if(shade > 0x00ff){
                       shade = shade && 0x00ff;
                   }
                   ofile << hex << shade; //rk1[z];
                   
               }
               ofile << endl;
               ofile.close();
               
               
                sendto(sockfd, circuit, increment, 0, (struct sockaddr *)&their_addr1, sizeof(their_addr1));
               
               
               type = 0x62; // request type 1 byte
               hostc_id = 0x0001;
               netwc_id = htons(hostc_id); //incoming circuit id 2bytes
               uint16_t netwport;
               if(temphops == 1){
                   netwport = 0xffff;
               }
               else if(temphops !=1){
                   if(next_router[1] == 1){
                       netwport = their_addr1.sin_port;
                   }
                   else if(next_router[1] == 2){
                       netwport = their_addr2.sin_port;
                   }
                   else if(next_router[1] == 3){
                       netwport = their_addr3.sin_port;
                   }
                   else if(next_router[1] == 4){
                       netwport = their_addr4.sin_port;
                   }
                   else if(next_router[1] == 5){
                       netwport = their_addr5.sin_port;
                   }
                   else if(next_router[1] == 6){
                       netwport = their_addr6.sin_port;
                   }
               }
               

               AES_KEY enc_key;
               int clear_text_len = 2; //2 bytes for netwport
               unsigned char testd[2];
               testd[1] = netwport & 0x00ff; //lower bits
               netwport = netwport/256;
               testd[0] = netwport & 0x00ff; //upper bits
               unsigned char *crypt_text;
               int crypt_text_len;

               class_AES_set_encrypt_key(firstkey, &enc_key);
               class_AES_encrypt_with_padding(testd, clear_text_len, &crypt_text, &crypt_text_len, &enc_key);
               
               
               memset(circuit, 0, sizeof(circuit));

               increment = 0;
               memcpy(circuit, &minhdr, sizeof(minhdr));
               increment = sizeof(minhdr);
               memcpy(circuit+increment,&type,sizeof(type));
               increment = increment + sizeof(type);
               memcpy(circuit+increment,&netwc_id,sizeof(netwc_id));
               increment = increment + sizeof(netwc_id);
               memcpy(circuit+increment,crypt_text,crypt_text_len);
               increment = increment + crypt_text_len;
               
               /*
               ofile.open("stage2.proxy.out",std::fstream::app);
               
               ofile << setw(2) << setfill('0') << hex << int(circuit[0]) << endl;
               ofile << setw(2) << setfill('0') << hex << int(circuit[1]) << endl;
               ofile << setw(2) << setfill('0') << hex << int(circuit[2]) << endl;
               ofile << setw(2) << setfill('0') << hex << int(circuit[3]) << endl;
               ofile << setw(2) << setfill('0') << hex << int(circuit[4]) << endl;
               ofile.close();
               */
               
                /*
               ofile.open("stage2.proxy.out",std::fstream::app);
               ofile << "We sent this message from proxy: ";
               int daf = strlen(circuit);

               for(int i = 0; i < 5; i++){
                   ofile << hex << int(circuit[i]) << endl;  //maybe in network order
               }
                */
               /*
               ofile << endl;
               ofile << "strlen of minitorbuf is: " << daf << endl;
               
               ofile.close();
               */
               sendto(sockfd, circuit, increment, 0, (struct sockaddr *)&their_addr1, sizeof(their_addr1));
           }
           else if((direction == 2) && (temphops)){
               int nrouter;
               ofile.open("stage5.proxy.out",std::fstream::app);
               for(int hop_t = 0; hop_t < hops; hop_t++){
                   nrouter = next_router[hop_t];
                   ofile << "hop: " << hop_t+1 << ", router: " << nrouter << endl;
               }
               ofile.close();
               
               
               char circuit[2048];
               memset(circuit, 0, sizeof(circuit));
               
               struct ip minhdr;
               memset(&minhdr, 0, sizeof(minhdr));
               
               inet_pton(AF_INET, "127.0.0.1", &(minhdr.ip_src)); //stores minitor IP in_addr
               inet_pton(AF_INET, "127.0.0.1", &(minhdr.ip_dst));
               
               minhdr.ip_p = 253;
               /*
                struct sockaddr_in sa;
                char str[INET_ADDRSTRLEN];
                // store this IP address in sa:
                inet_pton(AF_INET, "192.0.2.33", &(sa.sin_addr));
                */
               
               uint8_t type = 0x65; // fake-diffle-hellman
               uint16_t hostc_id = 0x0001;
               uint16_t netwc_id = htons(hostc_id); //incoming circuit id 2bytes
               
               int increment = 0;
               memcpy(circuit, &minhdr, sizeof(minhdr));
               increment = sizeof(minhdr);
               memcpy(circuit+increment,&type,sizeof(type));
               increment = increment + sizeof(type);
               memcpy(circuit+increment,&netwc_id,sizeof(netwc_id));
               increment = increment + sizeof(netwc_id);
               memcpy(circuit+increment,firstkey,sizeof(firstkey));
               increment = increment + sizeof(firstkey);
               /*
               ofile.open("stage5.proxy.out",std::fstream::app);
               ofile << "aes_key is: ";
               for(int z = 0; z < 16; z++){
                   uint16_t shade;
                   memset(&shade,0,2);
                   shade = (uint16_t)aes_key[z];
                   ofile << hex << shade;
               }
               ofile << endl;
               ofile.close();
               */
               ofile.open("stage5.proxy.out",std::fstream::app);
               ofile << "new-fake-diffie-hellman, router index: 2, circuit outgoing: 0x01, key: 0x";
               
               for(int z = 0; z < 16; z++){
                   uint16_t shade;
                   memset(&shade,0,2);
                   shade = (uint16_t)firstkey[z];
                   if((shade < 0x000f)){
                       ofile << "0";
                   }
                   if(shade > 0x00ff){
                       shade = shade && 0x00ff;
                   }
                   ofile << hex << shade; //rk1[z];

               }
               ofile << endl;
               ofile.close();
               
               
               sendto(sockfd, circuit, increment, 0, (struct sockaddr *)&their_addr2, sizeof(their_addr2));
               
               
               type = 0x62; // request type 1 byte
               hostc_id = 0x0001;
               netwc_id = htons(hostc_id); //incoming circuit id 2bytes
               uint16_t netwport;
               if(temphops == 1){
                   netwport = 0xffff;
               }
               else if(temphops !=1){
                   if(next_router[1] == 1){
                       netwport = their_addr1.sin_port;
                   }
                   else if(next_router[1] == 2){
                       netwport = their_addr2.sin_port;
                   }
                   else if(next_router[1] == 3){
                       netwport = their_addr3.sin_port;
                   }
                   else if(next_router[1] == 4){
                       netwport = their_addr4.sin_port;
                   }
                   else if(next_router[1] == 5){
                       netwport = their_addr5.sin_port;
                   }
                   else if(next_router[1] == 6){
                       netwport = their_addr6.sin_port;
                   }
               }
               
               
               AES_KEY enc_key;
               int clear_text_len = 2; //2 bytes for netwport
               unsigned char testd[2];
               testd[1] = netwport & 0x00ff; //lower bits
               netwport = netwport/256;
               testd[0] = netwport & 0x00ff; //upper bits
               unsigned char *crypt_text;
               int crypt_text_len;
               
               class_AES_set_encrypt_key(firstkey, &enc_key);
               class_AES_encrypt_with_padding(testd, clear_text_len, &crypt_text, &crypt_text_len, &enc_key);
               
               
               memset(circuit, 0, sizeof(circuit));
               
               increment = 0;
               memcpy(circuit, &minhdr, sizeof(minhdr));
               increment = sizeof(minhdr);
               memcpy(circuit+increment,&type,sizeof(type));
               increment = increment + sizeof(type);
               memcpy(circuit+increment,&netwc_id,sizeof(netwc_id));
               increment = increment + sizeof(netwc_id);
               memcpy(circuit+increment,crypt_text,crypt_text_len);
               increment = increment + crypt_text_len;
               
               /*
                ofile.open("stage2.proxy.out",std::fstream::app);
                
                ofile << setw(2) << setfill('0') << hex << int(circuit[0]) << endl;
                ofile << setw(2) << setfill('0') << hex << int(circuit[1]) << endl;
                ofile << setw(2) << setfill('0') << hex << int(circuit[2]) << endl;
                ofile << setw(2) << setfill('0') << hex << int(circuit[3]) << endl;
                ofile << setw(2) << setfill('0') << hex << int(circuit[4]) << endl;
                ofile.close();
                */
               
               /*
                ofile.open("stage2.proxy.out",std::fstream::app);
                ofile << "We sent this message from proxy: ";
                int daf = strlen(circuit);
                
                for(int i = 0; i < 5; i++){
                ofile << hex << int(circuit[i]) << endl;  //maybe in network order
                }
                */
               /*
                ofile << endl;
                ofile << "strlen of minitorbuf is: " << daf << endl;
                
                ofile.close();
                */
               sendto(sockfd, circuit, increment, 0, (struct sockaddr *)&their_addr2, sizeof(their_addr2));
           }
           else if((direction == 3) && (temphops)){
               int nrouter;
               ofile.open("stage5.proxy.out",std::fstream::app);
               for(int hop_t = 0; hop_t < hops; hop_t++){
                   nrouter = next_router[hop_t];
                   ofile << "hop: " << hop_t+1 << ", router: " << nrouter << endl;
               }
               ofile.close();
               
               
               char circuit[2048];
               memset(circuit, 0, sizeof(circuit));
               
               struct ip minhdr;
               memset(&minhdr, 0, sizeof(minhdr));
               
               inet_pton(AF_INET, "127.0.0.1", &(minhdr.ip_src)); //stores minitor IP in_addr
               inet_pton(AF_INET, "127.0.0.1", &(minhdr.ip_dst));
               
               minhdr.ip_p = 253;
               /*
                struct sockaddr_in sa;
                char str[INET_ADDRSTRLEN];
                // store this IP address in sa:
                inet_pton(AF_INET, "192.0.2.33", &(sa.sin_addr));
                */
               
               uint8_t type = 0x65; // fake-diffle-hellman
               uint16_t hostc_id = 0x0001;
               uint16_t netwc_id = htons(hostc_id); //incoming circuit id 2bytes
               
               int increment = 0;
               memcpy(circuit, &minhdr, sizeof(minhdr));
               increment = sizeof(minhdr);
               memcpy(circuit+increment,&type,sizeof(type));
               increment = increment + sizeof(type);
               memcpy(circuit+increment,&netwc_id,sizeof(netwc_id));
               increment = increment + sizeof(netwc_id);
               memcpy(circuit+increment,firstkey,sizeof(firstkey));
               increment = increment + sizeof(firstkey);
               /*
               ofile.open("stage5.proxy.out",std::fstream::app);
               ofile << "aes_key is: ";
               for(int z = 0; z < 16; z++){
                   uint16_t shade;
                   memset(&shade,0,2);
                   shade = (uint16_t)aes_key[z];
                   ofile << hex << shade;
               }
               ofile << endl;
               ofile.close();
               */
               ofile.open("stage5.proxy.out",std::fstream::app);
               ofile << "new-fake-diffie-hellman, router index: 3, circuit outgoing: 0x01, key: 0x";
               for(int z = 0; z < 16; z++){
                   uint16_t shade;
                   memset(&shade,0,2);
                   shade = (uint16_t)firstkey[z];
                   if((shade < 0x000f)){
                       ofile << "0";
                   }
                   if(shade > 0x00ff){
                       shade = shade && 0x00ff;
                   }
                   ofile << hex << shade; //rk1[z];
                   
               }
               ofile << endl;
               ofile.close();
               
               
               sendto(sockfd, circuit, increment, 0, (struct sockaddr *)&their_addr3, sizeof(their_addr3));
               
               
               type = 0x62; // request type 1 byte
               hostc_id = 0x0001;
               netwc_id = htons(hostc_id); //incoming circuit id 2bytes
               uint16_t netwport;
               if(temphops == 1){
                   netwport = 0xffff;
               }
               else if(temphops !=1){
                   if(next_router[1] == 1){
                       netwport = their_addr1.sin_port;
                   }
                   else if(next_router[1] == 2){
                       netwport = their_addr2.sin_port;
                   }
                   else if(next_router[1] == 3){
                       netwport = their_addr3.sin_port;
                   }
                   else if(next_router[1] == 4){
                       netwport = their_addr4.sin_port;
                   }
                   else if(next_router[1] == 5){
                       netwport = their_addr5.sin_port;
                   }
                   else if(next_router[1] == 6){
                       netwport = their_addr6.sin_port;
                   }
               }
               
               
               AES_KEY enc_key;
               int clear_text_len = 2; //2 bytes for netwport
               unsigned char testd[2];
               testd[1] = netwport & 0x00ff; //lower bits
               netwport = netwport/256;
               testd[0] = netwport & 0x00ff; //upper bits
               unsigned char *crypt_text;
               int crypt_text_len;
               
               class_AES_set_encrypt_key(firstkey, &enc_key);
               class_AES_encrypt_with_padding(testd, clear_text_len, &crypt_text, &crypt_text_len, &enc_key);
               
               
               memset(circuit, 0, sizeof(circuit));
               
               increment = 0;
               memcpy(circuit, &minhdr, sizeof(minhdr));
               increment = sizeof(minhdr);
               memcpy(circuit+increment,&type,sizeof(type));
               increment = increment + sizeof(type);
               memcpy(circuit+increment,&netwc_id,sizeof(netwc_id));
               increment = increment + sizeof(netwc_id);
               memcpy(circuit+increment,crypt_text,crypt_text_len);
               increment = increment + crypt_text_len;
               
               /*
                ofile.open("stage2.proxy.out",std::fstream::app);
                
                ofile << setw(2) << setfill('0') << hex << int(circuit[0]) << endl;
                ofile << setw(2) << setfill('0') << hex << int(circuit[1]) << endl;
                ofile << setw(2) << setfill('0') << hex << int(circuit[2]) << endl;
                ofile << setw(2) << setfill('0') << hex << int(circuit[3]) << endl;
                ofile << setw(2) << setfill('0') << hex << int(circuit[4]) << endl;
                ofile.close();
                */
               
               /*
                ofile.open("stage2.proxy.out",std::fstream::app);
                ofile << "We sent this message from proxy: ";
                int daf = strlen(circuit);
                
                for(int i = 0; i < 5; i++){
                ofile << hex << int(circuit[i]) << endl;  //maybe in network order
                }
                */
               /*
                ofile << endl;
                ofile << "strlen of minitorbuf is: " << daf << endl;
                
                ofile.close();
                */
               sendto(sockfd, circuit, increment, 0, (struct sockaddr *)&their_addr3, sizeof(their_addr3));
           }
           else if((direction == 4) && (temphops)){
               int nrouter;
               ofile.open("stage5.proxy.out",std::fstream::app);
               for(int hop_t = 0; hop_t < hops; hop_t++){
                   nrouter = next_router[hop_t];
                   ofile << "hop: " << hop_t+1 << ", router: " << nrouter << endl;
               }
               ofile.close();
               
               
               char circuit[2048];
               memset(circuit, 0, sizeof(circuit));
               
               struct ip minhdr;
               memset(&minhdr, 0, sizeof(minhdr));
               
               inet_pton(AF_INET, "127.0.0.1", &(minhdr.ip_src)); //stores minitor IP in_addr
               inet_pton(AF_INET, "127.0.0.1", &(minhdr.ip_dst));
               
               minhdr.ip_p = 253;
               /*
                struct sockaddr_in sa;
                char str[INET_ADDRSTRLEN];
                // store this IP address in sa:
                inet_pton(AF_INET, "192.0.2.33", &(sa.sin_addr));
                */
               
               uint8_t type = 0x65; // fake-diffle-hellman
               uint16_t hostc_id = 0x0001;
               uint16_t netwc_id = htons(hostc_id); //incoming circuit id 2bytes
               
               int increment = 0;
               memcpy(circuit, &minhdr, sizeof(minhdr));
               increment = sizeof(minhdr);
               memcpy(circuit+increment,&type,sizeof(type));
               increment = increment + sizeof(type);
               memcpy(circuit+increment,&netwc_id,sizeof(netwc_id));
               increment = increment + sizeof(netwc_id);
               memcpy(circuit+increment,firstkey,sizeof(firstkey));
               increment = increment + sizeof(firstkey);
               /*
               ofile.open("stage5.proxy.out",std::fstream::app);
               ofile << "aes_key is: ";
               for(int z = 0; z < 16; z++){
                   uint16_t shade;
                   memset(&shade,0,2);
                   shade = (uint16_t)aes_key[z];
                   ofile << hex << shade;
               }
               ofile << endl;
               ofile.close();
               */
               ofile.open("stage5.proxy.out",std::fstream::app);
               ofile << "new-fake-diffie-hellman, router index: 4, circuit outgoing: 0x01, key: 0x";
               for(int z = 0; z < 16; z++){
                   uint16_t shade;
                   memset(&shade,0,2);
                   shade = (uint16_t)firstkey[z];
                   if((shade < 0x000f)){
                       ofile << "0";
                   }
                   if(shade > 0x00ff){
                       shade = shade && 0x00ff;
                   }
                   ofile << hex << shade; //rk1[z];
                   
               }
               ofile << endl;
               ofile.close();
               
               
               sendto(sockfd, circuit, increment, 0, (struct sockaddr *)&their_addr4, sizeof(their_addr4));
               
               
               type = 0x62; // request type 1 byte
               hostc_id = 0x0001;
               netwc_id = htons(hostc_id); //incoming circuit id 2bytes
               uint16_t netwport;
               if(temphops == 1){
                   netwport = 0xffff;
               }
               else if(temphops !=1){
                   if(next_router[1] == 1){
                       netwport = their_addr1.sin_port;
                   }
                   else if(next_router[1] == 2){
                       netwport = their_addr2.sin_port;
                   }
                   else if(next_router[1] == 3){
                       netwport = their_addr3.sin_port;
                   }
                   else if(next_router[1] == 4){
                       netwport = their_addr4.sin_port;
                   }
                   else if(next_router[1] == 5){
                       netwport = their_addr5.sin_port;
                   }
                   else if(next_router[1] == 6){
                       netwport = their_addr6.sin_port;
                   }
               }
               
               
               AES_KEY enc_key;
               int clear_text_len = 2; //2 bytes for netwport
               unsigned char testd[2];
               testd[1] = netwport & 0x00ff; //lower bits
               netwport = netwport/256;
               testd[0] = netwport & 0x00ff; //upper bits
               unsigned char *crypt_text;
               int crypt_text_len;
               
               class_AES_set_encrypt_key(firstkey, &enc_key);
               class_AES_encrypt_with_padding(testd, clear_text_len, &crypt_text, &crypt_text_len, &enc_key);
               
               
               memset(circuit, 0, sizeof(circuit));
               
               increment = 0;
               memcpy(circuit, &minhdr, sizeof(minhdr));
               increment = sizeof(minhdr);
               memcpy(circuit+increment,&type,sizeof(type));
               increment = increment + sizeof(type);
               memcpy(circuit+increment,&netwc_id,sizeof(netwc_id));
               increment = increment + sizeof(netwc_id);
               memcpy(circuit+increment,crypt_text,crypt_text_len);
               increment = increment + crypt_text_len;
               
               /*
                ofile.open("stage2.proxy.out",std::fstream::app);
                
                ofile << setw(2) << setfill('0') << hex << int(circuit[0]) << endl;
                ofile << setw(2) << setfill('0') << hex << int(circuit[1]) << endl;
                ofile << setw(2) << setfill('0') << hex << int(circuit[2]) << endl;
                ofile << setw(2) << setfill('0') << hex << int(circuit[3]) << endl;
                ofile << setw(2) << setfill('0') << hex << int(circuit[4]) << endl;
                ofile.close();
                */
               
               /*
                ofile.open("stage2.proxy.out",std::fstream::app);
                ofile << "We sent this message from proxy: ";
                int daf = strlen(circuit);
                
                for(int i = 0; i < 5; i++){
                ofile << hex << int(circuit[i]) << endl;  //maybe in network order
                }
                */
               /*
                ofile << endl;
                ofile << "strlen of minitorbuf is: " << daf << endl;
                
                ofile.close();
                */
               sendto(sockfd, circuit, increment, 0, (struct sockaddr *)&their_addr4, sizeof(their_addr4));
           }
           else if((direction == 5) && (temphops)){
               int nrouter;
               ofile.open("stage5.proxy.out",std::fstream::app);
               for(int hop_t = 0; hop_t < hops; hop_t++){
                   nrouter = next_router[hop_t];
                   ofile << "hop: " << hop_t+1 << ", router: " << nrouter << endl;
               }
               ofile.close();
               
               
               char circuit[2048];
               memset(circuit, 0, sizeof(circuit));
               
               struct ip minhdr;
               memset(&minhdr, 0, sizeof(minhdr));
               
               inet_pton(AF_INET, "127.0.0.1", &(minhdr.ip_src)); //stores minitor IP in_addr
               inet_pton(AF_INET, "127.0.0.1", &(minhdr.ip_dst));
               
               minhdr.ip_p = 253;
               /*
                struct sockaddr_in sa;
                char str[INET_ADDRSTRLEN];
                // store this IP address in sa:
                inet_pton(AF_INET, "192.0.2.33", &(sa.sin_addr));
                */
               
               uint8_t type = 0x65; // fake-diffle-hellman
               uint16_t hostc_id = 0x0001;
               uint16_t netwc_id = htons(hostc_id); //incoming circuit id 2bytes
               
               int increment = 0;
               memcpy(circuit, &minhdr, sizeof(minhdr));
               increment = sizeof(minhdr);
               memcpy(circuit+increment,&type,sizeof(type));
               increment = increment + sizeof(type);
               memcpy(circuit+increment,&netwc_id,sizeof(netwc_id));
               increment = increment + sizeof(netwc_id);
               memcpy(circuit+increment,firstkey,sizeof(firstkey));
               increment = increment + sizeof(firstkey);
               /*
               ofile.open("stage5.proxy.out",std::fstream::app);
               ofile << "aes_key is: ";
               for(int z = 0; z < 16; z++){
                   uint16_t shade;
                   memset(&shade,0,2);
                   shade = (uint16_t)aes_key[z];
                   ofile << hex << shade;
               }
               ofile << endl;
               ofile.close();
               */
               ofile.open("stage5.proxy.out",std::fstream::app);
               ofile << "new-fake-diffie-hellman, router index: 5, circuit outgoing: 0x01, key: 0x";
               for(int z = 0; z < 16; z++){
                   uint16_t shade;
                   memset(&shade,0,2);
                   shade = (uint16_t)firstkey[z];
                   if((shade < 0x000f)){
                       ofile << "0";
                   }
                   if(shade > 0x00ff){
                       shade = shade && 0x00ff;
                   }
                   ofile << hex << shade; //rk1[z];
                   
               }
               ofile << endl;
               ofile.close();
               
               
               sendto(sockfd, circuit, increment, 0, (struct sockaddr *)&their_addr5, sizeof(their_addr5));
               
               
               type = 0x62; // request type 1 byte
               hostc_id = 0x0001;
               netwc_id = htons(hostc_id); //incoming circuit id 2bytes
               uint16_t netwport;
               if(temphops == 1){
                   netwport = 0xffff;
               }
               else if(temphops !=1){
                   if(next_router[1] == 1){
                       netwport = their_addr1.sin_port;
                   }
                   else if(next_router[1] == 2){
                       netwport = their_addr2.sin_port;
                   }
                   else if(next_router[1] == 3){
                       netwport = their_addr3.sin_port;
                   }
                   else if(next_router[1] == 4){
                       netwport = their_addr4.sin_port;
                   }
                   else if(next_router[1] == 5){
                       netwport = their_addr5.sin_port;
                   }
                   else if(next_router[1] == 6){
                       netwport = their_addr6.sin_port;
                   }
               }
               
               
               AES_KEY enc_key;
               int clear_text_len = 2; //2 bytes for netwport
               unsigned char testd[2];
               testd[1] = netwport & 0x00ff; //lower bits
               netwport = netwport/256;
               testd[0] = netwport & 0x00ff; //upper bits
               unsigned char *crypt_text;
               int crypt_text_len;
               
               class_AES_set_encrypt_key(firstkey, &enc_key);
               class_AES_encrypt_with_padding(testd, clear_text_len, &crypt_text, &crypt_text_len, &enc_key);
               
               
               memset(circuit, 0, sizeof(circuit));
               
               increment = 0;
               memcpy(circuit, &minhdr, sizeof(minhdr));
               increment = sizeof(minhdr);
               memcpy(circuit+increment,&type,sizeof(type));
               increment = increment + sizeof(type);
               memcpy(circuit+increment,&netwc_id,sizeof(netwc_id));
               increment = increment + sizeof(netwc_id);
               memcpy(circuit+increment,crypt_text,crypt_text_len);
               increment = increment + crypt_text_len;
               
               /*
                ofile.open("stage2.proxy.out",std::fstream::app);
                
                ofile << setw(2) << setfill('0') << hex << int(circuit[0]) << endl;
                ofile << setw(2) << setfill('0') << hex << int(circuit[1]) << endl;
                ofile << setw(2) << setfill('0') << hex << int(circuit[2]) << endl;
                ofile << setw(2) << setfill('0') << hex << int(circuit[3]) << endl;
                ofile << setw(2) << setfill('0') << hex << int(circuit[4]) << endl;
                ofile.close();
                */
               
               /*
                ofile.open("stage2.proxy.out",std::fstream::app);
                ofile << "We sent this message from proxy: ";
                int daf = strlen(circuit);
                
                for(int i = 0; i < 5; i++){
                ofile << hex << int(circuit[i]) << endl;  //maybe in network order
                }
                */
               /*
                ofile << endl;
                ofile << "strlen of minitorbuf is: " << daf << endl;
                
                ofile.close();
                */
               sendto(sockfd, circuit, increment, 0, (struct sockaddr *)&their_addr5, sizeof(their_addr5));
           }
           else if((direction == 6) && (temphops)){
               int nrouter;
               ofile.open("stage5.proxy.out",std::fstream::app);
               for(int hop_t = 0; hop_t < hops; hop_t++){
                   nrouter = next_router[hop_t];
                   ofile << "hop: " << hop_t+1 << ", router: " << nrouter << endl;
               }
               ofile.close();
               
               
               char circuit[2048];
               memset(circuit, 0, sizeof(circuit));
               
               struct ip minhdr;
               memset(&minhdr, 0, sizeof(minhdr));
               
               inet_pton(AF_INET, "127.0.0.1", &(minhdr.ip_src)); //stores minitor IP in_addr
               inet_pton(AF_INET, "127.0.0.1", &(minhdr.ip_dst));
               
               minhdr.ip_p = 253;
               /*
                struct sockaddr_in sa;
                char str[INET_ADDRSTRLEN];
                // store this IP address in sa:
                inet_pton(AF_INET, "192.0.2.33", &(sa.sin_addr));
                */
               
               uint8_t type = 0x65; // fake-diffle-hellman
               uint16_t hostc_id = 0x0001;
               uint16_t netwc_id = htons(hostc_id); //incoming circuit id 2bytes
               
               int increment = 0;
               memcpy(circuit, &minhdr, sizeof(minhdr));
               increment = sizeof(minhdr);
               memcpy(circuit+increment,&type,sizeof(type));
               increment = increment + sizeof(type);
               memcpy(circuit+increment,&netwc_id,sizeof(netwc_id));
               increment = increment + sizeof(netwc_id);
               memcpy(circuit+increment,firstkey,sizeof(firstkey));
               increment = increment + sizeof(firstkey);
               /*
               ofile.open("stage5.proxy.out",std::fstream::app);
               ofile << "aes_key is: ";
               for(int z = 0; z < 16; z++){
                   uint16_t shade;
                   memset(&shade,0,2);
                   shade = (uint16_t)aes_key[z];
                   ofile << hex << shade;
               }
               ofile << endl;
               ofile.close();
               */
               ofile.open("stage5.proxy.out",std::fstream::app);
               ofile << "new-fake-diffie-hellman, router index: 6, circuit outgoing: 0x01, key: 0x";
               for(int z = 0; z < 16; z++){
                   uint16_t shade;
                   memset(&shade,0,2);
                   shade = (uint16_t)firstkey[z];
                   if((shade < 0x000f)){
                       ofile << "0";
                   }
                   if(shade > 0x00ff){
                       shade = shade && 0x00ff;
                   }
                   ofile << hex << shade; //rk1[z];
                   
               }
               ofile << endl;
               ofile.close();
               
               
               sendto(sockfd, circuit, increment, 0, (struct sockaddr *)&their_addr6, sizeof(their_addr6));
               
               
               type = 0x62; // request type 1 byte
               hostc_id = 0x0001;
               netwc_id = htons(hostc_id); //incoming circuit id 2bytes
               uint16_t netwport;
               if(temphops == 1){
                   netwport = 0xffff;
               }
               else if(temphops !=1){
                   if(next_router[1] == 1){
                       netwport = their_addr1.sin_port;
                   }
                   else if(next_router[1] == 2){
                       netwport = their_addr2.sin_port;
                   }
                   else if(next_router[1] == 3){
                       netwport = their_addr3.sin_port;
                   }
                   else if(next_router[1] == 4){
                       netwport = their_addr4.sin_port;
                   }
                   else if(next_router[1] == 5){
                       netwport = their_addr5.sin_port;
                   }
                   else if(next_router[1] == 6){
                       netwport = their_addr6.sin_port;
                   }
               }
               
               
               AES_KEY enc_key;
               int clear_text_len = 2; //2 bytes for netwport
               unsigned char testd[2];
               testd[1] = netwport & 0x00ff; //lower bits
               netwport = netwport/256;
               testd[0] = netwport & 0x00ff; //upper bits
               unsigned char *crypt_text;
               int crypt_text_len;
               
               class_AES_set_encrypt_key(firstkey, &enc_key);
               class_AES_encrypt_with_padding(testd, clear_text_len, &crypt_text, &crypt_text_len, &enc_key);
               
               
               memset(circuit, 0, sizeof(circuit));
               
               increment = 0;
               memcpy(circuit, &minhdr, sizeof(minhdr));
               increment = sizeof(minhdr);
               memcpy(circuit+increment,&type,sizeof(type));
               increment = increment + sizeof(type);
               memcpy(circuit+increment,&netwc_id,sizeof(netwc_id));
               increment = increment + sizeof(netwc_id);
               memcpy(circuit+increment,crypt_text,crypt_text_len);
               increment = increment + crypt_text_len;
               
               /*
                ofile.open("stage2.proxy.out",std::fstream::app);
                
                ofile << setw(2) << setfill('0') << hex << int(circuit[0]) << endl;
                ofile << setw(2) << setfill('0') << hex << int(circuit[1]) << endl;
                ofile << setw(2) << setfill('0') << hex << int(circuit[2]) << endl;
                ofile << setw(2) << setfill('0') << hex << int(circuit[3]) << endl;
                ofile << setw(2) << setfill('0') << hex << int(circuit[4]) << endl;
                ofile.close();
                */
               
               /*
                ofile.open("stage2.proxy.out",std::fstream::app);
                ofile << "We sent this message from proxy: ";
                int daf = strlen(circuit);
                
                for(int i = 0; i < 5; i++){
                ofile << hex << int(circuit[i]) << endl;  //maybe in network order
                }
                */
               /*
                ofile << endl;
                ofile << "strlen of minitorbuf is: " << daf << endl;
                
                ofile.close();
                */
               sendto(sockfd, circuit, increment, 0, (struct sockaddr *)&their_addr6, sizeof(their_addr6));
           }
           
    //sent initial request message
           

           char incoming[2048];
           int mbytes;
           int nexthop = 2; // the 3rd element of the next_router deq
           
           cout << "finished 1 hop encryption" <<endl;
           
           while(temphops){ // recieve circuit extend messages and send out another control message to that same 1st hop router and breaks when NEXT-NAME is 0xfff
               temphops = temphops -1;
               
               mbytes = recvfrom(sockfd, incoming, 2048, 0, (struct sockaddr *)&their_addr, &addr_len);
               
               if(mbytes < 0){
                   cout << "error in router up message" << endl;
               }
               
               int length = mbytes - sizeof(struct ip);

               
               struct ip * IP;
               struct in_addr src;
               struct in_addr dest;
               char IP_src[NI_MAXHOST];
               char IP_dest[NI_MAXHOST];
               
               IP = (struct ip *)incoming;
               src = IP->ip_src;
               inet_ntop(AF_INET, &src, IP_src, sizeof(IP_src));
               
               dest = IP->ip_dst;
               inet_ntop(AF_INET, &dest, IP_dest, sizeof(IP_dest));
               

               if((strcmp(IP_src,"127.0.0.1") == 0) && (strcmp(IP_dest, "127.0.0.1") == 0) && (IP->ip_p == 253)){
                   
                   char minitorbuf[2048];
                   memset(minitorbuf, 0, sizeof(minitorbuf));
                   memcpy(minitorbuf,incoming+sizeof(struct ip),3);
                   
                   ofile.open("stage5.proxy.out",std::fstream::app);
                   
                   ofile << "pkt from port: " << dec << ntohs(their_addr.sin_port) << ", length: " << length << ", contents: 0x";
                   
                   
                   for(int i = 0; i < length; i++){
                       ofile << setw(2) << setfill('0') << hex << int(minitorbuf[i]); //maybe in network order
                   }
                   
                   ofile << endl;
                   
                   ofile.close();
                   
                   
               if (minitorbuf[0] == 0x63) { //circuit-extend done type
                   ofile.open("stage5.proxy.out",std::fstream::app);

                   ofile << "incoming extend-done circuit, incoming: 0x";
                   
                   uint16_t cid1 = (uint16_t)minitorbuf[1]; //upper
                   uint16_t cid2 = (uint16_t)minitorbuf[2]; //lower
                   cid1 = cid1 & 0x00ff;   //chars are only bytes so make sure
                   cid2 = cid2 & 0x00ff;
                   cid1 = cid1*256;
                   cid1 = cid1 + cid2;
                   
                   
                   ofile << setw(2) << setfill('0') << hex << cid1;
                   ofile << " from port: " << dec << ntohs(their_addr.sin_port) << endl;
                   
                   ofile.close();

               }

               //breakdown what they sent and send another request if number hops is not exhausted
               if(temphops){
                   cout << "enters temphops" <<endl;

                   char circuit[2048];
                   memset(circuit, 0, sizeof(circuit));
                   
                   struct ip minhdr;
                   memset(&minhdr, 0, sizeof(minhdr));
                   
                   inet_pton(AF_INET, "127.0.0.1", &(minhdr.ip_src)); //stores minitor IP in_addr
                   inet_pton(AF_INET, "127.0.0.1", &(minhdr.ip_dst));
                   
                   minhdr.ip_p = 253;
                   
 
                   //type = 0x62; // request type 1 byte
                   //hostc_id = 0x0001;
                   //netwc_id = htons(hostc_id); //incoming circuit id 2bytes
                   uint16_t netwport;
                   if(temphops == 1){
                       netwport = 0xffff;
                   }
                   else if(temphops !=1){
                       if(next_router[nexthop] == 1){
                           netwport = their_addr1.sin_port;
                       }
                       else if(next_router[nexthop] == 2){
                           netwport = their_addr2.sin_port;
                       }
                       else if(next_router[nexthop] == 3){
                           netwport = their_addr3.sin_port;
                       }
                       else if(next_router[nexthop] == 4){
                           netwport = their_addr4.sin_port;
                       }
                       else if(next_router[nexthop] == 5){
                           netwport = their_addr5.sin_port;
                       }
                       else if(next_router[nexthop] == 6){
                           netwport = their_addr6.sin_port;
                       }
                   }
                   unsigned char keysent[16];
                   memset(keysent, 0, 16);
                   memcpy(keysent,keyorder[nexthop-1],16);
                   uint8_t type = 0x65; // fake-diffle-hellman
                   uint16_t hostc_id = 0x0001;
                   uint16_t netwc_id = htons(hostc_id); //incoming circuit id 2bytes
                   
                   int increment = 0;
                   memcpy(circuit, &minhdr, sizeof(minhdr));
                   increment = sizeof(minhdr);
                   memcpy(circuit+increment,&type,sizeof(type));
                   increment = increment + sizeof(type);
                   memcpy(circuit+increment,&netwc_id,sizeof(netwc_id));
                   increment = increment + sizeof(netwc_id);
                   
                   //recursion here recursion 1 works!
                   //encrypt_multiround_with_padding(char* keysent, int inlen =16, char**  etext, int* outlen,  int nexthop)
                   //{
                       //int inlen =16;
                       //char**  etext = NULL;
                       //int* outlen = 0;

                       char * inbuf = new char [16];
                       memset(inbuf, 0, 16);
                       memcpy(inbuf, keysent, 16); //clear text is in data initially length 16
                   
                       int ilen = 16;
                       int olen = 0;
                       char * outbuf=NULL;

                       for(int i = (nexthop-2); i >= 0; i--)
                       {
                           
                           unsigned char nextkey[16];
                           memset(nextkey,0,16);
                           memcpy(nextkey,keyorder[i],16); //storying encryption key in nextkey
                           /*
                           ofile.open("stage5.proxy.out",std::fstream::app);
                           ofile << "encrypted with key: ";
                           
                           for(int z = 0; z < 16; z++){
                               uint16_t shade;
                               memset(&shade,0,2);
                               shade = (uint16_t)nextkey[z];
                               if((shade < 0x000f)){
                                   ofile << "0";
                               }
                               if(shade > 0x00ff){
                                   shade = shade && 0x00ff;
                               }
                               ofile << hex << shade; //rk1[z];
                               
                           }
                           ofile << endl;
                           ofile.close();
                           */
                           
                           
                           AES_KEY enc_key;
                           class_AES_set_encrypt_key(nextkey, (AES_KEY *)&enc_key); //successfully creating AES_KEY from nextkey
                           class_AES_encrypt_with_padding((unsigned char*)inbuf, ilen,(unsigned char**) &outbuf, &olen, (AES_KEY *)&enc_key); //clear text inbuf

                           //remember the new length
                           ilen = olen;
                           delete [] inbuf;
                           
                           inbuf = new char [ilen];
                           memset(inbuf, 0, ilen);
                           memcpy(inbuf, outbuf, ilen);
                           delete [] outbuf;
                       }
                   cout << "right before sending onion" << endl;
                   //cout << "olen is" << olen << endl;

                       //*etext = inbuf;
                       //*outlen = olen;
                   //}              }
                   //recursion here
                   cout << "ilen is " << ilen << endl;
                   cout << "length of encrypted text is " << strlen(inbuf) << endl;
                   memcpy(circuit+increment,inbuf,ilen);
                   increment = increment + ilen;
                   
                   /*
                    unsigned char nextkey[16];
                   memset(nextkey,0,16);
                   memcpy(nextkey,keyorder[0],16); //storying encryption key in nextkey

                   
                   AES_KEY enc_key;
                   int clear_text_len = 2; //2 bytes for netwport
                   unsigned char testda[2];
                   testda[1] = netwport & 0x00ff; //lower bits
                   netwport = netwport/256;
                   testda[0] = netwport & 0x00ff; //upper bits
                   unsigned char *crypt_text;
                   int crypt_text_len;
                   
                   class_AES_set_encrypt_key(nextkey, &enc_key);
                   class_AES_encrypt_with_padding(testda, clear_text_len, &crypt_text, &crypt_text_len, &enc_key);
                   
                   
                   memset(circuit, 0, sizeof(circuit));
                   
                   increment = 0;
                   memcpy(circuit, &minhdr, sizeof(minhdr));
                   increment = sizeof(minhdr);
                   memcpy(circuit+increment,&type,sizeof(type));
                   increment = increment + sizeof(type);
                   memcpy(circuit+increment,&netwc_id,sizeof(netwc_id));
                   increment = increment + sizeof(netwc_id);
                   memcpy(circuit+increment,crypt_text,crypt_text_len);
                   increment = increment + crypt_text_len;
                   */
                   
                   cout << "right before sending onion" << endl;
                   /*
                   ofile.open("stage5.proxy.out",std::fstream::app);
                   ofile << "aes_key is: ";
                   for(int z = 0; z < 16; z++){
                       uint16_t shade;
                       memset(&shade,0,2);
                       shade = (uint16_t)aes_key[z];
                       ofile << hex << shade;
                   }
                   ofile << endl;
                   ofile.close();
                    */
                   cout << "writing to proxy" << endl;

                   ofile.open("stage5.proxy.out",std::fstream::app);
                   ofile << "new-fake-diffie-hellman, router index: ";
                   ofile << next_router[nexthop-1];
                   ofile << ", circuit outgoing: 0x01, key: 0x";
                   
                   for(int z = 0; z < 16; z++){
                       uint16_t shade;
                       memset(&shade,0,2);
                       shade = (uint16_t)keysent[z];
                       if((shade < 0x000f)){
                           ofile << "0";
                       }
                       if(shade > 0x00ff){
                           shade = shade && 0x00ff;
                       }
                       ofile << hex << shade; //rk1[z];
                       
                   }
                   ofile << endl;
                   ofile.close();
                   unsigned char getprint[16];
                   memset(getprint,0,16);
                   memcpy(getprint,keyorder[0],16);
                   /*
                   ofile.open("stage5.proxy.out",std::fstream::app);
                   ofile << "first hop key is: ";
                   
                   for(int z = 0; z < 16; z++){
                       uint16_t shade;
                       memset(&shade,0,2);
                       shade = (uint16_t)getprint[z];
                       if((shade < 0x000f)){
                           ofile << "0";
                       }
                       if(shade > 0x00ff){
                           shade = shade && 0x00ff;
                       }
                       ofile << hex << shade; //rk1[z];
                       
                   }
                   ofile << endl;
                   ofile.close();
                    */
                   
                   
                   
                   sendto(sockfd, circuit, increment, 0, (struct sockaddr *)&their_addr, sizeof(their_addr));
                   
                   memset(circuit, 0, sizeof(circuit));
                   increment = 0;
                   type = 0x62; // request type 1 byte
                   hostc_id = 0x0001;
                   netwc_id = htons(hostc_id); //incoming circuit id 2bytes
                   
                   unsigned char testd[2];
                   testd[1] = netwport & 0x00ff; //lower bits
                   netwport = netwport/256;
                   testd[0] = netwport & 0x00ff; //upper bits

                   memcpy(circuit, &minhdr, sizeof(minhdr));
                   increment = sizeof(minhdr);
                   memcpy(circuit+increment,&type,sizeof(type));
                   increment = increment + sizeof(type);
                   memcpy(circuit+increment,&netwc_id,sizeof(netwc_id));
                   increment = increment + sizeof(netwc_id);
                   
                   //recursion 2 encrypt with two keys in reverse order. outer most is the first hop,etc
                   //char *inbuf
                   inbuf = new char [2]; //initially it is 2 bytes for the port number
                   memset(inbuf, 0, 2);
                   memcpy(inbuf, testd, 2); //clear text is in data initially length 16
                   
                   ilen = 2;
                   olen = 0;
                   outbuf=NULL;
                   
                   for(int i = (nexthop-1); i >= 0; i--)
                   {
                       
                       unsigned char nextkey[16];
                       memset(nextkey,0,16);
                       memcpy(nextkey,keyorder[i],16); //storying encryption key in nextkey
                       /*
                       ofile.open("stage5.proxy.out",std::fstream::app);
                       ofile << "encrypted with key: ";
                       
                       for(int z = 0; z < 16; z++){
                           uint16_t shade;
                           memset(&shade,0,2);
                           shade = (uint16_t)nextkey[z];
                           if((shade < 0x000f)){
                               ofile << "0";
                           }
                           if(shade > 0x00ff){
                               shade = shade && 0x00ff;
                           }
                           ofile << hex << shade; //rk1[z];
                           
                       }
                       ofile << endl;
                       ofile.close();
                       */
                       
                       
                       AES_KEY enc_key;
                       class_AES_set_encrypt_key(nextkey, (AES_KEY *)&enc_key); //successfully creating AES_KEY from nextkey
                       class_AES_encrypt_with_padding((unsigned char*)inbuf, ilen,(unsigned char**) &outbuf, &olen, (AES_KEY *)&enc_key); //clear text inbuf
                       
                       //remember the new length
                       ilen = olen;
                       delete [] inbuf;
                       
                       inbuf = new char [ilen];
                       memset(inbuf, 0, ilen);
                       memcpy(inbuf, outbuf, ilen);
                       delete [] outbuf;
                   }
                   
                   cout << "ilen is " << ilen << endl;
                   cout << "length of encrypted text is " << strlen(inbuf) << endl;
                   memcpy(circuit+increment,inbuf,ilen);
                   increment = increment + ilen;
                   
                   sendto(sockfd, circuit, increment, 0, (struct sockaddr *)&their_addr, sizeof(their_addr));
                   nexthop = nexthop + 1;
    
               }
               else if(!temphops){
                   break;
               }
               }//if minitor IP header
           }
               
           
           
           
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
                   else //if(nread >= 60) //cheat method
                   {
                       printf("Read a packet from tunnel, packet length:%d\n", nread);
                       
                       //This is the print out of the ICMP message
                       
                       tun_ICMP = (struct ip *)tun_buf;
                       tun_src = tun_ICMP->ip_src;
                       inet_ntop(AF_INET, &tun_src, tun_ip_src, sizeof(tun_ip_src));
                       
                       tun_dest = tun_ICMP->ip_dst;
                       inet_ntop(AF_INET, &tun_dest, tun_ip_dest, sizeof(tun_ip_dest));
                       
                       
                       hlenlt = tun_ICMP->ip_hl;
                       if(tun_ICMP->ip_p == IPPROTO_ICMP){
                           tun_rICMP = (struct icmp *)(tun_buf + hlenlt*4);
                           tun_rICMP->icmp_type = 8;
                           tun_utype = tun_rICMP->icmp_type;
                       }
                       /*
                        cout << "source in tun_buf is :" << tun_ip_src << endl;
                        cout << "dest in tun_buf is :" << tun_ip_dest << endl;
                        cout << "type is: " << tun_utype << endl;
                        */
                       
                       //stage 7 test
                       /*struct tcphdr *tcp_hdr;
                       tcp_hdr = (struct tcphdr *)(tun_buf + hlenlt*4); // char = 8 so mult by 4
                       u_short original = tcp_hdr->check;

                       tcp_hdr->check = 0;
                       struct pseudo_tcphdr tcppseudo;
                       memset(&tcppseudo, 0, sizeof(tcppseudo));
                       tcppseudo.source = tun_src;
                       tcppseudo.dest = tun_dest;
                       tcppseudo.protocol = IPPROTO_TCP;
                       tcppseudo.length = htons(ntohs(tun_ICMP->ip_len) - hlenlt*4); //ip_len is in bytes, hlen is in 32 bits
                       char pseud_and_hdr[2048];
                       memset(&pseud_and_hdr,0,2048);
                       memcpy(pseud_and_hdr,&tcppseudo,sizeof(tcppseudo));
                       memcpy(pseud_and_hdr+sizeof(tcppseudo),tcp_hdr, ntohs(tun_ICMP->ip_len) - hlenlt*4); // cpy pseudo header and all of tcp into a buffer.
                       //tun_ICMP->ip_len - hlenl*4
                       int total_length = ntohs(tun_ICMP->ip_len) - hlenlt*4 + sizeof(tcppseudo);
                       cout << endl << endl << "sent out raw1g" << endl << endl;
                       //tcp_hdr->check = 0;
                       cout << endl << endl << "sent out raw1h" << endl << endl;
                       tcp_hdr->check = ip_checksum(pseud_and_hdr,total_length);
                       //req.checksum=ip_checksum(&req,8);
                       cout << endl << endl << "sent out raw2" << endl << endl;
                       u_short generated = tcp_hdr->check;
                       
                       cout << endl << endl << "This is original checksum is: " << original << endl << endl;
                       cout << endl << endl << "This is generated checksum is: " << generated << endl << endl;
                         */

                       
                       //stage 7 test
                       if(tun_ICMP->ip_p == IPPROTO_ICMP){
                           ofile.open("stage5.proxy.out",std::fstream::app);

                           ofile << "ICMP from tunnel, src: " << tun_ip_src << ", dst: " << tun_ip_dest << ", type is: ";
                       
                           if(tun_utype == 8){
                               ofile << "8\n";
                           }
                           else{
                               ofile << "\n";
                           }
                                ofile.close();
                       }
                       
                       if(tun_ICMP->ip_p == IPPROTO_TCP){
                           struct tcphdr *tcp_hdr;
                           int doitnow = ntohs(tun_ICMP->ip_len) - hlenlt*4;
                           tcp_hdr = (struct tcphdr *)(tun_buf + hlenlt*4); // char = 8 so mult by 4
                           
                           tcp_checksum(tcp_hdr,doitnow,tun_ICMP);
                           
                           ofile.open("stage5.proxy.out",std::fstream::app);
                           ofile << "TCP from tunnel, src IP/port: " << tun_ip_src << ":" << tcp_hdr->source << ", dst IP/port: " << tun_ip_dest << ":" << tcp_hdr->dest << ", seqno: " << tcp_hdr->seq << ", ackno: " << tcp_hdr->ack_seq << endl;
                           ofile.close();
                       }
                       
                       //stage 6 setting to zero
                       ipreturnchar[proxyid] = tun_ip_src; //store the source ip address as we set it to zero.
                       // Stage 6 setting to zero
                       inet_pton(AF_INET, "0.0.0.0", &(tun_ICMP->ip_src)); //storing my address in mintor's source addrss
                       tun_ICMP->ip_sum = 0;
                       tun_ICMP->ip_sum = ip_checksum(tun_ICMP,sizeof(struct ip));
                       //
                        //stage 6 setting to zero
                       //Stage5 TODO
                       
                       direction = next_router[0]; //first hop
                       char embed[2048];
                       memset(embed, 0, sizeof(embed));

                       struct ip minhdr;
                       memset(&minhdr, 0, sizeof(minhdr));
                       
                       inet_pton(AF_INET, "127.0.0.1", &(minhdr.ip_src)); //stores minitor IP in_addr
                       inet_pton(AF_INET, "127.0.0.1", &(minhdr.ip_dst));
                       
                       minhdr.ip_p = 253;
                       /*
                       struct sockaddr_in sa;
                       char str[INET_ADDRSTRLEN];
                       // store this IP address in sa:
                       inet_pton(AF_INET, "192.0.2.33", &(sa.sin_addr));
                       */
                       
                       uint8_t type = 0x61;
                       uint16_t hostc_id = 0x0001;
                       uint16_t netwc_id = htons(hostc_id); //incoming circuit id 2bytes
                       
                       int increment = 0;
                       memcpy(embed, &minhdr, sizeof(minhdr));
                       increment = sizeof(minhdr);
                       memcpy(embed+increment, &type, sizeof(type));
                       increment = increment + sizeof(type);
                       memcpy(embed+increment, &netwc_id, sizeof(netwc_id));
                       increment = increment + sizeof(netwc_id);
                       
                       //encrypt multiple times
                       char * inbuf = new char [nread];
                       memset(inbuf, 0, nread);
                       memcpy(inbuf, tun_buf, nread); //clear text(tun_buf) is copied into inbuf
                       
                       int ilen = nread;
                       int olen = 0;
                       char * outbuf=NULL;
                       
                       for(int i = (hops-1); i >= 0; i--)
                       {
                           
                           unsigned char nextkey[16];
                           memset(nextkey,0,16);
                           memcpy(nextkey,keyorder[i],16); //storying encryption key in nextkey
                           /*
                           ofile.open("stage5.proxy.out",std::fstream::app);
                           ofile << "encrypted with key: ";
                           
                           for(int z = 0; z < 16; z++){
                               uint16_t shade;
                               memset(&shade,0,2);
                               shade = (uint16_t)nextkey[z];
                               if((shade < 0x000f)){
                                   ofile << "0";
                               }
                               if(shade > 0x00ff){
                                   shade = shade && 0x00ff;
                               }
                               ofile << hex << shade; //rk1[z];
                               
                           }
                           ofile << endl;
                           ofile.close();
                           */

                           AES_KEY enc_key;
                           class_AES_set_encrypt_key(nextkey, (AES_KEY *)&enc_key); //successfully creating AES_KEY from nextkey
                           class_AES_encrypt_with_padding((unsigned char*)inbuf, ilen,(unsigned char**) &outbuf, &olen, (AES_KEY *)&enc_key); //clear text inbuf
                           
                           //remember the new length
                           ilen = olen;
                           delete [] inbuf;
                           
                           inbuf = new char [ilen];
                           memset(inbuf, 0, ilen);
                           memcpy(inbuf, outbuf, ilen);
                           delete [] outbuf;
                       }
                       
                       /*decrypt for testing MIT address
                       AES_KEY enc_keey;
                       unsigned char *clear_crypt_text12;
                       int clear_crypt_text_len12;
                       unsigned char data3[2048];
                       memset(data3, 0, sizeof(data3));
                       memcpy(data3,inbuf,ilen);
                       
                       
                       class_AES_set_decrypt_key(keyorder[0], &enc_keey);
                       class_AES_decrypt_with_padding(data3, ilen, &clear_crypt_text12, &clear_crypt_text_len12, &enc_keey);
                       
                       struct ip realip;
                       struct in_addr srcreal;
                       struct in_addr destreal;
                       char IP_srcreal[NI_MAXHOST];
                       char IP_destreal[NI_MAXHOST];
                       
                       memcpy(&realip,clear_crypt_text12,sizeof(struct ip));
                       
                       srcreal = realip.ip_src;
                       inet_ntop(AF_INET, &srcreal, IP_srcreal, sizeof(IP_srcreal));
                       
                       destreal = realip.ip_dst;
                       inet_ntop(AF_INET, &destreal, IP_destreal, sizeof(IP_destreal));
                       ofile.open("stage5.proxy.out",std::fstream::app);
                       
                       ofile << "ICMP 2 from tunnel, src: " << IP_srcreal << ", dst: " << IP_destreal << ", type is: ";
                       
                       if(tun_utype == 8){
                           ofile << "8\n";
                       }
                       ofile.close();
                       
                       decrypt for testing MIT address*/

                       
                       cout << "right before sending onion" << endl;

                       cout << "ilen is " << ilen << endl;
                       cout << "length of encrypted text is " << strlen(inbuf) << endl;
                       //memcpy(circuit+increment,inbuf,ilen);
                       //increment = increment + ilen;
                       
                       
                       //encrypt multiple times
                       memcpy(embed+increment, inbuf,ilen);
                       increment = increment + ilen;
                              
                       /*
                       char circuit[5];
                       memset(circuit, 0, sizeof(circuit));
                       
                       uint8_t type = 0x52; // request type 1 byte
                       uint16_t hostc_id = 0x0001;
                       uint16_t netwc_id = htons(hostc_id); //incoming circuit id 2bytes
                       uint16_t netwport;
                       if(temphops == 1){
                           netwport = 0xffff;
                       }
                       else if(temphops !=1){
                           if(next_router[nexthop] == 1){
                               netwport = their_addr1.sin_port;
                           }
                           else if(next_router[nexthop] == 2){
                               netwport = their_addr2.sin_port;
                           }
                           else if(next_router[nexthop] == 3){
                               netwport = their_addr3.sin_port;
                           }
                           else if(next_router[nexthop] == 4){
                               netwport = their_addr4.sin_port;
                           }
                           else if(next_router[nexthop] == 5){
                               netwport = their_addr5.sin_port;
                           }
                           else if(next_router[nexthop] == 6){
                               netwport = their_addr6.sin_port;
                           }
                       }
                       
                       nexthop = nexthop + 1;
                       
                       memcpy(circuit,&type,1);
                       memcpy(circuit+1,&netwc_id,2);
                       memcpy(circuit+3,&netwport,2);
                       */
                       
                       if(direction == 1){
                           sendto(sockfd, embed, increment, 0, (struct sockaddr *)&their_addr1, sizeof(their_addr1));
                       }
                       else if(direction == 2){
                           sendto(sockfd, embed, increment, 0, (struct sockaddr *)&their_addr2, sizeof(their_addr2));
                       }
                       else if(direction == 3){
                           sendto(sockfd, embed, increment, 0, (struct sockaddr *)&their_addr3, sizeof(their_addr3));
                       }
                       else if(direction == 4){
                           sendto(sockfd, embed, increment, 0, (struct sockaddr *)&their_addr4, sizeof(their_addr4));
                       }
                       else if(direction == 5){
                           sendto(sockfd, embed, increment, 0, (struct sockaddr *)&their_addr5, sizeof(their_addr5));
                       }
                       else if(direction == 6){
                           sendto(sockfd, embed, increment, 0, (struct sockaddr *)&their_addr6, sizeof(their_addr6));
                       }
                       
                       /*
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
                       */
                       
                       
                       
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
                       
                       
                       int prox_recv = recvfrom(sockfd, router_buf, 2048, 0, (struct sockaddr *)&their_addr, &addr_len);
                       
                       cout << "Proxy received a packet" << endl;
                       
                       
                       struct ip * IP;
                       struct in_addr src;
                       struct in_addr dest;
                       char IP_src[NI_MAXHOST];
                       char IP_dest[NI_MAXHOST];
                       
                       
                       IP = (struct ip *)router_buf;
                       src = IP->ip_src;
                       inet_ntop(AF_INET, &src, IP_src, sizeof(IP_src));
                       
                       dest = IP->ip_dst;
                       inet_ntop(AF_INET, &dest, IP_dest, sizeof(IP_dest));
                       
                       /*
                        cout << "router says source is :" << tun_ip_src << endl;
                        cout << "router says destination is :" << tun_ip_dest << endl;
                        */
                       
                       //if we recieve a minitor IP headermessage
                       if((strcmp(IP_src,"127.0.0.1") == 0) && (strcmp(IP_dest, "127.0.0.1") == 0) && (IP->ip_p == 253)){
                           int length = prox_recv - sizeof(struct ip);
                           
                           ofile.open("stage5.proxy.out",std::fstream::app);
                           ofile << "pkt from port: " << dec << ntohs(their_addr.sin_port) << ", length: " << length << ", contents: 0x";
                           
                           char minitorbuf[2048];
                           memset(minitorbuf, 0, sizeof(minitorbuf));
                           memcpy(minitorbuf, router_buf+sizeof(struct ip), length);
                           
                           for(int i = 0; i < length; i++){
                               if(int(minitorbuf[i]) && 0x00008){
                                   int getit = int(minitorbuf[i]);
                                   getit = getit & 0x000000ff;
                                   ofile << setw(2) << setfill('0') << hex << getit;
                               }
                               else if(!(int(minitorbuf[i]) && 0x00008)){
                                   ofile << setw(2) << setfill('0') << hex << int(minitorbuf[i]);  //maybe in network order
                               }
                           }
                           
                           ofile << endl;
                           ofile.close();
                           
                           
                           uint16_t cid1 = (uint16_t)minitorbuf[1]; //upper
                           uint16_t cid2 = (uint16_t)minitorbuf[2]; //lower
                           cout << cid1 << endl;
                           cout << cid2 << endl;
                           cid1 = cid1 & 0x00ff;   //chars are only bytes so make sure
                           cid2 = cid2 & 0x00ff;
                           
                           cid1 = cid1*256;
                           cid1 = cid1 + cid2;
                           
                           uint16_t host = cid1; //convert from network to host basically switch lower with upper
                       
                           if(minitorbuf[0] == 0x64){
                               /*decrypt multiple rounds
                               AES_KEY enc_key;
                               unsigned char *clear_crypt_text;
                               int clear_crypt_text_len;
                               unsigned char data[2048];
                               memset(data, 0, sizeof(data));
                               memcpy(data,minitorbuf+3,length-3);
                               
                               
                               class_AES_set_decrypt_key(mykey, &enc_key);
                               class_AES_decrypt_with_padding(data, length-3, &clear_crypt_text, &clear_crypt_text_len, &enc_key);
                               */
                                /*
                               //encrypt multiple times
                               char * inbuf = new char [nread];
                               memset(inbuf, 0, nread);
                               memcpy(inbuf, tun_buf, nread); //clear text(tun_buf) is copied into inbuf
                               
                               int ilen = 16;
                               int olen = 0;
                               char * outbuf=NULL;
                               
                               for(int i = (hops-1); i >= 0; i--)
                               {
                                   
                                   unsigned char nextkey[16];
                                   memset(nextkey,0,16);
                                   memcpy(nextkey,keyorder[i],16); //storying encryption key in nextkey
                                   
                                   AES_KEY enc_key;
                                   class_AES_set_encrypt_key(nextkey, (AES_KEY *)&enc_key); //successfully creating AES_KEY from nextkey
                                   class_AES_encrypt_with_padding((unsigned char*)inbuf, ilen,(unsigned char**) &outbuf, &olen, (AES_KEY *)&enc_key); //clear text inbuf
                                   
                                   //remember the new length
                                   ilen = olen;
                                   delete [] inbuf;
                                   
                                   inbuf = new char [ilen];
                                   memset(inbuf, 0, ilen);
                                   memcpy(inbuf, outbuf, ilen);
                                   delete [] outbuf;
                               }
                                memcpy(embed+increment, inbuf,ilen);
                                increment = increment + ilen;
                                */
                               cout << "decrypt multiple rounds" << endl;
                               //decrpyt multiple
                               unsigned char data[2048];
                               memset(data, 0, sizeof(data));
                               memcpy(data,minitorbuf+3,length-3);
                               
                               char * inbuf = new char [length-3];
                               memset(inbuf, 0, length-3);
                               memcpy(inbuf, data, length-3);
                               
                               
                               int ilen = length-3;
                               int olen;
                               char * outbuf=NULL;
                               
                               for(int i = 0;  i < hops; i++)
                               {
                                   cout << "decrypt multiple rounds loop entered" << endl;
                                   unsigned char nextkey[16];
                                   memset(nextkey,0,16);
                                   memcpy(nextkey,keyorder[i],16); //storying encryption key in nextkey
                                   
                                   AES_KEY enc_key;
                                   class_AES_set_decrypt_key(nextkey, (AES_KEY *)&enc_key);
                                   class_AES_decrypt_with_padding((unsigned char*)inbuf, ilen,(unsigned char**) &outbuf, &olen, (AES_KEY *)&enc_key);
                                   cout << "decrypt multiple rounds past once" << endl;
                                   //remember the new length
                                   ilen = olen;
                                   delete [] inbuf;
                                   inbuf = new char [ilen];
                                   memcpy(inbuf, outbuf, ilen);
                                   delete [] outbuf;
                               }
                               
                               
                               
                               //decrpyt

                               router_ICMP = (struct ip *)(inbuf); // sets real ip as router_ICMP
                               cout << "decrypt multiple rounds" << endl;
                               
                               //test remove after
                               //inet_pton(AF_INET, "128.30.2.134", &(router_ICMP->ip_src)); //storing my address in mintor's source addrss
                               //router_ICMP->ip_sum = 0;
                               //router_ICMP->ip_sum = ip_checksum(tun_ICMP,sizeof(struct ip));
                               //test remove after
                               
                               char *outlet;
                               outlet = ipreturnchar[proxyid]; //retrieves IP address associated with proxyid (1 circuit for now)
                               inet_pton(AF_INET, outlet, &(router_ICMP->ip_dst));
                               router_ICMP->ip_sum = 0;
                               router_ICMP->ip_sum = ip_checksum(router_ICMP,sizeof(struct ip));
                               
                               
                               router_src = router_ICMP->ip_src;
                               inet_ntop(AF_INET, &router_src, router_ip_src, sizeof(router_ip_src));
                       
                               router_dest = router_ICMP->ip_dst;
                               inet_ntop(AF_INET, &router_dest, router_ip_dest, sizeof(router_ip_dest));
                       
                       
                               hlenlr = router_ICMP->ip_hl;
                               
                               if(router_ICMP->ip_p == IPPROTO_ICMP){
                                   router_rICMP = (struct icmp *)(inbuf + hlenlr*4);
                                //router_rICMP = router_rICMP + 0x00000001
                                   router_utype = router_rICMP->icmp_type;
                               
                                   cout << "This is ICMP type: ";
                                   
                                   if(router_utype == 8)
                                       cout << "8" << endl;
                                   else if(router_utype == 0)
                                       cout << "0" << endl;
                               }
                               //Stage 7 todo
                               if(router_ICMP->ip_p == IPPROTO_TCP){
                                   //inet_pton(AF_INET, myIP, &(tun_ICMP->ip_src));
                                   //hlenlr = tun_ICMP->ip_hl;
                                   struct tcphdr *tcp_hdr;
                                   int total_tcp = ntohs(router_ICMP->ip_len) - hlenlr*4;
                                   tcp_hdr = (struct tcphdr *)(inbuf + hlenlr*4); // char = 8 so mult by 4
                                   
                                   tcp_checksum(tcp_hdr,total_tcp,router_ICMP);
                               
                                   ofile.open("stage5.proxy.out",std::fstream::app);
                                   ofile << "incoming TCP packet, circuit incoming: 0x" << hex << host;
                                   ofile << ", incoming src IP/port: " << router_ip_src << ":" << tcp_hdr->source << ", dst IP/port: " << router_ip_dest << ":" << tcp_hdr->dest << ", seqno: " << tcp_hdr-> seq << ", ackno: " << tcp_hdr->ack_seq << endl;
                                   ofile.close();
                               
                               }
          
                               /*
                                cout << "source is router :" << router_ip_src << endl;
                                cout << "dest is tun1 :" << router_ip_dest << endl;
                                cout << "type is: " << router_utype << endl;
                                */
                       
                               if(tun_ICMP->ip_p == IPPROTO_ICMP){
                                   ofile.open("stage5.proxy.out",std::fstream::app);

                                   ofile << "incoming packet, circuit incoming: 0x" << hex << host;
                                   ofile << ", src: " << router_ip_src << ", dst: " << router_ip_dest << endl;
                                   ofile.close();
                               }

                               
                               char print[2048];
                               memset(print, 0, sizeof(print));
                               memcpy(print, inbuf, ilen);
                               
                               //TODO write to tunnel
                               
                               int nwrite = write(tun_fd,print,sizeof(print));
                               if(nwrite < 0)
                               {
                                   perror("Writing to tunnel interface");
                                   close(tun_fd);
                                   exit(1);
                               }
                               
                               //delete [] inbuf;
                
                           }// if type is 0x54
                       }//if minitor IP detected
               
                   } // if sockfd read
                   
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