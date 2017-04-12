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
#ifndef CLIENT_H
#define CLIENT_H
#include "mysocket.h"
class CRouter
{
    private:
    CSocket _mysock;
    CSocket _icmp_sock;

    struct circuit cc;

    char _logfn[MAX_FN_LEN];
    int _stage;
    int _index;
    int _cc_seq;
    struct sockaddr_in _paddr;

    struct sockaddr_in old_src_addr;
    unsigned short old_ip_id;
    char packet_buf[MAX_PACKET_SIZE];
    int packet_len;

    unsigned long _rip;
    unsigned char aes_key[KEY_LEN];

    public:
    CRouter();
    CRouter(int stage, int index,  struct sockaddr_in paddr, unsigned long ip);
    ~CRouter();

    void print_buf_hex(char* buf, int buf_len, int port);

    bool initialize_socket();
    bool initialize_rawsocket();

    bool bind_rawsock_src(struct sockaddr_in src);

    int get_port();
    void output_log(char* out_str);


    int send_ICMP_packet(struct sockaddr_in dst_addr);
    int recv_data_ICMP(char* recv_buf, struct sockaddr_in & ser_addr);
    void run();
    int send_data_UDP(const char* send_buf, const int len, struct sockaddr_in & ser_addr);
    int recv_data_UDP(char* recv_buf, struct sockaddr_in & si_other);
    
    void handle_rawsock_icmp_traffic(char* buf, int len);
    void handle_proxy_icmp_traffic(char* buf, int len, struct sockaddr_in si_other);

    void handle_ccext_msg(char* buf, int len, struct sockaddr_in si_other);
    void handle_ccext_done_msg(char* buf, int len, struct sockaddr_in si_other);
    void handle_relay_msg(char* buf, int len, struct sockaddr_in si_other);
    void self_reply_icmp(char* buf, int len, struct sockaddr_in si_other);

    void handle_deffie_hellman_msg(char* buf, int len, struct sockaddr_in si_other);

};

#endif
