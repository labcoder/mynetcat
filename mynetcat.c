/*
 * program:     mynetcat
 * author:      oscar sanchez (oms1005@gmail.com)
 * description: mynetcat is a simple netcat where you can use it as a simple
 *              UDP echo server/client depending on what arguments you give it.
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#define MaxMessageLength 256
char* usageMessage="Usage:\nserver-mode:\n  ./mynetcat -l -u -p port [-C]\nclient-mode:\n  ./mynetcat -p port -u -h host <things to send>\n";

int main(int argc, char ** argv) {

  int l=0;                    //if enabled, then it's in listening mode (server)
  int hexDump=0;              //if -C is enabled in client, then hex is printed
  int port=0;                 //port connecting to/from
  char* host="\0";            //host name
  char c;                     //used for argument processing
  
  while((c=getopt(argc,argv,"lCup:h:"))!=-1)
    switch(c) {
      case 'l':               //we're in listening mode
        l=1;
        break;
      case 'u':               //already know that I'm accepting UDP packets...
        break;
      case 'C':               //print hex output
        hexDump=1;
        break;
      case 'p':
        port=atoi(optarg);    //the port we're binding to
        break;
      case 'h':
        host=optarg;          //the host address
        break;
      case '?':               //error checking
        fprintf(stderr,usageMessage);
        return 1;
      default:
        abort();              //something went wrong... 
    }
  if(port==0 || (l==0 && strlen(host)<5)) {
    printf(usageMessage);
    return 1;
  }
  
  /* Server Mode */
  if(l) {
    int sd, rc, n, cliLen, flags;
    struct sockaddr_in cliAddr, servAddr;
    char msg[MaxMessageLength];
    
    /* socket creation */
    sd=socket(AF_INET, SOCK_DGRAM, 0);
    if(sd<0) {
      printf("%s: cannot open socket\n",argv[0]);
      exit(1);
    }
    
    /* bind server port */
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
    rc = bind (sd, (struct sockaddr *) &servAddr,sizeof(servAddr));
    if(rc<0) {
      printf("%s: cannot bind port number %d \n", 
	     argv[0], port);
      exit(1);
    }
    
    printf("Waiting on port %d for incoming packets...\n",port);
    
    flags=0;
    while(1) { //keep server running...
      memset(msg,0x0,MaxMessageLength);
      
      /* receive message */
      cliLen = sizeof(cliAddr);
      n = recvfrom(sd, msg, MaxMessageLength, flags,
		   (struct sockaddr *) &cliAddr, &cliLen); 
		  if(n<0) {
        printf("%s: cannot receive data\n",argv[0]);
        continue;
      }
		  /* print received message */
      printf("%s: from %s:UDP%u : %s \n", argv[0],inet_ntoa(cliAddr.sin_addr),
	      ntohs(cliAddr.sin_port),msg);
		  /* if hexdump, print again in hex */
		  if(hexDump) {
		    printf("In HEX: %x\n",msg);
		  }
		  
		  sleep(1);
      sendto(sd,msg,n,flags,(struct sockaddr *)&cliAddr,cliLen);
    }
  }
  /* Client Mode */
  else {
    int sd, rc, i, n, echoLen, flags;
    struct sockaddr_in cliAddr, remoteServAddr, echoServAddr;
    struct hostent *h;
    char msg[MaxMessageLength];
    
    h = gethostbyname(host);
    if(h==NULL) {
      printf("%s: unknown host '%s' \n", argv[0], host);
      exit(1);
    }
    printf("%s: sending data to '%s' (IP : %s) \n", argv[0], h->h_name,
	  inet_ntoa(*(struct in_addr *)h->h_addr_list[0]));
    remoteServAddr.sin_family = h->h_addrtype;
    memcpy((char *) &remoteServAddr.sin_addr.s_addr, 
	   h->h_addr_list[0], h->h_length);
    remoteServAddr.sin_port = htons(port);
    
    /* socket creation */
    sd = socket(AF_INET,SOCK_DGRAM,0);
    if(sd<0) {
     printf("%s: cannot open socket \n",argv[0]);
     exit(1);
    }
    
    /* bind any port */
    cliAddr.sin_family = AF_INET;
    cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    cliAddr.sin_port = htons(0);
    rc = bind(sd, (struct sockaddr *) &cliAddr, sizeof(cliAddr));
    if(rc<0) {
      printf("%s: cannot bind port\n", argv[0]);
      exit(1);
    }
    
    flags = 0;
    
    /* send data */
    for(i=6;i<argc;i++) {
      rc = sendto(sd, argv[i], strlen(argv[i])+1, flags, 
		  (struct sockaddr *) &remoteServAddr, 
		  sizeof(remoteServAddr));

      if(rc<0) {
        printf("%s: cannot send data %d \n",argv[0],i-1);
        close(sd);
        exit(1);
      }
      
      memset(msg,0x0,MaxMessageLength);
      /* receive echoed message */
      echoLen = sizeof(echoServAddr);
      n = recvfrom(sd, msg, MaxMessageLength, flags,
        (struct sockaddr *) &echoServAddr, &echoLen);
      if(n<0) {
        printf("%s: cannot receive data \n",argv[0]);
        continue;
      }

      /* print received message */
      printf("%s: echo from %s:UDP%u : %s \n", 
        argv[0],inet_ntoa(echoServAddr.sin_addr),
        ntohs(echoServAddr.sin_port),msg);
      }
  }
  return 0;
}













