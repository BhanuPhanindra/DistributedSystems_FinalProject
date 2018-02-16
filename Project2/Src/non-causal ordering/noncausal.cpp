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
#include <queue>

using namespace std;


int sockfd, portno,n; //Socket descriptor
int d_clock;
int clk[100];
int i=0;
int nodes_count;
int nodeno;
pthread_mutex_t mutex;
struct sockaddr_in serv_addr, cli_addr; 
void error(const char *msg);
int vect_clk[25]; //Supports upto 25-1 nodes in network


//Function executed by each thread when created
 class buff
{
  public:
  //vector clock
  int vect_clk[25];  //25-1 because one location is used to store transmitter node number
};
void *serve(void *threadid)
{   
	long tid; //variable used to differentiate each thread
   	tid = (long)threadid;
    std::queue<buff> packet_buffer;
    buff b[10];//Can buffer up to 10 elments
    buff tempb;
    int buffer_count = 0;
   	char buffer[256];
    listen(sockfd,50);
   	for(int z=0; z<10; z++)   //Z limit DEpends on number of nodes and multicasts per node.(change if any of these are changed)
   	{


   	int newsockfd; //Socket descriptor used for accept function
   	string log;
   	int vect_clkt[10][nodes_count];
    struct sockaddr_in cli_addr;
    socklen_t   clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
  if (newsockfd < 0) 
          error("ERROR on accept");
      
   
    	bzero(buffer,256); //clearing buffer
    
    int clock;
 		int clock_;
 	
 	recv(newsockfd, &clock_, 4, 0);
 	clock = ntohl(clock_);
 	cout<<"receiving vector clock from node::"<<clock<<endl;
  int tnode = clock-1;//Index of clock of transmitting node
 	for(int k=0; k<nodes_count; k++)
 	{
 	  recv(newsockfd, &clock_, 4, 0);
 	  clock = ntohl(clock_);

     	cout<<clock<<" ";
      if(vect_clk[k]<clock)
      {
        vect_clk[k] = clock;
      }
     	//vect_clkt[0][k] = clock;     	     	
  }
  cout<<endl;
  cout<<"This message is delivered"<<endl;
  cout<<"the clock is now:"<<endl;
  for(int k=0; k<nodes_count; k++)
  {
    cout<<vect_clk[k]<<" ";
  }
  cout<<endl;
  
                         //Commenting Code responsible for buffering
  /*
  int check_flag = 0;
    if(vect_clk[tnode]+1 == vect_clkt[0][tnode])
    {
      for(int d=0; d<nodes_count; d++)
      {
        if(d!=tnode)
        {
           if(vect_clkt[0][d]<=vect_clk[d])
           {
              //Do nothing
           }
           else
           {
             check_flag = 1;
           }
        }
        
      }
    }
    else
    {
      check_flag = 1;
    }

     if(check_flag == 0)
     {
        cout<<"This message is delivered to application::"<<endl; 
        pthread_mutex_lock(&mutex);
        vect_clk[tnode] = vect_clk[tnode] +1;
        pthread_mutex_unlock(&mutex);
        //Checking if buffer packets can be devilered
        if(buffer_count ==0)
        {
          //Indicates No packets are present in buffer

        }
        



        else
        {
          int queue_size = packet_buffer.size();
          for(int q=0; q<queue_size; q++)
          {
          int in_flag = 0;
          tempb = packet_buffer.front();
          for(int i=0; i<nodes_count; i++)
        {
            vect_clkt[0][i] = tempb.vect_clk[i];
        }
        int buffer_tnode = tempb.vect_clk[nodes_count];
        if(vect_clk[buffer_tnode]+1 == vect_clkt[0][buffer_tnode])                                    //Correction NEEDED
    {
      for(int d=0; d<nodes_count; d++)
      {
        if(d!=buffer_tnode)
        {
           if(vect_clkt[0][d]<=vect_clk[d])
           {
              //Do nothing
           }
           else
           {
             in_flag = 1;
           }
        }
        
      }
    }
    else
    {
      in_flag = 1;
    }

    if(in_flag == 0)
    {
      cout<<"Deliver buffered packet";
      pthread_mutex_lock(&mutex);
      vect_clk[buffer_tnode] = vect_clk[buffer_tnode] +1;
      pthread_mutex_unlock(&mutex);
      packet_buffer.pop();
      buffer_count--;
    }
    else if(in_flag == 1)
    {
      //Message must stay in buffer and adding at back of queue
      packet_buffer.pop();
      packet_buffer.push(tempb);
    }
  }

 }
     }
     
      else if(check_flag ==1)
      {
        cout<<"This message has to be buffered"<<endl;
        for(int i=0; i<nodes_count; i++)
        {
            b[buffer_count].vect_clk[i] = vect_clkt[0][i];
        }
        b[buffer_count].vect_clk[nodes_count] = tnode;
        packet_buffer.push(b[buffer_count]);
        buffer_count++;

      }
      cout<<"Present clock:"<<endl;
     pthread_mutex_lock(&mutex);
     for(int i=0; i<nodes_count; i++)
     {
     	cout<<vect_clk[i]<<" ";
     }
     cout<<endl;
     pthread_mutex_unlock(&mutex);
     



     	
     	//send(newsockfd, (const char*)&clock_, 4, 0);
     //	while(nodes_count!=i-1)
     	
*/
     	
     close(newsockfd);
     
 }
 close(sockfd);
   pthread_exit(NULL);
}




