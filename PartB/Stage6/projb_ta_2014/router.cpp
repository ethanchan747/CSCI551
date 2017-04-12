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

#include "router.h"
CRouter::CRouter()
{

}

CRouter::CRouter(int stage, int index, struct sockaddr_in paddr, unsigned long ip)
{
    _stage = stage;
    _index = index;
    _cc_seq = 1;
    _rip = ip;
    memset(&cc,0,sizeof(struct circuit));
    memcpy(&_paddr, &paddr, sizeof( struct sockaddr_in));
    printf("router: proxy port: %d\n", ntohs(_paddr.sin_port));

    memset(_logfn, 0, MAX_FN_LEN);
    sprintf(_logfn,"stage%d.router%d.out",_stage,_index);

}


CRouter::~CRouter()
{
}
//create socket
bool CRouter::initialize_socket()
{
    bool status=true;
    status &= _mysock.create(SOCK_DGRAM,0);
    status &= _mysock.bind(0);
    if(status)
    {
	char out_buf[MAX_BUF_SIZE];

	struct sockaddr_in rip;
	rip.sin_addr.s_addr = _rip;
    	memset(out_buf,0, MAX_BUF_SIZE);
	sprintf(out_buf, "router: %d, pid: %d, port: %d, IP: %s\n",_index, getpid(), get_port(), inet_ntoa(rip.sin_addr));
	FILE* logfp=fopen(_logfn,"w");
    	if(!logfp)
    	{
		printf("Open Log File:%s failed \n",_logfn);
    	}	
	else
	{
		fputs(out_buf, logfp);
    		fclose(logfp);
	}
    }
    return status;
    
}

//create ICMP raw socket
bool CRouter::initialize_rawsocket()
{
    bool status=true;
    status &= _icmp_sock.create(SOCK_RAW,IPPROTO_ICMP);
    if(status)
    {
	printf("icmp socket create succeed\n");
    }

    in_addr_t src=_rip;
    struct sockaddr_in src_addr;
    src_addr.sin_family = AF_INET;
    src_addr.sin_addr.s_addr = src;
    
    status &= _icmp_sock.bind_rawsock(src_addr);

    return status;
}


//bind raw ICMP socket to router's IP
bool CRouter::bind_rawsock_src(struct sockaddr_in src)
{
   return( _icmp_sock.bind_rawsock(src));
}


int CRouter::recv_data_UDP(char* recv_buf, struct sockaddr_in & si_other)
{
    int recv_ret=_mysock.recv_data_UDP(recv_buf, si_other);
    return recv_ret;
}

int CRouter::send_data_UDP(const char* send_buf, const int len, struct sockaddr_in & ser_addr)
{
    int send_ret=_mysock.send_data_UDP(send_buf, len, ser_addr);
    return send_ret;
}


int CRouter::send_ICMP_packet(struct sockaddr_in dst_addr)
{
    int send_ret=_icmp_sock.send_icmp_rawsock(dst_addr);
    return send_ret;
}


int CRouter::recv_data_ICMP(char* recv_buf, struct sockaddr_in & ser_addr)
{
    int recv_ret=_icmp_sock.recv_data_RAW(recv_buf, ser_addr);
    return recv_ret;
}


int CRouter::get_port()
{
    return  ntohs((_mysock._addr).sin_port);
}


//handle circuit extend done message
void CRouter::handle_ccext_done_msg(char* buf, int len, struct sockaddr_in si_other)
{

    char log_buf[MAX_BUF_SIZE];
    int nsend;

    struct iphdr *iph = (struct iphdr *)buf;
    struct cc_ext_msg * ccextmsg = (struct cc_ext_msg*)(iph+1);
    print_buf_hex((char*)ccextmsg, len-(sizeof(struct iphdr)), ntohs(si_other.sin_port));

    unsigned short iID = ntohs(ccextmsg->cid);
    
    memset(log_buf, 0, MAX_BUF_SIZE);
    if(iID == cc._oid)
    {
        //forward the message back along the partial created path.
	ccextmsg->cid = htons(cc._iid);
	struct sockaddr_in prev_hop;
	prev_hop.sin_family = AF_INET;
	prev_hop.sin_port = htons(cc._iport);
	prev_hop.sin_addr.s_addr =  htonl(INADDR_ANY);

	nsend = send_data_UDP(buf, len, prev_hop);
        if( nsend <= 0)
        {
	    printf("**Router** %d, failed send circuit message via UDP\n", _index);
        }

	sprintf(log_buf, "forwarding extend-done circuit, incoming: 0x%02x, outgoing: 0x%02x at %d\n",iID, cc._iid, cc._iport );
	output_log(log_buf);
    }
    else
    {
	printf("**Router** %d, received unknown circuit message via UDP\n", _index);
	sprintf(log_buf, "unknown extend-done circuit: incoming:0x%d, port:%d\n",iID, ntohs(si_other.sin_port));

	output_log(log_buf);
    }

    
	
}

