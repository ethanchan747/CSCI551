#include "Major.h"
#define SERVER "nunki.usc.edu"

using namespace std;

int main(int argc, char *argv[])
{     

  //reading passMj.txt file my code
  char *pch;
  int a, b, c, t;
  char mport[5];
  char password[20];
  char msg[50];
  char s1[10];
  char s2[10];

//cout << "this is msg: " << msg << endl;

/*
**********************************
  addr_len = sizeof(their_addr);
  bytes_sent = recvfrom(sockfd, newbuf, 5, 0, (struct sockaddr *)&their_addr, &addr_len);

************************************
*/

  //the following code is copied from Beej Guide p. 19-20
  int once = 1;
  int sockfd, sockbind, sockfdmajor;
  int status;
  int bytes_sent;
  struct addrinfo hints;
  struct addrinfo *res; //will point to the results
  
  //char z[2];
  char d[2] = "y";
  struct hostent *he;
  char buf[20], rec[5], re[8], co[8], r1[8], c1[8], r2[8], c2[8];
  struct sockaddr_storage their_addr;
  socklen_t addr_len;
  struct in_addr **addr_list;
  int i, success1, success2;
  //char **sa_data;
  struct sockaddr_in sin;
  socklen_t addrlen1 = sizeof(sin);
  int local_port;
  int tab = 0;
  int bat = 0;
  int count = 0;

  memset(&hints, 0, sizeof hints); // make sure the struct is empty
  
  hints.ai_family = AF_UNSPEC; // don't care IPV4 or IPv6
  hints.ai_socktype = SOCK_DGRAM; // UDP stream sockets
  //TODO: Check Port number 3430
  status = getaddrinfo("nunki.usc.edu", "3430", &hints, &res);
  if (status != 0){
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
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
    
    freeaddrinfo(res);	//This is done earlier than the Captains because we don't need res
    
    //TODO cout <Major> is now connected to the Captain#
        
cout << "Preparing for Phase 1" << endl;
cout << endl;
sleep(2);

cout << "Major is setting up" << endl;
sleep(2);


    int get = getsockname(sockfd, (struct sockaddr *)&sin, &addrlen1);

    if(get == 0 && sin.sin_family == AF_INET && addrlen1 ==sizeof(sin)){
      local_port = ntohs(sin.sin_port);
    }

    he = gethostbyname("nunki.usc.edu");

    //TODO finish display
    cout << "<Major> has UDP port ";


    cout << "3430";   //local_port?

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

    addr_len = sizeof(their_addr);

    while((tab && bat) != 1){

      bytes_sent = recvfrom(sockfd, buf, 20, 0, (struct sockaddr *)&their_addr, &addr_len);
      //cout << "this is buf: " << buf << endl;

      if(bytes_sent == -1){
	perror("recvfrom error");
	exit(1);
      }

      count = count + 1;
	//cout << "flag0" << endl;
	pch = strchr(buf, ' ');
	a = pch- buf;
 	for(b = 0; b < a; b++){
   	 	re[b] = buf[b];
    	}
 	re[b] = NULL;
	//cout << "this is re: " << re << " done " << endl;
//cout << "this is bufmod: " << bufmod << endl; 
//cout << "this is a: " << a << endl;
//cout << "flag1" << endl;
	pch = strchr(pch+1, ' ');
	c = pch-buf-a;
	//cout << "this is c: " << c << endl;
	//cout << "buf[b+a+1]: " << buf[15] << buf[16] << endl;
  	for(b = 0; b < (c-1); b++){
    		co[b] = buf[(b+a+1)];
		//cout << "this is s1[b] " << s1[b] << endl;
    	}
	co[b] = NULL;
//cout << "this is co: " << co << " done"<< endl;
//cout << "flag2" << endl;
	//cout << s1 << endl;
	pch = strchr(pch+1, ' ');
	t = pch-buf-c-a;
	//cout << "t = " << t << endl;
	//cout << "t-1 = " << (t-1) << endl;
  	for(b = 0; b < (t-1); b++){
    		rec[b] = buf[(b+a+c+1)];
    	}
	rec[b] = NULL;
//cout << "flag3" << endl;
      if((int)rec[(b-1)] == 56){
	cout <<"<Major> is now connected to the Captain1#" << endl;
	cout << "<Major> has received the following:" << endl;
	cout << count << ". ";
	/*r1[0] = buf[0];
	r1[1] = NULL;
	c1[0] = buf[2];
	c1[1] = NULL;*/
	strcpy(r1,re);
	strcpy(c1,co);
	//cout << "r1 is: " << r1 << endl;
	//cout << "c1 is: " << c1 << endl;
	success1 = 5*(atoi(r1)) + 3*(atoi(c1)) + 2*8;
	//p = 5 * Resources + 3 * Confidence + 2* Record
	cout << "<Captain1#>: Resources" << r1 << "#, SuccessRate " << success1 << "# Record8#" <<  endl;
	tab = 1;
	//cout << "flag4" << endl;
      }
      else if((int)rec[(b-1)] == 57){
	cout << "<Major> is now connected to Captain2#" << endl;
	cout << "<Major> has received the following:" << endl;
	cout << count << ". ";
	/*r2[0] = buf[0];
	r2[1] = NULL;
	c2[0] = buf[2];
	c2[1] = NULL;*/
	strcpy(r2,re);
	strcpy(c2,co);
	//cout << "r2[0]: " << r2[0] << " c2[0]: " << c2[0] << endl;
	success2 = 5*(atoi(r2)) + 3*(atoi(c2)) + 2*9;
	cout << "<Captain2#>: Resources" << r2 << "#, SuccessRate" << success2 << "# Record9#" <<  endl;
	bat = 1;
//cout << "flag5" << endl;
      }
      
      
    }

//cout << "this is msg: " << msg << endl;

    close(sockfd);
    cout << "End of Phase 1 for <Major>" << endl;
    cout << endl;
    
// ***************************************************************************************

cout << "Preparing for Phase 2" << endl;
sleep(10);

//get ready to connect for PhaseII

while(!(strcmp(d,"y"))){

  ifstream ifile("passMj.txt");
    

  ifile >> mport;
  ifile >> password;

  strcpy(msg, mport);
  strcat(msg, password);
  strcat(msg," ");

string Result;
ostringstream convert;
convert << success1;
Result = convert.str();
char *cstr1 = new char [Result.length()+1];
strcpy(cstr1,Result.c_str());
strcpy(s1,cstr1);
delete[] cstr1;

string Result2;
ostringstream convert2;
convert2 << success2;
Result2 = convert2.str();
char *cstr2 = new char [Result2.length()+1];
strcpy(cstr2,Result2.c_str());
strcpy(s2,cstr2);
delete[] cstr2;

  //itoa (success1, s1, 10);
  //itoa (success2, s2, 10);
  strcat(msg, s1);
  strcat(msg," ");
  strcat(msg, s2);
  strcat(msg, " ");

//cout << "this is msg: " << msg << endl;

  memset(&hints, 0, sizeof hints); // make sure the struct is empty
  hints.ai_family = AF_UNSPEC;    // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_DGRAM; //UDP stream sockets


if(once){
//*******************Hardcoded Major port
  status = getaddrinfo("nunki.usc.edu", "3530", &hints, &res); //hardcodes Major' Port address = 3400 + 130

  if(status != 0){
    cout << "getaddrinfo error" << endl;
    return 1;
  }
  //servinfo now points to a link list of 1 or more struct adrinfos
  
  //etc.
  //end of copied code from Beej Manual p.20
  
  sockfdmajor = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  if(sockfdmajor == -1){
    perror("socket error");
  }

    bind(sockfdmajor, res->ai_addr, res->ai_addrlen);
    
    if(sockbind == -1){
      perror("bind error");
      return 1;
    }
    
   get = getsockname(sockfdmajor, (struct sockaddr *)&sin, &addrlen1);

    if(get == 0 && sin.sin_family == AF_INET && addrlen1 ==sizeof(sin)){
      local_port = ntohs(sin.sin_port);
    }

    freeaddrinfo(res);
//*********************Hardcoded Major Port
once = 0;
}

  memset(&hints, 0, sizeof hints); // make sure the struct is empty
  hints.ai_family = AF_UNSPEC;    // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_DGRAM; //UDP stream sockets

  status = getaddrinfo("nunki.usc.edu", "3830", &hints, &res); //hardcodes General's IP address = 3700 + 130

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
  cout << "Do you want to connect to General? (y/n)" << endl;
  cin >> z;


  //TODO finish disconnect (close) in if statement
  if(strcmp(z,"n") == 0){
    cout << "<Major> disconnects from <General>" << endl;
  }
  else if(strcmp(z,"y") == 0){*/

cout << endl;
cout << "Major is setting up" << endl;
sleep(7);

    bytes_sent = sendto(sockfd, msg, 50, 0, res->ai_addr,res->ai_addrlen); 


//This is for dynamic ports
/*
   get = getsockname(sockfd, (struct sockaddr *)&sin, &addrlen1);

    if(get == 0 && sin.sin_family == AF_INET && addrlen1 ==sizeof(sin)){
      local_port = ntohs(sin.sin_port);
    }
*/
    he = gethostbyname("nunki.usc.edu");

    //TODO finish display
    cout << "<Major> has UDP port ";


    cout << local_port;  //3530

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

  cout << "< Captain 1> has P(success rate) = " << success1 << "#" << endl;
  cout << "< Captain 2> has P(success rate) = " << success2 << "#" << endl;

    cout << "<Major> sent the P(success rate) to the General" << endl;
cout << "Please wait for General's response" << endl;


    if(bytes_sent == -1){
      perror("sendto error");
      exit(1);
    } 
    
  freeaddrinfo(res);

  cout << "If password was incorrect, do you want to try resending password to general? (y/n)" << endl;
  cin >> d;

}



  cout << "End of Phase 2 for <Major>" << endl;
  cout << endl;

     close(sockfdmajor);
     close(sockfd);
     return 0; //return 0 for int main funciton
     
}