void *multicast(void *threadid)
{   
	long tid; //variable used to differentiate each thread
   	tid = (long)threadid;
    int latepacket[nodes_count];
   	sleep(2);
   	//transmit code

   	for(int k=0; k<5; k++)
   	{
   		pthread_mutex_lock(&mutex);
		vect_clk[nodeno-1] = vect_clk[nodeno -1] +1;
    cout<<"MUlticasting clock"<<endl;
    cout<<"clock after multicast::"<<endl;
    int temp_vect[nodes_count];
    for(int j=0; j<nodes_count; j++)
    {
      temp_vect[j] = vect_clk[j];
      cout<<vect_clk[j]<<" ";
    }
    cout<<endl;
		pthread_mutex_unlock(&mutex);
			
   		for(int i=1; i<=nodes_count;i++)
   		{
        if((k==3) && (i==3) && (nodeno==2))
   {
    for(int j=0; j<nodes_count; j++)
  {   
    latepacket[j] = temp_vect[j];       
  }
      continue;
   }

   			if(i!=nodeno)
   			{
          sleep(i);
   				//pthread_mutex_lock(&mutex);
   				//transmit(i);
         
          int network_socket,n;
  network_socket = socket(AF_INET, SOCK_STREAM, 0);
  //char buffer[1024] = {0};
  //char buffer1[1024];
  //Address setup and connection
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
   
  server_address.sin_port = htons(portno+i);
  inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

  if(connect(network_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1)
  {
    printf("conection error\n");
    
  }
  int clock_;
  
     clock_ = htonl(nodeno);
    send(network_socket, (const char*)&clock_, 4, 0);
  //pthread_mutex_lock(&mutex);
   if((k==3) && (i==3) && (nodeno==2))
   {
      for(int j=0; j<nodes_count; j++)
  {   
    latepacket[j] = temp_vect[j];       
    //send(network_socket, (const char*)&clock_, 4, 0);
  }
   }
   else
   {
  for(int j=0; j<nodes_count; j++)
  {
    
    clock_ = htonl(temp_vect[j]);
    
    //cout<<vect_clk[j];
    send(network_socket, (const char*)&clock_, 4, 0);
  }
}
/*
else
{
  for(int j=0; j<nodes_count; j++)
  {
    
   latepacket[j] = temp_vect[j];
  }
}*/
  
  cout<<endl;  
  close(network_socket);              				
   			}
   		
   		}
      	
   	}

    int i = 3;
    int k = 3;
    
  if(nodeno == 2)
        {
          sleep(i);
          
              int network_socket,n;
  network_socket = socket(AF_INET, SOCK_STREAM, 0);
  //Address setup and connection
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
   
  server_address.sin_port = htons(portno+3);
  inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

  if(connect(network_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1)
  {
    printf("conection error\n");
    
  }
  int clock_;
  
     clock_ = htonl(nodeno);
    send(network_socket, (const char*)&clock_, 4, 0);
 // pthread_mutex_lock(&mutex);
  for(int j=0; j<nodes_count; j++)
  {
    
    clock_ = htonl(latepacket[j]);
    
    //cout<<vect_clk[j];
    send(network_socket, (const char*)&clock_, 4, 0);
  }
  //pthread_mutex_unlock(&mutex);
  
  cout<<endl;

  
  close(network_socket); 
            }
          
        

   	}


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{

	//Setting up socket

	pthread_t threads[stoi(argv[2]) +1];
  
     //socklen_t clilen;
     char buffer[256];
        
     if (argc < 4) 
     {
         fprintf(stderr,"ERROR, no path provided. Usage ./filename [port] [nodes in network] [node number of this program] \n");
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
     nodes_count = atoi(argv[2]);
     nodeno = atoi(argv[3]); //Node number of this program given through command line argument
     //Initializing vector clock to zeroes
     for(int i=0; i<nodes_count; i++)
     {
     	vect_clk[i] = 0;
     }


     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno + nodeno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     


    /*srand(time(NULL));
	 d_clock = rand() %10 +1;
	 cout<<"Daemon clock is :"<<d_clock<<endl;*/
   //Creating threads in loop
     int t,rc;
     
    //Thread for listening on port 
   for(t=0;t<1;t++)
   {
     //printf("In main: creating thread %ld\n", t);
     rc = pthread_create(&threads[t], NULL, serve, (void *)t);
     if (rc){
       printf("ERROR; return code from pthread_create() is %d\n", rc);
       exit(-1);
       }
   }
   //Thread for multicast
   rc = pthread_create(&threads[t], NULL, multicast, (void *)t);
     if (rc){
       printf("ERROR; return code from pthread_create() is %d\n", rc);
       exit(-1);
       }

   pthread_exit(NULL);
   //computing average and sending back


}