//handle relay message
void CRouter::handle_relay_msg(char* buf, int len, struct sockaddr_in si_other)
{

    char log_buf[MAX_BUF_SIZE];
    char send_buf[MAX_PACKET_SIZE];
    char ssi[MAX_BUF_SIZE];
    char sso[MAX_BUF_SIZE];
    char sdest[MAX_BUF_SIZE];
    char odest[MAX_BUF_SIZE];
    struct sockaddr_in si,so,dest,dest2;

    memset(send_buf,0, MAX_PACKET_SIZE);
    int nsend;
    char *p;
    unsigned short last_hop = strtol("0xffff", &p, 16);

    struct iphdr *iph = (struct iphdr *)buf;
    struct cc_relay_msg * ccrelaymsg = (struct cc_relay_msg*)(iph+1);

    print_buf_hex((char*)ccrelaymsg, len-(sizeof(struct iphdr)), ntohs(si_other.sin_port));

    unsigned short iID = ntohs(ccrelaymsg->cid);

    struct iphdr *riph = (struct iphdr *)(ccrelaymsg+1);
    si.sin_addr.s_addr = riph->saddr;
    so.sin_addr.s_addr = _rip;
    dest.sin_addr.s_addr = riph->daddr;
    strcpy(ssi, inet_ntoa(si.sin_addr));
    strcpy(sso, inet_ntoa(so.sin_addr));
    strcpy(sdest, inet_ntoa(dest.sin_addr));

    
    struct sockaddr_in next_hop;
    next_hop.sin_family = AF_INET;
    next_hop.sin_addr.s_addr =  htonl(INADDR_ANY);
    memset(log_buf, 0, MAX_BUF_SIZE);

    int hlen = sizeof(struct iphdr) + sizeof( struct cc_relay_msg);
    int plen = 0;
    if(ccrelaymsg->msg_type == CC_RELAY_MSGTYPE || ccrelaymsg->msg_type == CC_ENCRYPTED_RELAY)
    {
    	if(iID == cc._iid )
    	{
	    //for stage 5
	    if(ccrelaymsg->msg_type == CC_RELAY_MSGTYPE)
	    {
	    	//remember the mapping.
	    	cc._iip =  riph->saddr;
	    	cc._oip = _rip;

	    	// change the source IP address and recompute checksum;
	    	riph->saddr = _rip;
	    	riph->check = 0;
	    	riph->check = in_cksum((unsigned short*)riph, sizeof(struct iphdr));
	    }
	    //for stage 6
	    if(ccrelaymsg->msg_type == CC_ENCRYPTED_RELAY)
	    {

		plen = len - hlen;
		char * decrypted_payload = NULL; 
		int dlen;
		char * clear_payload = new char [plen];
		memset(clear_payload, 0, plen);
		memcpy(clear_payload, buf+hlen, plen);
		
		//decrypt the payload with its key.
		decrypt_msg_with_padding(clear_payload, plen, &decrypted_payload, &dlen, aes_key);
		memcpy(buf+hlen, decrypted_payload, dlen);
		delete [] decrypted_payload;
		delete [] clear_payload;
		len = hlen + dlen;
	    }

	    //last hop? send out via raw socket.
    	    if(cc._oport == last_hop)
    	    {
		//log
		if(_stage == 6)
		{
		    si.sin_addr.s_addr = riph->saddr;
		    strcpy(ssi, inet_ntoa(si.sin_addr));
		}
		print_packet_hex(buf+hlen+sizeof(struct iphdr), len-hlen-sizeof(struct iphdr));
		dest.sin_addr.s_addr = riph->daddr;
		strcpy(sdest, inet_ntoa(dest.sin_addr));
		sprintf(log_buf, "outgoing packet, circuit incoming: 0x%02x, incoming src:%s, outgoing src: %s, dst: %s\n", cc._iid, ssi, sso, sdest);
		output_log(log_buf);
		
		if(ccrelaymsg->msg_type == CC_ENCRYPTED_RELAY)
		{
		    // change the source IP address and recompute checksum;
		    riph->saddr = _rip;
		    riph->check = 0;
		    riph->check = in_cksum((unsigned short*)riph, sizeof(struct iphdr));
		}

    	        printf("**Router** %d, reach the final hop, send packet out via raw socket\n",_index);
		//for debug only
		print_icmp_packet(buf+hlen, len-hlen);
		//send out the packet via raw socket.
    	        handle_proxy_icmp_traffic(buf+hlen, len-hlen, si_other);

    	    }
    	    //otherwise, forward the message along the partial created path.
    	    else
    	    {
    	        ccrelaymsg->cid = htons(cc._oid);
    	        next_hop.sin_port = htons(cc._oport);
    	        nsend = send_data_UDP(buf, len, next_hop);
    	        if( nsend <= 0)
    	        {
		    printf("**Router** %d, failed send circuit message via UDP\n", _index);
    	        }

		if(ccrelaymsg->msg_type == CC_ENCRYPTED_RELAY)
		    sprintf(log_buf, "relay encrypted packet, circuit incoming: 0x%02x, outgoing: 0x%02x\n", cc._iid, cc._oid);
		else
		    sprintf(log_buf, "relay packet, circuit incoming: 0x%02x, outgoing: 0x%02x, incoming src:%s, outgoing src: %s, dst: %s\n", cc._iid, cc._oid, ssi, sso, sdest);
		output_log(log_buf);

    	    }
    	    
    	}
	//unknown packet???
    	else
    	{
    	    printf("**Router** %d, received UNNORMAL relay message via UDP\n", _index);
    	    sprintf(log_buf, "unknown incoming circuit: 0x%02x, src: %s, dst: %s\n", iID, ssi, sdest);
	    output_log(log_buf);
    	    
    	}
    }

    //handle relay reply packet
    if(ccrelaymsg->msg_type == CC_RELAY_BACK_MSGTYPE ||  ccrelaymsg->msg_type == CC_ENCRYPTED_RELAY_REPLY)
    {
	memset(log_buf, 0, MAX_BUF_SIZE);
    	if (iID == cc._oid)
    	{
	    //for stage 5
	    if(ccrelaymsg->msg_type == CC_RELAY_BACK_MSGTYPE)
	    {
		dest2.sin_addr.s_addr = cc._iip;
	    	strcpy(odest, inet_ntoa(dest2.sin_addr));
	    	sprintf(log_buf, "relay reply packet, circuit incoming: 0x%02x, outgoing: 0x%02x, src: %s, incoming dst: %s, outgoing dest: %s\n", iID, cc._iid, ssi, sdest, odest);
	    	// change the destination IP address and recompute checksum;
	    	riph->daddr = cc._iip;
	    	riph->check = 0;
	    	riph->check = in_cksum((unsigned short*)riph, sizeof(struct iphdr));
	    }

	    //for stage 6
	    if(ccrelaymsg->msg_type == CC_ENCRYPTED_RELAY_REPLY)
	    {

		int plen = len - hlen;
		char * encrypted_payload = NULL; 
		int olen;
		char * clear_payload = new char [plen];
		memset(clear_payload, 0, plen);
		memcpy(clear_payload, buf+hlen, plen);

		//encrypt the payload with its key.
		encrypt_msg_with_padding(clear_payload, plen, &encrypted_payload, &olen, aes_key);
		memcpy(buf+hlen, encrypted_payload, olen);
		sprintf(log_buf, "relay reply encrypted packet, circuit incoming: 0x%02x, outgoing: 0x%02x\n", iID, cc._iid);
		delete [] encrypted_payload;
		delete [] clear_payload;
		len = hlen + olen;
	    }

    	    ccrelaymsg->cid = htons(cc._iid);
	    next_hop.sin_port = htons(cc._iport);
       	    nsend = send_data_UDP(buf, len, next_hop);
    	    if( nsend <= 0)
    	    {
    	        printf("**Router** %d, failed send relay message via UDP\n", _index);
    	    }
	    output_log(log_buf);

    	}
	//unknown packet????
	else
    	{
    	    printf("**Router** %d, received UNNORMAL relay message via UDP\n", _index);
    	    sprintf(log_buf, "unknown incoming circuit: 0x%02x, src: %s, dst: %s\n", iID, ssi, sdest);
	    output_log(log_buf);
    	}

    }
        
	
}


