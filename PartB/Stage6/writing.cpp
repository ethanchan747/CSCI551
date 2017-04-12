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
    memcpy(circuit+increment,rk2,sizeof(rk2));
    increment = increment + sizeof(rk2);
    
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
    
    ofile.open("stage5.proxy.out",std::fstream::app);
    ofile << "new-fake-diffie-hellman, router index: 1, circuit outgoing: 0x01, key: 0x";
    
    for(int z = 0; z < 16; z++){
        uint16_t shade;
        memset(&shade,0,2);
        shade = (uint16_t)rk2[z];
        if((!z)&&(shade < 0x000f)){
            ofile << "0";
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
    
    class_AES_set_encrypt_key(rk2, &enc_key);
    class_AES_encrypt_with_padding(testd, clear_text_len, &crypt_text, &crypt_text_len, &enc_key);
    
    
    memset(circuit, 0, sizeof(circuit));
    
    increment = 0;
    memcpy(circuit, &minhdr, sizeof(minhdr));
    increment = sizeof(minhdr);
    memcpy(circuit+increment,&type,sizeof(type));
    increment = increment + sizeof(type);
    memcpy(circuit+increment,&netwc_id,sizeof(netwc_id));
    increment = increment + sizeof(netwc_id);
    memcpy(circuit+increment,crypt_text,strlen((const char*)crypt_text));
    increment = increment + strlen((const char*)crypt_text);
    
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
    
    
    
    
    
    
    
    
   // ************************************************************
    
    char circuit[2048];
    memset(circuit, 0, sizeof(circuit));
    
    struct ip minhdr;
    memset(&minhdr, 0, sizeof(minhdr));
    
    inet_pton(AF_INET, "127.0.0.1", &(minhdr.ip_src)); //stores minitor IP in_addr
    inet_pton(AF_INET, "127.0.0.1", &(minhdr.ip_dst));
    
    minhdr.ip_p = 253;
    
    
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
    
    
    int increment = 0;
    memcpy(circuit, &minhdr, sizeof(minhdr));
    increment = sizeof(minhdr);
    memcpy(circuit+increment,&type,sizeof(type));
    increment = increment + sizeof(type);
    memcpy(circuit+increment,&netwc_id,sizeof(netwc_id));
    increment = increment + sizeof(netwc_id);
    memcpy(circuit+increment,&netwport,sizeof(netwport));
    increment = increment + sizeof(netwport);
    
    
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
     
     ofile << endl;
     ofile << "strlen of minitorbuf is: " << daf << endl;
     
     ofile.close();
     */
    sendto(sockfd, circuit, increment, 0, (struct sockaddr *)&their_addr, sizeof(their_addr));

    
