#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>

using namespace std;

int clientCount;
int bport[3] = {1050, 1060, 1070};
int sockfd;
struct sockaddr_in serv_addr, cli_addr;


void error(const char *msg);

class customer
{
public:
    long int num1;  //Account Number
	long int num2;    //Balance
	
	pthread_mutex_t mutex;
	void lock() 
	{
		pthread_mutex_lock(&mutex);
	}
	void unlock()
	{
		pthread_mutex_unlock(&mutex);
	}
	
};

vector<customer> customer_list; //Creating Vector of objects for customer class

void *serve(void *threadid)
{   
  long tid; //variable used to differentiate each thread
  tid = (long)threadid;
  int bsock[] = {0,0,0};
  fd_set readSockSet;
  timeval timeout;


  //Socket to communicate with client
  int newsockfd; //Socket descriptor used for accept function
    string log;
    struct sockaddr_in cli_addr;
    socklen_t   clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
  if (newsockfd < 0) 
          error("ERROR on accept");

  //For every client create connection to back end servers
 
  for(int i=0; i<3; i++)
  {
    if(bport[i] != 0)
    {
      bsock[i] = socket(AF_INET, SOCK_STREAM, 0);

      struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
   
  server_address.sin_port = htons(bport[i]);
  inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

  if(connect(bsock[i], (struct sockaddr*) &server_address, sizeof(server_address)) == -1)
  {
    printf("conection error\n");
   // return -1;
  }
    }
    
  }

//Start 2pc protocol

  while(1)
{
  
  //Send command
  char buffer[256];
  bzero(buffer,256); //clearing buffer
  cout<<"waiting for client \n";
   int n = read(newsockfd,buffer,255); //Reading data from client
     if (n < 0) error("ERROR reading from socket");
    cout<<"client sent::"<<endl;
    cout<<buffer<<endl;
    string check(buffer);
    
    for(int i=0; i<3; i++)
    {
      if(bport[i] != 0)
      {
        //cout<<"bsock value"<<bsock[i]<<endl;
        send(bsock[i], buffer , sizeof(buffer) ,0);
        //send(newsockfd, st.c_str() , st.length() ,0);
      }
    
    }
 string temp = "vote_request";
 
    for(int i=0; i<3; i++)
    {
      if(bport[i] != 0)
      {
        //cout<<"bsock value"<<bsock[i]<<endl;
        send(bsock[i], temp.c_str() , temp.length() ,0);
      }
    
    }
 
    cout<<"receive votes \n";
    //Receive votes
    int flag2 = 0;
    int max1 = max(bsock[0], bsock[1]);
    int max2 = max(max1, bsock[2]);
    
    timeout.tv_sec = 5;
    timeout.tv_usec = 5;
    

    
    for(int i=0; i<3; i++)
    {
      if(bport[i] != 0)
      {
        //cout<<"bsock value"<<bsock[i]<<endl;
        //if (retval > 0)
          FD_ZERO(&readSockSet);
          FD_SET(bsock[i], &readSockSet);          
          int retval = select(max2+1, &readSockSet, NULL, NULL, &timeout);
          if (FD_ISSET(bsock[i], &readSockSet))
          {
            bzero(buffer,256);
            read(bsock[i],buffer,255);
            cout<<buffer<<endl;
            string ch1(buffer);
            if(ch1.compare("vote") != 0)
           {
            flag2 = 1;
           }
          }
          else
          {
            cout<<"TimeOut (one backend may be down) \n";
            bport[i] = 0;
            flag2 = 1;
          }
        
        
      }
    }
    cout<<"flag value:"<<flag2<<endl;
    if(flag2 == 0)
    {
      //send commit
      temp = "commit";
      //bzero(buffer,256);
      //temp.copy(buffer,temp.length(),0);

      for(int i=0; i<3; i++)
      {
       if(bport[i] != 0)
      {
        //cout<<"bsock value"<<bsock[i]<<endl;
        //send(bsock[i], buffer , strlen(buffer) ,0);
        send(bsock[i], temp.c_str() , temp.length() ,0);
      }
    
    }

    //read and send command back to client
    char cmd[256];
    memset(&cmd,0,256);
    for(int i =0; i<3; i++)
    {
      if(bport[i] != 0)
      {
        read(bsock[i], cmd, 255);
      }
    }
    cout<<cmd<<endl;
    

    send(newsockfd, cmd, 256 ,0);

    }
    
    //string test = "testinguu";
    
    
    //send(newsockfd, test.c_str(), test.length() ,0);

    if(flag2 ==1)
    {
      //send abort
      temp = "abort";
      for(int i=0; i<3; i++)
      {
        if(bport[i] != 0)
        send(bsock[i], temp.c_str() , temp.length() ,0);  
      }
      temp = "Transaction aborted try again if necessary.";
      send(newsockfd, temp.c_str() , temp.length() ,0);
    }

}

  
        close(newsockfd);
        close(sockfd);
        pthread_exit(NULL);

}
void error(const char *msg)
{
    perror(msg);
    exit(1);
}


int main(int argc, char *argv[])
{

  sleep(1);
	pthread_t threads[1];
  
     //socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
        
     if (argc < 2) 
     {
         fprintf(stderr,"ERROR, no path provided. Usage ./filename [port] [ clientCount] \n");
         exit(1);
     }

     clientCount = atoi(argv[2]);

      sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");

    int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
    error("setsockopt(SO_REUSEADDR) failed");

     bzero((char *) &serv_addr, sizeof(serv_addr));
     int portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,50);

   //Creating threads in loop
     int t,rc;
   for(t=0;t<clientCount;t++)
   {
     //printf("In main: creating thread %ld\n", t);
     rc = pthread_create(&threads[t], NULL, serve, (void *)t);
     if (rc){
       printf("ERROR; return code from pthread_create() is %d\n", rc);
       exit(-1);
       }
   }

   pthread_exit(NULL);
}