//handle deffie hellman message
void CRouter::handle_deffie_hellman_msg(char* buf, int len, struct sockaddr_in si_other)
{
    char log_buf[MAX_BUF_SIZE];
    char send_buf[MAX_PACKET_SIZE];
    memset(send_buf,0, MAX_PACKET_SIZE);
    int nsend;

    struct iphdr *iph = (struct iphdr *)buf;
    struct cc_deffie_hellman_msg * cc_dh_msg = (struct cc_deffie_hellman_msg*)(iph+1);

    print_buf_hex((char*)cc_dh_msg, len-(sizeof(struct iphdr)), ntohs(si_other.sin_port));

    unsigned short iID = ntohs(cc_dh_msg->cid);
    

    int hlen = sizeof( struct iphdr) + sizeof( struct cc_deffie_hellman_msg);
    int plen = len - hlen;

    unsigned short oID = compute_circuit_id(_index, _cc_seq);
    if(iID == cc._iid )
    {
        //decrypt the key, forward the message along the partial created path.
	cc_dh_msg->cid = htons(oID);
	char* decrypted_key = NULL;
	int klen;
	decrypt_msg_with_padding(buf+ hlen, plen, &decrypted_key, &klen, aes_key);
	memcpy(buf+hlen, decrypted_key, klen);
	delete [] decrypted_key;
	len = hlen+klen;

	struct sockaddr_in next_hop;
	next_hop.sin_family = AF_INET;
	next_hop.sin_port = htons(cc._oport);
	next_hop.sin_addr.s_addr =  htonl(INADDR_ANY);

	nsend = send_data_UDP(buf, len, next_hop);
        if( nsend <= 0)
        {
	    printf("**Router** %d, failed send circuit message via UDP\n", _index);
        }

	//log
	memset(log_buf, 0, MAX_BUF_SIZE);
	char key_hex_buf[MAX_BUF_SIZE];
	memset(key_hex_buf, 0, MAX_BUF_SIZE);
	int key_buf_len = key_to_hex_buf((unsigned char*)buf+hlen, key_hex_buf,plen);
    	int index = sprintf(log_buf, "fake-diffie-hellman, forwarding,  circuit incoming: 0x%02x, key: 0x",iID);
	memcpy(log_buf+index, key_hex_buf, key_buf_len);
    	output_log(log_buf);
    }
    else
    {

	//the destination of this message, just remember the key
	memcpy(aes_key, buf+hlen, plen);
	char key_hex_buf[MAX_BUF_SIZE];
	memset(key_hex_buf, 0, MAX_BUF_SIZE);
	memset(log_buf, 0, MAX_BUF_SIZE);
	int key_buf_len = key_to_hex_buf((unsigned char*)buf+hlen, key_hex_buf,plen);
    	int index = sprintf(log_buf, "fake-diffie-hellman, new circuit incoming: 0x%02x, key: 0x",iID);
	memcpy(log_buf+index, key_hex_buf, key_buf_len);
    	output_log(log_buf);
    }

}



