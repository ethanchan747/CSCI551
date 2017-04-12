#include "Captain1.h"
#define SERVER "nunki.usc.edu"

using namespace std;

int main(int argc, char *argv[])
{
  //reading Captain1.txt file my code
  char str1[15];
  char str2[15];
  char num1[8];
  char num2[8];
  char r[8];
  char c[8];
  char msg[20];
  ifstream ifile("Captain1.txt");
    
  ifile.getline(str1,15,'$');
  ifile >> num1;
  ifile.getline(str2,15,'$');
  ifile >> num2;
  

  if(strcmp(str1,"Resources") == 0){
    strcpy(r,num1);
    strcpy(c,num2);
  }
  else{
    strcpy(r,num2);
    strcpy(c,num1);
  }
  strcpy(msg, r);
  strcat(msg," ");
  strcat(msg,c);
  strcat(msg," ");
  strcat(msg,"8");
  strcat(msg," ");
  

  //cout << "This is msg " << msg << endl;

  //Run value through a function set it to Network Byte Order (Big-Endian MSB on far left page11
  //host to network, network to host conversion page 12
  //do not have bind fucntion for Captain since it is dynamically allocated
  //perhaps error checking is needed for info getaddrinfo() returns p.19

  //Copied code from Beej Manual p.20 of Beej Manual
  int sockfd, sockbind, listenfd, new_fd;
  int status;
  int bytes_sent, bytes_recv;
  //char z[2];
  struct addrinfo hints;
  struct addrinfo *res; // will point to the results

  memset(&hints, 0, sizeof hints); // make sure the struct is empty
  hints.ai_family = AF_UNSPEC;    // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_DGRAM; //UDP stream sockets
  struct hostent *he;
  char buf[5];
  struct sockaddr_storage their_addr;
  socklen_t addr_len;
  struct in_addr **addr_list;
  int i, get;
  struct sockaddr_in sin;
  socklen_t addrlen1 = sizeof(sin);
  int local_port;

  //get ready to connect
  status = getaddrinfo("nunki.usc.edu", "3430", &hints, &res); //hardcodes Major's IP address = 3300 + 130

  if(status != 0){
    cout << "getaddrinfo error" << endl;
    return 1;
  }
  //servinfo now points to a link list of 1 or more struct adrinfos
  
  //etc.
  //end of copied code from Beej Manual p.20
  
  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  if(sockfd == -1){
    perror("socket error");
  }

  /*
  cout << "Do you want to connect to Major? (y/n)" << endl;
  cin >> z;


  //TODO finish disconnect (close) in if statement
  if(strcmp(z,"n") == 0){
    cout << "<Captain1#> disconnects from <Major>" << endl;

  }
  else if(strcmp(z,"y") == 0){*/

cout << "Preparing for Phase 1" << endl;
cout << endl;
sleep(2);

   cout << "<Captain#1> is setting up" << endl;
   sleep(2);

    bytes_sent = sendto(sockfd, msg, 20, 0, res->ai_addr,res->ai_addrlen); 


    get = getsockname(sockfd, (struct sockaddr *)&sin, &addrlen1);

    if(get == 0 && sin.sin_family == AF_INET && addrlen1 ==sizeof(sin)){
      local_port = ntohs(sin.sin_port);
    }

    he = gethostbyname("nunki.usc.edu");

    //TODO finish display
    cout << "<Captain#1> has UDP port ";


    cout << local_port;

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

    cout << "for Phase 1" << endl;

    cout << "Sending log file to the <Major>" << endl;

    if(bytes_sent == -1){
      perror("sendto error");
      exit(1);
    } 
    //}
  
  
  freeaddrinfo(res);
  close(sockfd);
  cout << "End of Phase 1 for <Captain1#>" << endl;
  cout << endl;
  
//*********************************************PhaseIII


  memset(&hints, 0, sizeof hints); // make sure the struct is empty
  hints.ai_family = AF_UNSPEC;    // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; //TCP stream sockets

status = getaddrinfo("nunki.usc.edu", "21330", &hints, &res); //21200 + 130
  if(status != 0){
    cout << "getaddrinfo error" << endl;
    return 1;
  }

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

    get = getsockname(sockfd, (struct sockaddr *)&sin, &addrlen1);

    if(get == 0 && sin.sin_family == AF_INET && addrlen1 ==sizeof(sin)){
      local_port = ntohs(sin.sin_port);
    }


    he = gethostbyname("nunki.usc.edu");

    //TODO finish display
    cout << "<Captain#1 has TCP port ";


    cout << "21330";   //local_port?

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
 
    cout << "for Phase 3" << endl;

cout << endl;
cout << "<Captain#1> is idle" << endl;
cout << endl;


    freeaddrinfo(res);
    listenfd = listen(sockfd, 1); // check int backlog

    if(listenfd == -1){
      perror("listen error");
      return 1;
    }        

   addr_len = sizeof(their_addr);
   new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_len);
   close(sockfd); //close listening socket since there is only one connection


   bytes_recv = recv(new_fd, buf, 5, 0);

      if(bytes_recv == -1){
	perror("recvfrom error");
	exit(1);
      }

    cout << "<Captain#1> is now connected to General" << endl;
    
    if(buf[0]){
	cout << "<Captain#1 has started the Mission!!!!!!" << endl;
    }
    else if(!buf[0]){
	cout << "<Captain#1 is back up for the mission!" << endl;
    }

    cout << "End of Phase 3 for Captain#1" << endl;

  close(new_fd);
  return 0; //return 0 for int main funciton

}
