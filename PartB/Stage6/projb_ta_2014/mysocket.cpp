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
#include "mysocket.h"
#include "aes-test.h"

CSocket::CSocket()
{
    _sock=-1;
    memset(&(_addr),0,sizeof(_addr));
}

CSocket::~CSocket()
{
    if(is_alive())
	::close(_sock);
}

bool CSocket::close()
{
    if(is_alive())
	::close(_sock);
    return true;
}


bool CSocket::create(int sock_type, int protocol)
{
    //create socket
    _sock=::socket(AF_INET,sock_type,protocol);
    if(_sock==-1)
    {
	perror("socket failed\t");
	return false;
    }

    //set port reuse
    int on = 1;
    if (::setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, (const char*) &on, sizeof (on))== -1)
    {
	perror("setsockopt failed\t");
	return false;
    }
    return true;
}


//bind raw socket to a specific IP address
bool CSocket::bind_rawsock(struct sockaddr_in src)
{
    if(!is_alive())
	return false;
    int bind_ret = ::bind(_sock,(struct sockaddr *) &src,sizeof(src));
    if(bind_ret==-1)
    {
	perror("bind failed\t");
	return false;
    }
    return true;
}


bool CSocket::bind(const int port)
{
    if(!is_alive())
	return false;
    sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port=port;
    int bind_ret = ::bind(_sock,(struct sockaddr * ) &servaddr,sizeof(servaddr));
    if(bind_ret==-1)
    {
	perror("bind failed\t");
	return false;
    }
    else
    {
	int addr_len=sizeof(_addr);
	if(getsockname(_sock, (sockaddr *) &_addr, (socklen_t *)&addr_len) == -1)
	{
	    perror("getsockname failed\t");
	    return false;
	}
	return true;
    }
}

//for tcp socket, no need in this project
bool CSocket::listen()
{
    if(!is_alive())
	return false;
    int listen_ret = ::listen(_sock, MAX_CONNECTIONS);
    if(listen_ret==-1)
    {
	perror("listen failed\t");
	return false;
    }
    return true;
}

//for tcp socket, no need in this project
bool CSocket::accept(CSocket& new_sock)
{
    if(!is_alive())
	return false;
    sockaddr_in client_addr;
    socklen_t   addr_len=sizeof(client_addr);
    int client_sock;
    client_sock= ::accept(_sock,(sockaddr*)&client_addr,&addr_len);
    if(client_sock<=0)
	return false;
    else
    {
	new_sock._sock=client_sock;
	memcpy(&(new_sock._addr),&client_addr,sizeof(client_addr));
	return true;
    }
}

//send data via UDP socket
int CSocket::send_data_UDP(const char* send_buf, const int len, struct sockaddr_in & ser_addr )
{
    if(!is_alive())
	return -1;
    //int len=strlen(send_buf);
   
    int addr_len=sizeof(sockaddr_in);
    int send_ret = ::sendto(_sock, send_buf , len, 0, (const struct sockaddr*)&ser_addr, addr_len);
    if(send_ret==-1)
    {
	perror("sendto error:\t");
    }
    return send_ret;
}


//send an ICMP packet via raw socket (sendmsg)
int CSocket::send_icmp_rawsock(struct sockaddr_in dst_addr)
{
    struct Packet {
	struct icmphdr icmp;
	struct timeval tv;
    } p;

    struct iovec iov;
    
    struct msghdr m = {
	&dst_addr, sizeof(struct sockaddr_in), &iov,
	1, 0, 0, 0
	};

    ssize_t bs;
    p.icmp.type = ICMP_ECHO;
    p.icmp.code = 0;
    p.icmp.un.echo.id = 0;
 
    iov.iov_base = &p;
    iov.iov_len = sizeof(struct icmphdr)+ sizeof(struct timeval);
 

    p.icmp.checksum = 0;
    p.icmp.un.echo.sequence = htons (0);
    gettimeofday(&p.tv, NULL);
 
    p.icmp.checksum = in_cksum((uint16_t*)&p, iov.iov_len);
 
    if (0> (bs = sendmsg (_sock, &m, 0))) 
    {
	perror ("ERROR: sendmsg ()");
    }

    memset(&p, 0, sizeof(struct Packet));
    memset(&iov, 0, sizeof(struct iovec));
    memset(&m, 0, sizeof(struct msghdr));
    return bs;
}