//handle circuit extend message
void CRouter::handle_ccext_msg(char* buf, int len, struct sockaddr_in si_other)
{
    char log_buf[MAX_BUF_SIZE];
    char send_buf[MAX_PACKET_SIZE];
    memset(send_buf,0, MAX_PACKET_SIZE);
    int nsend;

    struct iphdr *iph = (struct iphdr *)buf;
    struct cc_ext_msg * ccextmsg = (struct cc_ext_msg*)(iph+1);

    print_buf_hex((char*)ccextmsg, len-(sizeof(struct iphdr)), ntohs(si_other.sin_port));

    unsigned short iID = ntohs(ccextmsg->cid);
    unsigned short next_port;

    if(_stage == 5)
    {
    	next_port = ntohs(ccextmsg->next_hop);
	printf("**Router** %d, decrypted next hop: %d\n", _index, next_port);

    }

    //compute it's own circuit ID;
    unsigned short oID = compute_circuit_id(_index, _cc_seq);
    //header length (IP header + circuit extend header)
    int hlen = sizeof(struct iphdr) +sizeof( struct cc_encrypt_ext_msg );
    // payload length
    int plen = len - hlen;

    if(iID == cc._iid )
    {

	if(_stage == 6)
	{
	   
	    //decrypt the port number in the circuit extend message
	    char* decrypted_port = NULL;
	    int elen;
	    decrypt_msg_with_padding(buf+ hlen, plen, &decrypted_port, &elen, aes_key);
	    memcpy(buf+hlen, decrypted_port, elen);
	    delete [] decrypted_port;
	    len = hlen + elen;

	}

        //forward the message along the partial created path.
	ccextmsg->cid = htons(oID);
	struct sockaddr_in next_hop;
	next_hop.sin_family = AF_INET;
	next_hop.sin_port = htons(cc._oport);
	next_hop.sin_addr.s_addr =  htonl(INADDR_ANY);
	nsend = send_data_UDP(buf, len, next_hop);
        if( nsend <= 0)
        {
	    printf("**Router** %d, failed send circuit message via UDP\n", _index);
        }

	memset(log_buf, 0, MAX_BUF_SIZE);
	sprintf(log_buf, "forwarding extend circuit: incoming: 0x%02x, outgoing: 0x%02x at %d\n",cc._iid, cc._oid, cc._oport );
	output_log(log_buf);

	
    }
    else
    {
	if(_stage == 6)
	{

	    char * decrypted_port = NULL;
	    int elen;
	    //decrypt the port number in the circuit extend message
	    decrypt_msg_with_padding(buf+ hlen, plen, &decrypted_port, &elen, aes_key);
	    //get the port number (string to integer);
	    char * tport = new char [elen];
	    memset(tport, 0, elen);
	    memcpy(tport, decrypted_port, elen);
	    next_port = ntohs(atoi(tport));
	    delete [] tport;
	    delete [] decrypted_port;
	    printf("**Router** %d, decrypted next hop:%d\n", _index, next_port);
	}
	

	//new circuit, remember this circuit and send circuit extend done msg back;
        if(cc._iid == 0)
        {
	    cc._iid = iID;
	    cc._oid = oID;
	    cc._iport = ntohs(si_other.sin_port);
	    cc._oport = next_port;
        }

        memcpy(send_buf, buf, len);
        iph = (struct iphdr *)send_buf;
        struct cc_ext_done_msg * ccextdonemsg = (struct cc_ext_done_msg *)(iph+1); 
	//different message type for stage 5 and 6
	if(_stage == 5)
	    ccextdonemsg->msg_type =  CC_EXT_DONE_MSGTYPE;
	else
	    ccextdonemsg->msg_type = CC_ENCRYPTED_EXT_DONE;

	int packet_len = sizeof(struct iphdr) + sizeof(struct cc_ext_done_msg) ;
        nsend = send_data_UDP(send_buf, packet_len, si_other);
        if( nsend <= 0)
        {
	    printf("**Router** %d, failed send circuit message via UDP\n", _index);
        }

	memset(log_buf, 0, MAX_BUF_SIZE);
	sprintf(log_buf, "new extend circuit: incoming: 0x%02x, outgoing: 0x%02x at %d\n",cc._iid, cc._oid, cc._oport );
	output_log(log_buf);
        
    }
    	
}

