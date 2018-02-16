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
    exit(1);
}

int main(int argc, char *argv[])
{

	//Socket 
	int network_socket,portno,n;
	network_socket = socket(AF_INET, SOCK_STREAM, 0);
	char buffer[1024] = {0};
	char buffer1[1024];
	
	//Address setup and connection
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	 if (argc < 3) 
    {
        fprintf(stderr,"ERROR::Usage ./filename [server address] [port]\n");
        exit(1);
    }
	server_address.sin_port = htons(stoi(argv[2]));
	inet_pton(AF_INET, argv[1], &server_address.sin_addr);

	if(connect(network_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1)
	{
		printf("conection error\n");
		return -1;
	}

	srand(time(NULL));
	int clock = rand() %10 +1;
	cout<<"my clock is :"<<endl;
	cout<<clock<<endl;
	int clock_;
 	recv(network_socket, &clock_, 4, 0); //Receive clock from Daemon
 	int d_clock = ntohl(clock_);
 	int diff = clock - d_clock;
  	clock_ = htonl(diff);
	send(network_socket, (const char*)&clock_, 4, 0);//Sending Difference

	recv(network_socket, &clock_, 4, 0);//Receiving offset
	int offset = ntohl(clock_);
	cout<<"offset Received::"<<offset<<endl;

	
	clock = clock + offset;
	cout<<"New time after synchronisation is:"<<clock<<endl;
	
	close(network_socket);
    return 0;
}