//receive data from raw socket
int CSocket::recv_data_RAW(char* recv_buf, struct sockaddr_in & ser_addr )
{
    return recv_data_UDP(recv_buf, ser_addr);
}

//receive data from UDP socket
int CSocket::recv_data_UDP(char* recv_buf,  struct sockaddr_in& si_other)
{
    if(!is_alive())
	return -1;
    char buf[MAX_BUF_SIZE];
    memset(buf,0,MAX_BUF_SIZE);
    socklen_t addr_len=sizeof(sockaddr_in);

    int recv_ret= ::recvfrom(_sock, buf ,MAX_BUF_SIZE-1,0, (struct sockaddr*)&si_other, &addr_len);
    if(recv_ret==-1)
    {
	perror("recvfrom error:\t");
    }
    else
    {
	if(recv_ret>0)
	    memcpy(recv_buf,buf,MAX_BUF_SIZE);
    }
    return recv_ret;
}




bool CSocket::set_socket_nonblock(bool nonblock)
{
    int opts;
    if(!is_alive())
	return false;
    opts = fcntl ( _sock, F_GETFL );
    if ( opts < 0 )
	return false;
    if(nonblock)
	opts = ( opts | O_NONBLOCK );
    else
	opts = ( opts & ~O_NONBLOCK );
    fcntl ( _sock,F_SETFL,opts );
    return true;
}