//this function was wrote for stage 3, but it is actually no needed anymore.
void CRouter::self_reply_icmp(char* buf, int len, struct sockaddr_in si_other)
{

    unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr *)buf;
    iphdrlen = iph->ihl*4;
    struct icmphdr *icmph = (struct icmphdr *)(buf + iphdrlen);


    char log_buf[MAX_BUF_SIZE];
    struct sockaddr_in source,dest;
    char src_addr_buf[MAX_BUF_SIZE];
    char dst_addr_buf[MAX_BUF_SIZE];

    /* check if it is an ICMP packet */
    if(iph->protocol == 1)
    {

	printf("router:%d, self reply\n",_index);
	source.sin_addr.s_addr = iph->saddr;
	dest.sin_addr.s_addr = iph->daddr;
	memset(log_buf, 0, MAX_BUF_SIZE);
	memset(src_addr_buf, 0, MAX_BUF_SIZE);
	memset(dst_addr_buf, 0, MAX_BUF_SIZE);
	strcpy(src_addr_buf, inet_ntoa(source.sin_addr));
	strcpy(dst_addr_buf, inet_ntoa(dest.sin_addr));

	sprintf(log_buf, "ICMP from port: %d, src: %s, dst: %s, type: %d\n",ntohs(si_other.sin_port), src_addr_buf, dst_addr_buf, icmph->type);
	output_log(log_buf);
	   
	/* exchange src and dst */
	source.sin_addr.s_addr = iph->saddr;
	dest.sin_addr.s_addr = iph->daddr;
	iph->saddr = dest.sin_addr.s_addr;
	iph->daddr = source.sin_addr.s_addr;


	/* recompute ip header checksum */
	iph->check=0;
	unsigned short checksum = in_cksum((unsigned short*)iph, sizeof(struct iphdr));
	iph->check=checksum;


	/* change type to 0: icmp echo-reply */
	icmph->type = (unsigned int)0;

	/* recompute icmp header check sum */
	icmph->checksum=0;
	checksum = in_cksum((unsigned short*)icmph, sizeof(struct icmphdr));
	icmph->checksum=checksum;
	   

	/*send the packet back to proxy. */ 
	int nsend = send_data_UDP(buf, len, si_other);
	if( nsend <= 0)
	{
	    printf("**Router** %d, failed send packet via UDP\n", _index);
	}
    }
    else
    {
	printf("**Router** %d, received unknown packet\n", _index);

    }

}

