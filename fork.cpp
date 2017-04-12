//
//  play.cpp
//  
//
//  Created by Ethan Chan on 9/25/14.
//
//

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
#include <unistd.h> //_exit, fork
//Ethan's additions
#include <iostream>
#include <fstream>
#include <sys/types.h> /* pid_t */
#include <sys/wait.h>  /* waitpid */
//Things to learn
//File I/O, fork(), socket programming
#include <pthread.h>

using namespace std;

//int main(int argc, char** argv)
int main(void)
{
    int test = 5;
    
    pid_t pid;

    for (int i = 0; i < 2 ; i++){
    pid = fork();          // after this function is called both parent AND child return from this function to this point
    }
    
    cout << "Hello" << endl;

    test = test - 1;
    
    
    if (pid == -1) {
        // When fork() returns -1, an error happened.
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        // When fork() returns 0, we are in the child process.
        test = test - 1;
        cout << test << endl;
        printf("Hello from the child process!\n");
        _exit(EXIT_SUCCESS);  // exit() is unreliable here, so _exit must be used
    }
    else {
        // When fork() returns a positive number, we are in the parent process
        // and the return value is the PID of the newly created child process.
        //sleep(1);
        cout << test << endl;
        printf("Hello from the parent process!\n");
        int status;
        (void)waitpid(pid, &status, 0);
    }
    
    return EXIT_SUCCESS;
}