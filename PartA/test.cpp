//
//  test.h
//
//
//  Created by Ethan Chan on 9/26/14.
//
//

#ifndef ____test__
#define ____test__

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
int main (){
    
    ofstream myfile;
    myfile.open("example.txt");
    myfile << "Writing this to a file. \n";
    myfile.close();
    
    myfile.open("example.txt");
    myfile << "hello \n";
    
    return 1;
}



#endif /* defined(____test__) */