//handle icmp packet from proxy
void CRouter::handle_proxy_icmp_traffic(char* buf, int len, struct sockaddr_in si_other)
{

    char log_buf[MAX_BUF_SIZE];
    struct sockaddr_in source,dest;
    char src_addr_buf[MAX_BUF_SIZE];
    char dst_addr_buf[MAX_BUF_SIZE];
    int nsend;

    memcpy(packet_buf, buf,len);
    packet_len = len;
    //print_icmp_packet(recv_buf,nread);
    unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr *)buf;

    iphdrlen = iph->ihl*4;
    struct icmphdr *icmph = (struct icmphdr *)(buf + iphdrlen);

    source.sin_addr.s_addr = iph->saddr;
    dest.sin_addr.s_addr = iph->daddr;
    
    memset(log_buf, 0, MAX_BUF_SIZE);
    memset(src_addr_buf, 0, MAX_BUF_SIZE);
    memset(dst_addr_buf, 0, MAX_BUF_SIZE);
    strcpy(src_addr_buf, inet_ntoa(source.sin_addr));
    strcpy(dst_addr_buf, inet_ntoa(dest.sin_addr));

    if(_stage<5)
    {
    	sprintf(log_buf, "ICMP from port: %d, src: %s, dst: %s, type: %d\n",ntohs(si_other.sin_port), src_addr_buf, dst_addr_buf, icmph->type);
	output_log(log_buf);
    }
	

    //send out the icmp packet through raw socket
    struct sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = dest.sin_addr.s_addr;
    nsend = send_ICMP_packet(serv);
    if(nsend<=0)
    {
	printf("**Router** %d, failed send packet via RAW socket\n", _index);
    }
}


