//
//  getsockname.cpp
//  
//
//  Created by Ethan Chan on 9/25/14.
//
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

//call bind first
//http://support.sas.com/documentation/onlinedoc/sasc/doc700/html/lr2/zockname.htm


int main(int argc, char** argv)
{
    int s;
    struct sockaddr_in sa;
    int sa_len;

    /* We must put the length in a variable.              */
    sa_len = sizeof(sa);
    /* Ask getsockname to fill in this socket's local     */
    /* address.                                           */
    if (getsockname(s, (struct sockaddr *)&sa, &sa_len) == -1) {
        perror("getsockname() failed");
        return -1;
    }
    
    /* Print it. The IP address is often zero beacuase    */
    /* sockets are seldom bound to a specific local       */
    /* interface.                                         */
    printf("Local IP address is: %s\n", inet_ntoa(sa.sin_addr));
    printf("Local port is: %d\n", (int) ntohs(sa.sin_port));

    return 1;
}