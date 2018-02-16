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
#include <algorithm>

using namespace std;


int sockfd, portno,n; //Socket descriptor
int d_clock;
int clk[100];
int i=0;
int nodes_count;
int nodeno;
int request; //Indicates whether this program needs shared resource
int node_arr[25]; //List of nodes that require shared resource
int request_count = 0;
int file_count; //Acknoledgements received from other process counter 
pthread_mutex_t mutex;
struct sockaddr_in serv_addr, cli_addr; 
void error(const char *msg);
int vect_clk[25]; //Supports upto 25-1 nodes in network

void allok();
void ok(int);
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
    std::queue<int> request_queue;
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
     	vect_clkt[0][k] = clock;     	     	
  }
  cout<<endl;
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
              	
     close(newsockfd);
     
 }


//                                        Checking if request is required and granting request to other process
  sleep(3);

  int newsockfd; //Socket descriptor used for accept function
  int i = 0;
  int clock_;
    struct sockaddr_in cli_addr;
    socklen_t   clilen = sizeof(cli_addr);
    
    for(int l=0; l<nodes_count-1; l++)
    {
      cout<<"In bonus assignment::"<<endl;
      newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
  if (newsockfd < 0) 
          error("ERROR on accept"); 
        
        recv(newsockfd, &clock_, 4, 0);
  int tnode = ntohl(clock_);
  cout<<"transmitted node value:"<<tnode<<endl;
  recv(newsockfd, &clock_, 4, 0);
  int status = ntohl(clock_); //Indicates whether it requires shared resource or not
  cout<<"and its status:"<<status<<endl;
  if(status == 1)
  {
    node_arr[i] = tnode;
    i++;
    request_count++;
  }
  
  close(newsockfd);
    }
    
    
    if(request ==0) //Does not require shared file
    {
      allok(); //Send ok messages to all waiting nodes
    }
    
    
    else
    {
      
      for(int k=0 ;k<request_count; k++)
      {
        if(node_arr[k]>nodeno)
        {
          ok(node_arr[k]);
        }
      }
    }
    



    if(request ==1)
    {
    for(int x=0; x<nodes_count-1; x++)
    {
      int newsockfd; //Socket descriptor used for accept function
    struct sockaddr_in cli_addr;
    socklen_t   clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
  if (newsockfd < 0) 
          error("ERROR on accept");
        recv(newsockfd, &clock_, 4, 0);
  int clock = ntohl(clock_);

  if(clock == 99)
  {
    file_count++;
    cout<<"recived ok"<<endl;
  }

    }
  
    if(file_count == nodes_count-1)
    {
      //Got access to shared file Edit file here
      ofstream myfile1;
      myfile1.open ("sfile.txt");
      myfile1 << "my node number is :"<<nodeno<<"\n";
      myfile1.close();

//Reading contents of file and printing it
      string line;
  ifstream myfile2 ("sfile.txt");
  if (myfile2.is_open())
  {
    while ( getline (myfile2,line) )
    {
      cout << line << '\n';
    }
    myfile2.close();
//After editing send ok to remaining functions
    for(int k=0 ;k<request_count; k++)
      {
        if(node_arr[k]<nodeno)
        {
          ok(node_arr[k]);
        }
      }

  }

  else cout << "Unable to open file";
      
    }   
}
 close(sockfd);
   pthread_exit(NULL);
}

void ok(int x)
{
  int network_socket,n;
  network_socket = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(portno+x);
  inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

  if(connect(network_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1)
  {
    printf("conection error\n");
    
  }
  int clock_;
  
  
     clock_ = htonl(99); //99 indicates that it is ok with this node to access the shared resource
    send(network_socket, (const char*)&clock_, 4, 0); //transmiting
    close(network_socket);
}


void allok()
{
  cout<<"message from allok";
  for(int i=0; i<request_count;i++)
      {
      
          int network_socket,n;
  network_socket = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(portno+node_arr[i]);
  inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

  if(connect(network_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1)
  {
    printf("conection error\n");
    
  }
  int clock_;
  
  
     clock_ = htonl(99); //99 indicates that it is ok with this node to access the shared resource
    send(network_socket, (const char*)&clock_, 4, 0); //transmiting
    close(network_socket);
              
      }

}

int	transmit(int i)
  {
   		int network_socket,n;
	network_socket = socket(AF_INET, SOCK_STREAM, 0);
	char buffer[1024] = {0};
	char buffer1[1024];
	//Address setup and connection
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	 
	server_address.sin_port = htons(portno+i);
	inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

	if(connect(network_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1)
	{
		printf("conection error\n");
		return -1;
	}
	int clock_;
	
	//cout<<"Multicasting vector clock";
	
		 clock_ = htonl(nodeno);
		send(network_socket, (const char*)&clock_, 4, 0);
	//pthread_mutex_lock(&mutex);
	for(int j=0; j<nodes_count; j++)
	{
		
		clock_ = htonl(vect_clk[j]);
    
		//cout<<vect_clk[j];
		send(network_socket, (const char*)&clock_, 4, 0);
	}
 // pthread_mutex_unlock(&mutex);
	cout<<endl;
  sleep(1);	
	close(network_socket);	
   }

   int  a_transmit(int i)
  {
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
    return -1;
  }
  int clock_;
  
  
     clock_ = htonl(nodeno);
    send(network_socket, (const char*)&clock_, 4, 0); //transmiting node no
    clock_ = htonl(request);
    send(network_socket, (const char*)&clock_, 4, 0);
   cout<<"from a_transmit"<<endl;

  
  close(network_socket);  
   }




void *multicast(void *threadid)
{   
	long tid; //variable used to differentiate each thread
   	tid = (long)threadid;
   	sleep(2);
   	//transmit code

   	for(int k=0; k<5; k++)
   	{
      //sleep(k); //To let other nodes multicast
   		pthread_mutex_lock(&mutex);
		vect_clk[nodeno-1] = vect_clk[nodeno -1] +1;
		
		cout<<"MUlticasting clock"<<endl;	
   		for(int i=1; i<=nodes_count;i++)
   		{
   			if(i!=nodeno)
   			{
          //sleep(i);
   				//pthread_mutex_lock(&mutex);
   				transmit(i);
          
   				//pthread_mutex_unlock(&mutex);
   			}
   		
   		}
      pthread_mutex_unlock(&mutex);	
   	}
    //Multicast for shared resource

    cout<<"message after 5 multicasts";
     
      for(int i=1; i<=nodes_count;i++)
      {
        if(i!=nodeno)
        {
                
          a_transmit(i);                    
        }
      
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
        
     if (argc < 5) 
     {
         fprintf(stderr,"ERROR, no path provided. Usage ./filename [port] [nodes in network] [node number of this program] [request to shared resource]\n");
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
     request = atoi(argv[4]);
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
   
}