//handle icmp packet from raw socket
void CRouter::handle_rawsock_icmp_traffic(char* buf, int len)
{

    char log_buf[MAX_BUF_SIZE];
    struct sockaddr_in source,dest;
    char src_addr_buf[MAX_BUF_SIZE];
    char dst_addr_buf[MAX_BUF_SIZE];
    int nsend;


    struct iphdr *iph = (struct iphdr *)buf;
    struct icmphdr *icmph = (struct icmphdr *)(iph+1);


    print_icmp_packet(buf,len);
    char reply_packet[MAX_PACKET_SIZE];
    memset(reply_packet,0, MAX_PACKET_SIZE);

		 		 
    //copy the original icmp packet;
    memcpy(reply_packet, packet_buf, MAX_PACKET_SIZE);
    struct iphdr *reply_iph = (struct iphdr *)reply_packet;
    struct icmphdr *reply_icmph = (struct icmphdr *)(reply_iph+1);

    source.sin_addr.s_addr = iph->saddr;
    dest.sin_addr.s_addr = iph->daddr;
    memset(log_buf, 0, MAX_BUF_SIZE);
    memset(src_addr_buf, 0, MAX_BUF_SIZE);
    memset(dst_addr_buf, 0, MAX_BUF_SIZE);
    strcpy(src_addr_buf, inet_ntoa(source.sin_addr));
    strcpy(dst_addr_buf, inet_ntoa(dest.sin_addr));

    reply_iph->daddr = reply_iph->saddr; 
    reply_iph->saddr = iph->saddr;

    reply_iph->check=0;
    unsigned short checksum = in_cksum((unsigned short*)reply_iph, sizeof(struct iphdr));
    reply_iph->check=checksum;

    reply_icmph->type = (unsigned int)0;
    reply_icmph->checksum=0;
    checksum = in_cksum((unsigned short*)reply_icmph, sizeof(struct icmphdr));
    reply_icmph->checksum=checksum;

    int old_plen = packet_len; 
    if(_stage<5)
    {
    	nsend = send_data_UDP(reply_packet, old_plen , _paddr);
	sprintf(log_buf, "ICMP from raw sock, src: %s, dst: %s, type: %d\n",src_addr_buf, dst_addr_buf, icmph->type);

    }
    else
    {
	char send_buf[MAX_BUF_SIZE];
	memset(send_buf, 0, MAX_BUF_SIZE);
	int new_packet_len = 0;
	if(_stage == 5)
	{
	    reply_iph->daddr = cc._iip;
	    new_packet_len  = construct_relay_msg(send_buf, MAX_PACKET_SIZE, cc._iid, reply_packet, old_plen, CC_RELAY_BACK_MSGTYPE, _stage);
	}

	if(_stage == 6)
	{
	    //zero the dst ip;
	    reply_iph->daddr = htonl(0);
	    //recompute checksume;
	    reply_iph->check = 0;
	    reply_iph->check = in_cksum((unsigned short*)reply_iph, sizeof(struct iphdr));  
	    //encrypt the packet
	    
	    char * encrypted_payload = NULL;
	    int olen;

	    //encrypt the packet
	    char * clear_payload = new char [old_plen];
	    memset(clear_payload, 0, old_plen);
	    memcpy(clear_payload, reply_packet, old_plen);
	    encrypt_msg_with_padding(clear_payload, old_plen, &encrypted_payload, &olen, aes_key);
	    memcpy(reply_packet, encrypted_payload, olen);
	    //construct the encrypted relay message.
	    new_packet_len  = construct_relay_msg(send_buf, MAX_PACKET_SIZE, cc._iid, reply_packet, olen, CC_ENCRYPTED_RELAY_REPLY, _stage);
	    delete [] encrypted_payload;
	    delete [] clear_payload;

	}


	struct sockaddr_in next_hop;
	next_hop.sin_family = AF_INET;
	next_hop.sin_port = htons(cc._iport);
	next_hop.sin_addr.s_addr =  htonl(INADDR_ANY);
	nsend = send_data_UDP(send_buf, new_packet_len , next_hop);
	sprintf(log_buf, "incoming packet, src:%s, dst: %s, outgoing circuit: 0x%02x\n", src_addr_buf, dst_addr_buf, cc._iid);


    }
    output_log(log_buf);
    if( nsend <= 0)
    {
	printf("**Router** %d, failed send packet via UDP\n", _index);
    }
}

