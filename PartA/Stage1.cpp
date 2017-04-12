//
//  play.cpp
//  
//
//  Created by Ethan Chan on 9/25/14.
//
/*Errors to fix: I need to wait for ALL children to finish until I exit parent. Also I need to store the individual pid_t pid for each child? perhaps make a vector or (?) of pids? Or can I use getpid() within each child to get its pid? need to make cstring str1 infinitely long?

 Notes: we are using IPv4, don't forget to free socks and res, sendt(inclues own socket, address of other's destination socket, we NEED to for PART B set routernum = num, num we read in from config_file
 
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

using namespace std;



int main(int argc, char** argv)
{
    ofstream ofile;
    deque<pid_t> my_deq;
    string str1;     //should make this cstring infinitely long maybe string
    string str2 = "stage 1";
    char str3[15];
    
    int routernum = 1;  //keep this as one for now
    int num;    //should be removed and replaced with routernum
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
    
    cout << "\nThis is the comment of the config_file: \n" << str1 << endl;
    
    getline(ifile,str1);
    
    if( str1 == str2 ) {
       cout << "We are in Stage 1 " << endl;
    }
    
       getline(ifile,str1,' ');
        ifile.getline(str3,15);
       
       num = atoi(str3);        //replace num with routernum
    cout << "The number of routers is: " << num << "\n" << endl; //replace num with router num
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
    
    cout << "1. My file descriptor is: " << sockfd << endl;
    
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
    
    cout << "We have just finished setting up the Onion Proxy:" << endl;
    cout << "1. Means message from Parent (Onion Proxy)" << endl;
    cout << "2. Means message from Child (Router)" << endl;
    cout << "12. Means both should print this statement \n" << endl;
    
    cout << "1. The proxy port number is: " << proxy_port << endl;

    if(routernum){
        //forking methode copied from wikipedia
         pid = fork(); // after this function is called both parent AND child return from this function to this point
        routernum = routernum - 1;
        if(pid){ //if you're parent then push back the pid
        my_deq.push_back(pid);
        }
    }
    
    cout << "12. This is int routernum value: " << routernum << endl;
    
    if (pid == -1) {
        // When fork() returns -1, an error happened.
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {

        // When fork() returns 0, we are in the child process.
        printf("2. Hello from the child process!\n");
        
        // socket programming copied from page 19 of Beej's Guide to Network Programming
        
        /* Setting up dynamic UDP port for Onion Router */
        struct addrinfo hintsr;
        struct addrinfo *resr;
        int sockfdr, sockbindr, dynamic_portr, statusr, getr;
        struct sockaddr_in *ipv4r; //structure for ipv4
        socklen_t addrlen1r = sizeof(*ipv4r);
        char msg[20];
        int child = (int) getpid();
        sprintf(msg, "%d",child);   //stores PID in msg succes
        
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
        
        
        cout << "2. My file descriptor is: " << sockfdr << endl;
        cout << "2. My dynamic port is: " << dynamic_portr << endl;
        cout << "2. Onion Proxy's port port is: " << proxy_port << endl;
        
        ofstream ofiler;
        ofiler.open("stage1.router1.out");
        ofiler << "router: " << one << ", pid: " << msg << ", port: " << dynamic_portr << "\n";
        ofiler.close();
        
        one = one + 1;

        /*finished setting up dynamic UDP port for Onion Router */

        /* Sending "I'm up" message to Onion Proxy */
        
        sendto(sockfdr, msg, 20, 0, res->ai_addr, res->ai_addrlen);  // res is the local variable of the addrinfo res from Onion router but we send from our own sock "sockfdr"
        
        /* Sending "I'm up" message to Onion Proxy */
        
        
        _exit(EXIT_SUCCESS);  // exit() is unreliable here, so _exit must be used
    }
   else {
       
       // When fork() returns a positive number, we are in the parent process
       // and the return value is the PID of the newly created child process.
       printf("Hello from the parent process!\n");
       
       int bytes_rec = 0;
       struct sockaddr_in their_addr;
       socklen_t addr_len = sizeof(their_addr);
       char buf[20];
       int router_port;
       //int getr;
       
       
       ofile.open("stage1.proxy.out");
       
       while(!bytes_rec){
           int number = 1;
           bytes_rec = recvfrom(sockfd, buf, 20, 0, (struct sockaddr *)&their_addr, &addr_len);
           
           router_port = ntohs(their_addr.sin_port);
           
           ofile << "proxy port: " << proxy_port << endl;
           ofile << "router: " << number << ", pid: " << buf << ", port: " << router_port << endl;
           ofile.close();
           number = number + 1;
       }
       
       cout << "1. Router's port number is: " << router_port << endl;

       cout << "1. Message from router (router's PID) is: " << buf << endl;
       
       /*
       getr = getsockname(sockfd, (struct sockaddr *)&their_addr, &addr_len);
       
       if(getr == 0 && their_addr.sin_family == AF_INET && addr_len ==sizeof(their_addr)){
           router_port = ntohs(their_addr.sin_port);
       }
       */
       
       //router_port = ntohs(their_addr.sin_port); // getting port number of router
       


        //sleep(2); this is a "fake, cheap" way of doing waitpid. How do I wait for all children to finish?
        int stat;
        pid_t end;
       while(!my_deq.empty()){
           end = my_deq.front();
           cout << "1. PID's in deq " << end << endl;
           (void)waitpid(end, &stat, 0);
           my_deq.pop_front();
       }
         
         //(void)waitpid(pid, &stat, 0);
       
    }
    return EXIT_SUCCESS;
}
//helloaasdfasdfa1234