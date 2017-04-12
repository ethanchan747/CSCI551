//
//  test.cpp
//  
//
//  Created by Ethan Chan on 10/26/14.
//
//

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



using namespace std; //include this right after headers

int main(int argc, char** argv)
{
    
    //hexdump()?
    deque<int> next_router;
    ofstream ofiler;
    char circuit[10];
    char *upper;
    char *lower;
    uint8_t type = 0x52; // request type 1 byte
    //cout << hex << circuit << endl;
    cout << type << endl;
    circuit [0] = type;
    circuit [1] = '\0';
    cout << circuit << endl;
    
    uint16_t hostc_id = 0x0309;
    uint16_t hostc_id1 = 0x0309;
    cout << hostc_id << endl;
    hostc_id1 = hostc_id1/(256);
    upper = (char *)&hostc_id1;
    strcat(circuit,upper);
    cout << (uint16_t)*upper << endl;
    lower = (char*)&hostc_id;
    strcat(circuit, lower);
    cout << (uint16_t)*lower << endl;
    circuit[3] = '\0';
    
    //cout << (uint16_t)*upper << endl;

    //strcat(circuit, (char*)hostc_id);
    //circuit [1] = (char)hostc_id;
    //circuit [3] = '\0';
    //circuit [2] = (char)(*((&hostc_id) << 1));

    cout << circuit << endl;
    cout << "size of circuit: " << circuit << endl;
    
    uint16_t num1 = (uint16_t)circuit[1];
    uint16_t num2 = (uint16_t)circuit[2];
    cout << num1 << endl;
    cout << num2 << endl;
    num1 = num1*256;
    num1 = num1 + num2;
    cout << "this is num1 : "<<  hex << num1 << endl;
    cout << "this is num1 network order " << hex << htons(num1) << endl;
    printf("%x", circuit[0]);
    cout << endl;
    printf("%x", circuit[1]);
    cout << endl;
    printf("%x", circuit[2]);
    cout << endl;
    uint8_t test = 0xc5;
    printf("%x", (unsigned char)test);
    cout << endl;

    cout << "length of circuit is " << strlen(circuit) << endl;
    
    if(circuit[0] == 0x52){
        cout << "YAYA" << endl;
    }
    
    cout << hex  << int(circuit[0]) << endl;
    cout << hex  << int(circuit[1]) << endl;
    cout << hex  << int(circuit[2]) << endl;
    cout << hex << int(circuit[3]) << endl;
    cout << hex << int(circuit[4]) << endl;
    cout << hex << int(circuit[5]) << endl;

    /*
    char testcase[10] = 0xabcdef2145241515;
    cout << "this is testcase ";
    printf("%x", testcase);
    cout << endl;
     */
    
    
    cout << "end of print out" << endl;
    
    
    char buffer[10];
    uint8_t type1 = 0x52;
    //uint16_t hostc_id4 = 0x0309;
    buffer[0] = type1;
    printf("%x", buffer[0]);
    cout << endl;

    
    int direction;
    int r;
    int temp;
    int routnum10 = 5;
    srand (time(0));
    cout << "this is random: " << rand() << endl;
    int test3 = rand() % 10 + 1;
    cout << "this is test3 " << test3 << endl;

    for(int t = 0; t < routnum10; t++){ // assignes numbers 1 to num
        next_router.push_back(t+1);
    }
    
    
    while(routnum10 > 0){
        //cout << "this is random: " << rand() << endl;
        //cout << "routnum10 is: " << routnum10 << endl;
        //cout << "entered in while" << endl;
        r =rand();
        //cout << "r is: " << r<<  endl;
        direction = r % routnum10; //numbers 0 - num -1
        //cout << "direciton is: " << direction << endl;
        temp = next_router[direction];
        next_router.erase(next_router.begin() + direction); // erased this element
        next_router.push_back(temp); //first hop pushed back
        routnum10 = routnum10 - 1;
        //cout << "routernum is " << routnum10 << endl;
    }
    cout << "mydeque contains:";
    for (std::deque<int>::iterator it = next_router.begin(); it!=next_router.end(); ++it)
        cout << ' ' << *it;
    cout << '\n';
    
    
    map<uint16_t, uint16_t> circuitids;
    

    // num1 = 0x3333
    circuitids[num1] = 0x1020;
    
    map<uint16_t,uint16_t>::iterator it;
    it = circuitids.find(0x0309);
    if(it != circuitids.end() ){
        cout << "Bill exists!" << endl; }
    else {
        cout << "Bill does not exist" << endl;
    }
    
    uint16_t myid = 0x0100*3 + 0x0001;

    cout << hex  << int(myid) << endl;
    
    uint16_t hello = 0x0001;
    printf("%x", hello);
    cout << hex << hello << endl;
    hello = 0x1000;
    cout << hex << hello << endl;
     printf("%x", hello);
    hello = 0x0000;
    char *a;
    a = (char *)&hello;
    cout << hex << int(*a) << endl;
    printf("%x", hello);
    
    uint8_t gtest = 0x34;
    cout << hex << int(gtest) << endl;
    char th = (char)gtest;
    cout << hex << int(th) << endl;
    char whati[20];
    whati[7] = (char)gtest;
    cout << hex << int(whati[7]) << endl;
    unsigned short int just = 48044;
    cout << "this starts " << endl;
    cout << hex << just << endl;
    cout << hex << htons(just) << endl;
    cout << "this ends " << endl;

    uint16_t top = just;
    unsigned short int wowzers;
    memcpy(&wowzers, &top, 2);
    int dad = int(wowzers);
    cout << "dad is : " << dad << endl;
    cout << wowzers << endl;
    
    cout << dec << wowzers << endl;
    cout << wowzers << endl;


    cout << char(top) << endl;
    uint64_t man = 0x12345;
    cout << hex << man << endl;
    cout << sizeof(uint64_t) << endl;
    uint64_t wow = 0xff000;
    uint64_t man2 = man&wow;
    cout << hex << man2 << endl;
    cout << sizeof(struct ip) << endl;
    u_char hoax = 253;
    if(hoax == 253){
        cout <<"HELLO" << endl;
    }
    
    /*
    uint16_t netwc_id = htons(hostc_id); //incoming circuit id 2bytes
    uint16_t netwport = their_addr1->sin_port;
    
    strcat(circuit, );
    strcat(msg, numberr);
    */
    
    return 1;

}