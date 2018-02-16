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

using namespace std;


int sockfd, portno,n; //Socket descriptor
int d_clock;
int clk[100];
int off[100];
int i=0;
int nodes;
int flag = 0;
int avg = 0;
int s_clock;
pthread_mutex_t mutex;
struct sockaddr_in serv_addr, cli_addr; 
void error(const char *msg);


//Function executed by each thread when created
void *serve(void *threadid)
{   
	long tid; //variable used to differentiate each thread
   	tid = (long)threadid;
   	char buffer[256];
   	//char d[1]={'d'};
   	int newsockfd; //Socket descriptor used for accept function
   	string log;
    struct sockaddr_in cli_addr;
    socklen_t   clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
  if (newsockfd < 0) 
          error("ERROR on accept");
      
   
    	bzero(buffer,256); //clearing buffer
    	
     	int clock;
 	int clock_;
  clock_ = htonl(d_clock);
  send(newsockfd, (const char*)&clock_, 4, 0);//Sending Daemon clock
 	
  recv(newsockfd, &clock_, 4, 0);//Receiving difference from each node
 	int diff = ntohl(clock_);

     //	cout<<"Here is the time:\n"<<clock<<endl;
     	pthread_mutex_lock(&mutex);
     	clk[2*i] = diff;
      clk[2*i+1] = newsockfd;
     	i++;
     	pthread_mutex_unlock(&mutex);
     	//clock = d_clock - clock;
     	
     	while(nodes!=i)
     	{
     		sleep(2);
     	}
      
      pthread_mutex_lock(&mutex);
      if(flag == 0)
      {
      for(int j=0; j<2*nodes;j=j+2)
      {
        avg = avg + clk[j];
        //cout<<"average:"<<avg<<endl;
      }
      avg = avg/(nodes+1); //Including daemon node in Average
      cout<<"average:"<<avg<<endl;
      d_clock = d_clock + avg; //Adding it to Daemon clock
      cout<<"new Daemon clock is:"<<d_clock<<endl;
      
      flag = 1;
    }
    

      pthread_mutex_unlock(&mutex);
      
      
        clock_ = htonl(avg-diff);
        send(newsockfd, (const char*)&clock_, 4, 0);
        
      
      

     	
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

	//Setting up socket

	pthread_t threads[5];
  
     //socklen_t clilen;
     char buffer[256];
        
     if (argc < 3) 
     {
         fprintf(stderr,"ERROR, no path provided. Usage ./filename [port] [nodes in network excluding daemon] \n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");

    int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
    error("setsockopt(SO_REUSEADDR) failed");

     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,50);


    srand(time(NULL));
	 d_clock = rand() %10 +1;
	 cout<<"Daemon clock is :"<<d_clock<<endl;
   //Creating threads in loop
     int t,rc;
     nodes = atoi(argv[2]);
   for(t=0;t<nodes;t++)
   {
     //printf("In main: creating thread %ld\n", t);
     rc = pthread_create(&threads[t], NULL, serve, (void *)t);
     if (rc){
       printf("ERROR; return code from pthread_create() is %d\n", rc);
       exit(-1);
       }
   }

   pthread_exit(NULL);
   //computing average and sending back


}