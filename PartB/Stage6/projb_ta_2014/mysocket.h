/*
* This code "USC CSci551 Projects A and B FA2012" is
* Copyright (C) 2012 by Zi Hu.
* All rights reserved.
*
* This program is released ONLY for the purposes of Fall 2012 CSci551
* students who wish to use it as part of their Project C assignment.
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
#ifndef SOCKET_H
#define SOCKET_H

#include "types.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>


class CSocket{
    public:
	int _sock;
	sockaddr_in _addr;
    public:
	CSocket();
	~CSocket();
	bool create(int sock_type, int protocol);

	bool close();

	bool bind(const int port);
	bool bind_rawsock(struct sockaddr_in src);


	//for tcp socket, no need for this project
	bool listen();
	bool accept(CSocket& new_sock);
	bool connect(const char* serv_host,const int port);

	//send/receive packet via UDP socket
	int recv_data_UDP(char* recv_buf, struct sockaddr_in & si_other);
	int send_data_UDP(const char* send_buf, const int len, struct sockaddr_in & ser_addr );


	int send_data_RAW(const char* send_buf, const int len, struct sockaddr_in & ser_addr );
	int recv_data_RAW(char* recv_buf, struct sockaddr_in & ser_addr );

	int send_icmp_rawsock(struct sockaddr_in dst_addr);

	bool set_socket_nonblock(bool nonblock);
	bool set_socket_fill_ipheader(bool on);
	bool is_alive(){return _sock!=-1;}
};

#endif