bool CSocket::set_socket_fill_ipheader(bool on)
{
    if(setsockopt(_sock, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0)
    {
	perror("setsockopt() for IP_HDRINCL error");
	return false;
    }
    return true;
}

//used for TCP socket no need in this project
bool CSocket::connect(const char* serv_host,const int port)
{
    if(!is_alive())
	return false;

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if(serv_host==NULL)
    {
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    }
    else
    {
	serv_addr.sin_addr.s_addr=inet_addr(serv_host);
    }

    if( ::connect(_sock,(sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
    {
	perror("Connect failed\t");
	return false;
    }
    else
    {
	memcpy(&(_addr),&serv_addr,sizeof(serv_addr));
	return true;
    }
}



/*
 *print icmp header, steal from http://www.binarytides.com/packet-sniffer-code-in-c-using-linux-sockets-bsd/
 */
void print_icmp_packet(char* Buffer , int Size)
{
    unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr *)Buffer;
    iphdrlen = iph->ihl*4;
    struct icmphdr *icmph = (struct icmphdr *)(Buffer + iphdrlen);
    printf("\n***********************ICMP Packet*************************\n");  
    print_ip_header(Buffer , Size);
    printf("ICMP Header\n");
    printf("   |-Type : %d",(icmph->type));
    if((unsigned int)(icmph->type) == 11)
	printf("  (TTL Expired)\n");
    else if((unsigned int)(icmph->type) == ICMP_ECHOREPLY)
	printf("  (ICMP Echo Reply)\n");
    printf("   |-Code : %d\n",(unsigned int)(icmph->code));
    printf("   |-Checksum : %d\n",ntohs(icmph->checksum));
    //printf("   |-ID       : %d\n",ntohs(icmph->id));
    //printf("   |-Sequence : %d\n",ntohs(icmph->sequence));
    printf("\n");

}
    

/*
 *print ip header, steal from http://www.binarytides.com/packet-sniffer-code-in-c-using-linux-sockets-bsd/
 */
void print_ip_header(char* Buffer, int Size)
{
    //unsigned short iphdrlen;
    struct sockaddr_in source,dest;

    struct iphdr *iph = (struct iphdr *)Buffer;
    //iphdrlen =iph->ihl*4;
    memset(&source, 0, sizeof(source));

    source.sin_addr.s_addr = iph->saddr;
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

    printf("IP Header\n");
    printf("   |-IP Version        : %d\n",(unsigned int)iph->version);
    printf("   |-IP Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)iph->ihl,((unsigned int)(iph->ihl))*4);
    printf("   |-Type Of Service   : %d\n",(unsigned int)iph->tos);
    printf("   |-IP Total Length   : %d  Bytes(Size of Packet)\n",ntohs(iph->tot_len));
    printf("   |-Identification    : %d\n",ntohs(iph->id));

    printf("   |-TTL      : %d\n",(unsigned int)iph->ttl);
    printf("   |-Protocol : %d\n",(unsigned int)iph->protocol);
    printf("   |-Checksum : %d\n",ntohs(iph->check));
    printf("   |-Source IP        : %s\n",inet_ntoa(source.sin_addr));
    printf("   |-Destination IP   : %s\n",inet_ntoa(dest.sin_addr));
}

/*
 *print tcp header, steal from http://www.binarytides.com/packet-sniffer-code-in-c-using-linux-sockets-bsd/
 */
void print_tcp_packet(char* Buffer, int Size)
{

    unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr *)Buffer;
    iphdrlen = iph->ihl*4;
    struct tcphdr *tcph=(struct tcphdr*)(Buffer + iphdrlen);

    printf("\n***********************TCP Packet*************************\n");   
    print_ip_header(Buffer,Size);
    printf("TCP Header\n");
    printf("   |-Source Port      : %u\n",ntohs(tcph->source));
    printf("   |-Destination Port : %u\n",ntohs(tcph->dest));
    printf("   |-Sequence Number    : %u\n",ntohl(tcph->seq));
    printf("   |-Acknowledge Number : %u\n",ntohl(tcph->ack_seq));
    printf("   |-Header Length      : %d DWORDS or %d BYTES\n" ,(unsigned int)tcph->doff,(unsigned int)tcph->doff*4);
    //fprintf(logfile,"   |-CWR Flag : %d\n",(unsigned int)tcph->cwr);
    //fprintf(logfile,"   |-ECN Flag : %d\n",(unsigned int)tcph->ece);
    printf("   |-Urgent Flag          : %d\n",(unsigned int)tcph->urg);
    printf("   |-Acknowledgement Flag : %d\n",(unsigned int)tcph->ack);
    printf("   |-Push Flag            : %d\n",(unsigned int)tcph->psh);
    printf("   |-Reset Flag           : %d\n",(unsigned int)tcph->rst);
    printf("   |-Synchronise Flag     : %d\n",(unsigned int)tcph->syn);
    printf("   |-Finish Flag          : %d\n",(unsigned int)tcph->fin);
    printf("   |-Window         : %d\n",ntohs(tcph->window));
    printf("   |-Checksum       : %d\n",ntohs(tcph->check));
    printf("   |-Urgent Pointer : %d\n",tcph->urg_ptr);
    printf("\n");

    
}

/*
 *print udp header, steal from http://www.binarytides.com/packet-sniffer-code-in-c-using-linux-sockets-bsd/
 */
void print_udp_packet(char *Buffer , int Size)
{
    unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr *)Buffer;
    iphdrlen = iph->ihl*4;
    struct udphdr *udph = (struct udphdr*)(Buffer + iphdrlen);
    printf("\n***********************UDP Packet*************************\n");
    print_ip_header(Buffer,Size);          
    printf("\nUDP Header\n");
    printf("   |-Source Port      : %d\n" , ntohs(udph->source));
    printf("   |-Destination Port : %d\n" , ntohs(udph->dest));
    printf("   |-UDP Length       : %d\n" , ntohs(udph->len));
    printf("   |-UDP Checksum     : %d\n" , ntohs(udph->check));
    printf("\n");

}


/*
 * compute checksume
 */
unsigned short in_cksum(unsigned short *addr, int len)
{
    register int sum = 0;
    u_short answer = 0;
    register u_short *w = addr;
    register int nleft = len;
	  
    while (nleft > 1)
    {
	sum += *w++;
	nleft -= 2;
    }
    /* mop up an odd byte, if necessary */
    if (nleft == 1)
    {
	*(u_char *) (&answer) = *(u_char *) w;
	sum += answer;
    }
    /* add back carry outs from top 16 bits to low 16 bits */
    sum = (sum >> 16) + (sum & 0xffff);       /* add hi 16 to low 16 */
    sum += (sum >> 16);               /* add carry */
    answer = ~sum;              /* truncate to 16 bits */
    return (answer);
}

// try to get the ip of all virtual interfaces (eth0, eth1, ...)
int get_all_interface_ip(struct router_info* rinfo)
{
    int fd;
    struct if_nameindex *curif, *ifs;
    struct ifreq req;
    if((fd = socket(PF_INET, SOCK_DGRAM, 0)) != -1) 
    {
	ifs = if_nameindex();
	if(ifs)
	{
	    for(curif = ifs; curif && curif->if_name; curif++)
	    {
		strncpy(req.ifr_name, curif->if_name, IFNAMSIZ);
		req.ifr_name[IFNAMSIZ] = 0;
		if (ioctl(fd, SIOCGIFADDR, &req) < 0)
		    perror("ioctl");
		else
		{
		    //printf("%s: [%s]\n", curif->if_name, inet_ntoa(((struct sockaddr_in*) &req.ifr_addr)->sin_addr));
		    if(strncmp(curif->if_name,"ethX",3)==0)
		    {
			char eth_num[2];
			memset(eth_num,0,2);
			strncpy(eth_num,curif->if_name+3,1);
			int index = atoi(eth_num);
			if(index>=MAX_ROUTER_COUNT)
			    printf("Exceed the max number of routers\n");
			else
			{
			    rinfo[index].nIP =  (((struct sockaddr_in*) &req.ifr_addr)->sin_addr).s_addr;
			}
		    }
		}
	    }
	    if_freenameindex(ifs);
	    if(close(fd)!=0)
	    {
		perror("close");
	    }
	}
	else
	{
	    perror("if_nameindex");
	    return 0;
	}
    }
    else
    {
	perror("socket");
	return 0;
    }
    return 1;
}

//compute circuit id
int compute_circuit_id(int index, int seq)
{
    return (index*256+seq);
}

//build relay message
int construct_relay_msg(char*buf, int buf_len,  unsigned short cID, char*payload, int payload_len, int msg_type, int stage)
{
    struct iphdr * iph = (struct iphdr *)buf;
    struct cc_relay_msg * ccrelaymsg = ( struct cc_relay_msg *)(iph+1);
    

    iph->protocol = CC_EXT_PROTOCOL;
    // use loop address 
    iph->saddr = inet_addr("127.0.0.1");
    iph->daddr = inet_addr("127.0.0.1");

    //iph->check = in_cksum((unsigned short*)buf, sizeof(struct iphdr));

    // msg type: 0x51
    ccrelaymsg->msg_type = msg_type;
    // circuit ID
    ccrelaymsg->cid = htons(cID);
    
    if(stage == 5)
    {
	struct iphdr * riph = (struct iphdr *)(payload);
	riph->check = 0 ;
	riph->check = in_cksum((unsigned short*)payload, sizeof(struct iphdr));
    }
    

    int hlen = sizeof(struct iphdr) + sizeof(struct cc_relay_msg);
    memcpy(buf+hlen, payload, payload_len);
    return (hlen + payload_len);
}

//encrypt
void encrypt_msg_with_padding(char* inbuf, int inlen, char** outbuf, int* outlen, unsigned char key[])
{
    AES_KEY enc_key;
    class_AES_set_encrypt_key(key, (AES_KEY *)&enc_key);
    class_AES_encrypt_with_padding((unsigned char*)inbuf, inlen,(unsigned char**) outbuf, outlen, (AES_KEY *)&enc_key);
}

//decrypt
void decrypt_msg_with_padding(char* inbuf, int inlen, char** outbuf, int* outlen, unsigned char key[])
{

    AES_KEY dec_key;
    class_AES_set_decrypt_key(key, (AES_KEY *)&dec_key);
    class_AES_decrypt_with_padding((unsigned char*)inbuf, inlen,(unsigned char**) outbuf, outlen, (AES_KEY *)&dec_key);

}

//transfer key to buffer in hex
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

//for debug
void print_packet_hex(char* buf, int len)
{
    char log_buf[MAX_BUF_SIZE];
    memset(log_buf, 0, MAX_BUF_SIZE);
    int index=0;
    index += sprintf(log_buf, "buf length: %d, contents: 0x", len);
    for(int i=0; i<len; i++)
    {
	index += sprintf(log_buf+index, "%02x", (unsigned char)buf[i]);
    }
    sprintf(log_buf+index, "\n");
    printf(log_buf);
}