void CRouter::run()
{
  /* use select() to handle three descriptors at once */
  int maxfd =  _mysock._sock > _icmp_sock._sock? _mysock._sock: _icmp_sock._sock;

  char recv_buf[MAX_PACKET_SIZE];
  int nread;

  while(1) {
    int ret;
    fd_set rd_set;

    FD_ZERO(&rd_set);
    FD_SET(_mysock._sock, &rd_set);
    FD_SET(_icmp_sock._sock, &rd_set);
    ret = select(maxfd + 1, &rd_set, NULL, NULL, NULL);

    if (ret < 0 && errno == EINTR){
      continue;
    }

    if (ret < 0) {
      perror("select()");
      exit(1);
    }

    // data from raw socket
    if(FD_ISSET(_icmp_sock._sock, &rd_set)) 
    {
	memset(recv_buf,0,MAX_PACKET_SIZE);
	struct sockaddr_in si_other;
	nread=recv_data_ICMP(recv_buf,si_other);
	if(nread > 0)
	{
	    
	     printf("**Router** %d, PID: %d, ICMP from raw socket, length: %d\n", _index, getpid(), nread);
	     struct iphdr *iph = (struct iphdr *)recv_buf;
	     if(iph->protocol == 1)
	     {
		 handle_rawsock_icmp_traffic(recv_buf, nread);
	     }
	}
    }

    // data from UDP socket
    if(FD_ISSET(_mysock._sock, &rd_set)) 
    {

    	memset(recv_buf,0,MAX_PACKET_SIZE);
    	struct sockaddr_in si_other;
    	nread = recv_data_UDP(recv_buf, si_other);
    	if(nread != 0) 
    	{
    	    struct iphdr *iph = (struct iphdr *)recv_buf;
	    
	    /* check if it is an ICMP packet */
	    if(iph->protocol == 1)
	    {

		printf("**Router** %d, PID: %d, ICMP from port: %d, length: %d\n", _index, getpid(), ntohs(si_other.sin_port), nread);
		if(iph->daddr == _rip)
		    self_reply_icmp(recv_buf,nread,si_other);
		else
		    handle_proxy_icmp_traffic(recv_buf,nread,si_other);
	    }
	    // circuit extend message 
	    else if (iph->protocol == CC_EXT_PROTOCOL)
	    {

		printf("**Router** %d, PID: %d, CIRCUIT Msg from port: %d, length: %d\n", _index, getpid(), ntohs(si_other.sin_port), nread);

		struct cc_ext_msg * ccextmsg = (struct cc_ext_msg*)(iph+1);
		if(ccextmsg->msg_type == FAKE_DIFFIE_HELLMAN)
		{
		    handle_deffie_hellman_msg(recv_buf,nread,si_other);
		}
		if(ccextmsg->msg_type == CC_EXT_MSGTYPE || ccextmsg->msg_type == CC_ENCRYPTED_EXT)
		{
		    handle_ccext_msg(recv_buf,nread,si_other);
		}

		if(ccextmsg->msg_type == CC_EXT_DONE_MSGTYPE || ccextmsg->msg_type == CC_ENCRYPTED_EXT_DONE)
		{
		    handle_ccext_done_msg(recv_buf,nread,si_other);
		}

		if(ccextmsg->msg_type == CC_RELAY_MSGTYPE || ccextmsg->msg_type == CC_RELAY_BACK_MSGTYPE || ccextmsg->msg_type == CC_ENCRYPTED_RELAY || ccextmsg->msg_type == CC_ENCRYPTED_RELAY_REPLY)
		{
		    handle_relay_msg(recv_buf,nread,si_other);
		}

	    }
	}
    }
  }
} 


void CRouter::output_log(char* out_str)
{
    FILE* logfp=fopen(_logfn,"a");
    if(!logfp)
    {
	printf("Open Log File:%s failed \n",_logfn);
	return;

    }
    fputs(out_str,logfp);
    fflush(logfp);
    fclose(logfp);
}

void CRouter::print_buf_hex(char* buf, int buf_len, int port)
{
    char log_buf[MAX_BUF_SIZE];
    memset(log_buf, 0, MAX_BUF_SIZE);
    int index=0;
    index += sprintf(log_buf, "pkt from port: %d, length: %d, contents: 0x", port, buf_len);
    for(int i=0; i<buf_len; i++)
    {
	index += sprintf(log_buf+index, "%02x", (unsigned char)buf[i]);
    }
    sprintf(log_buf+index, "\n");
    output_log(log_buf);
}

