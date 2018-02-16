#include <iostream>
#include <fstream>
#include <string>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include <vector>
#include <sys/time.h>


using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{

	//timer
	struct timeval start, end;
	long mtime, seconds, useconds;
	long transaction_time[1000];
	int transaction_count = 0;
	double total_time = 0;
	double average_time;
	sleep(3);
	
    if (argc < 5) 
    {
        fprintf(stderr,"ERROR::Usage ./filename [server address] [port] [timestep] [records.txt path]\n");
        exit(1);
    }
	//setting up socket for communication
	//creating Socket
	
	int network_socket,portno,n;
	network_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	//Address setup and connection
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	//portno = atoi(argv[2]);
	server_address.sin_port = htons(stoi(argv[2]));
	inet_pton(AF_INET, argv[1], &server_address.sin_addr);

	if(connect(network_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1)
	{
		printf("conection error\n");
		return -1;
	}

	string line;
	char buffer[256];
	int i = 0;
	int timer_offset =0;
	int timer = 0;

	//opening a file for reading set of transactions to be performed
	fstream handle;
	handle.open(argv[4]);


	if(handle.is_open())
	{
		while (getline(handle,line)) //getline copies data into line in String format
    {
      //Timer
    	istringstream iss(line);
		//customer temp_customer;
		int y = 0;
		do
    {
        string subs;

        iss >> subs;
        y++;
        if(subs != "" && y==1)
        {
        
        	 timer = stoi(subs);
        }
        else if(subs != "" && y==2)
        {
        	long int account_number = stoi(subs);
        }
        else if(subs != "" && y==3)
        {
        	string type = subs;
        }
        else if(subs != "" && y==4)
        {
        	float balance = stoi(subs);
        }

    } while (iss);

    	sleep((timer-timer_offset)*stof(argv[3]));
    	timer_offset = timer;


        bzero(buffer,256);
    	int length = line.size();
    	line.copy(buffer,length,0);
    	cout<<buffer<<endl;
    	gettimeofday(&start, NULL);
    	send(network_socket, buffer , strlen(buffer) , 0 );
    	n = read(network_socket,buffer,255);
    	gettimeofday(&end, NULL);
    	//Timer calculations
    	seconds  = end.tv_sec  - start.tv_sec;
    	useconds = end.tv_usec - start.tv_usec;
    	
		mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
		transaction_time[transaction_count] = (seconds*1000*1000) + useconds;
		transaction_count++;
    	cout<<buffer<<endl;
    }
	}
	write(network_socket,"00 00 x 00",10);
	close(network_socket);
	for(int i=0; i<transaction_count; i++)
	{
		total_time = transaction_time[i]+ total_time;
	}
	cout<<"Total time is:"<<total_time<<endl;
	average_time = total_time/transaction_count;
	cout<<"Average time is :"<<average_time<<endl;


return 0;
}