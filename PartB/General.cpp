#include "General.h"
#define SERVER "nunki.usc.edu"

using namespace std;

int main(int argc, char *argv[])
{     

  //reading passGn.txt file my code
  int a, b, c, t;
  int g = 1;
  char s1[10];
  char s2[10];
  char *pch;
  char mport[5];
  char password[20];
  char check[40];
  char bufmod[40];
  ifstream ifile("passGn.txt");
    
  ifile >> mport;
  ifile >> password;

  strcpy(check, mport);
  strcat(check, password);

  //the following code is copied from Beej Guide p. 19-20
  int sockfd, sockbind;
  int status;
  int bytes_sent;
  struct addrinfo hints;
  struct addrinfo *res; //will point to the results
  
  struct hostent *he;
  char buf[50];
  struct sockaddr_storage their_addr;
  socklen_t addr_len;
  struct in_addr **addr_list;
  int i;
  //char **sa_data;
  struct sockaddr_in sin;
  socklen_t addrlen1 = sizeof(sin);
  int local_port;

  memset(&hints, 0, sizeof hints); // make sure the struct is empty
  
  hints.ai_family = AF_UNSPEC; // don't care IPV4 or IPv6
  hints.ai_socktype = SOCK_DGRAM; // UDP stream sockets
  hints.ai_flags = AI_PASSIVE; // fill in my IP for me
  //TODO: Check Port number 3830
  status = getaddrinfo(NULL, "3830", &hints, &res);
  if (status != 0){
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    return 1;
    
  }
  else{

    //inet_ntoa(addr);

    //mycode
    //TODO error-checking on getaddrinfo() p.22
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    
    if(sockfd == -1){
      perror("socket error");
    }
    
    bind(sockfd, res->ai_addr, res->ai_addrlen);
    
    if(sockbind == -1){
      perror("bind error");
      return 1;
    }
    
    freeaddrinfo(res);
    
    //TODO cout <Major> is now connected to the Captain#
        

    int get = getsockname(sockfd, (struct sockaddr *)&sin, &addrlen1);

    if(get == 0 && sin.sin_family == AF_INET && addrlen1 ==sizeof(sin)){
      local_port = ntohs(sin.sin_port);
    }

    he = gethostbyname("nunki.usc.edu");

    //TODO finish display
    cout << "<General> has UDP port ";


    cout << "3830";   //local_port?

    /*
    sa_data = (char **)((res->ai_addr)->sa_data);
    for(i = 0; sa_data[i] != NULL; i++){
      printf("%s ",ntohs(*sa_data[i])); 
    }
    */

    cout  << " and IP address ";

    addr_list = (struct in_addr **)he->h_addr_list;
    for(i = 0; addr_list[i] != NULL; i++) {
      printf("%s ", inet_ntoa(*addr_list[i]));
    }
    
   cout << "for Phase 2" << endl;

while(g){
  bytes_sent = recvfrom(sockfd, buf, 50, 0, (struct sockaddr *)&their_addr, &addr_len);

//cout << "this is buf: " << buf << endl;
//cout << "this is check: " << check << endl;

  if(bytes_sent == -1){
	perror("recvfrom error");
	exit(1);
      }
  else if(bytes_sent != -1){
     cout << "Received the password from <Major>" << endl;
   }
  pch = strchr(buf, ' ');
  a = pch- buf;
  for(b = 0; b < a; b++){
    bufmod[b] = buf[b];
    }
  bufmod[b] = NULL;

//cout << "this is bufmod: " << bufmod << endl; 
//cout << "this is a: " << a << endl;
  if( !strcmp(check, bufmod) ) {
	cout << "Correct password from <Major>" << endl;
	pch = strchr(pch+1, ' ');
	c = pch-buf-a;
	//cout << "this is c: " << c << endl;
	//cout << "buf[b+a+1]: " << buf[15] << buf[16] << endl;
  	for(b = 0; b < (c-1); b++){
    		s1[b] = buf[(b+a+1)];
		//cout << "this is s1[b] " << s1[b] << endl;
    	}
	s1[b] = NULL;
	cout << "<Captain 1> has P(success rate) = " << s1 << "#" << endl;

	//cout << s1 << endl;
	pch = strchr(pch+1, ' ');
	t = pch-buf-c-a;
	//cout << "t-1 = " << (t-1) << endl;
  	for(b = 0; b < (t-1); b++){
    		s2[b] = buf[(b+a+c+1)];
    	}
	s2[b] = NULL;
	cout << "<Captain 2> has P(success rate) = " << s2 << "#" << endl;

	//cout << s1 << s2 << endl;
	g = 0;
}
  else if(strcmp(check, bufmod)){
       cout << "Wrong password from <Major>, try again" << endl;
   }
}



// %%%%%%%%%%%%%%%%%%%%%

/*
    while((tab && bat) != 1){
      bytes_sent = recvfrom(sockfd, buf, 40, 0, (struct sockaddr *)&their_addr, &addr_len);
      
      if(bytes_sent == -1){
	perror("recvfrom error");
	exit(1);
      }
      count = count + 1;

      if((int)buf[4] == 56){
	cout <<"<Major> is now connected to the Captain1#" << endl;
	cout << "<Major> has received the following:" << endl;
	cout << count << ". ";
	cout << "<Captain1#>: Resources" << buf[0] << "#, Confidence" << buf[2] << "#, Record8#" <<  endl;
	tab = 1;
      }
      else if((int)buf[4] == 57){
	cout << "<Major> is now connected to Captain2#" << endl;
	cout << "<Major> has received the following:" << endl;
	cout << count << ". ";
	cout << "<Captain2#>: Resources" << buf[0] << "#, Confidence" << buf[2] << "# Record9#" <<  endl;
	bat = 1;
      }
      
      
    }
*/

// %%%%%%%%%%%%%%%%%%%%%

    close(sockfd);
    cout << "End of Phase 2 for <General>" << endl;
    
     return 0; //return 0 for int main funciton
     
  } //endl of else statement
}
