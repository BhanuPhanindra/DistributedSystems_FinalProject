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
    sleep(2);
    char buffer[1024];
	//Socket 
    
	int network_socket,portno,n;
	network_socket = socket(AF_INET, SOCK_STREAM, 0);
	//char buffer1[1024];
	
	//Address setup and connection
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	 if (argc < 2) 
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


    char quitFlag[256];
    string temp = "QUIT";
    temp.copy(quitFlag,temp.length(),0); 
        while(1)
        {
            string line;
            cout<<"enter Command Here::";
            getline (cin,line);
            
            bzero(buffer,256);
            line.copy(buffer,line.length(),0);
            //cout<<buffer<<endl;
            for(int i=0; i<line.length(); ++i)
    {
        buffer[i] = toupper(buffer[i]);
    }

        send(network_socket, buffer , line.length() ,0);

        string input(buffer);
        //cout<<input<<endl;
        if(input.compare(temp) == 0)
        {
            cout<<"Terminating program ...";
            break;
        }
            
            memset(&buffer,0,256); //clearing buffer
            //cout<<"Contents of  BUffer"<<endl;
        n = read(network_socket,buffer,sizeof(buffer)); //Reading data from client
        //cout<<"Data from server::"<<endl;
        if (n < 0) error("ERROR reading from socket");
        cout<<buffer<<endl;
        //cout<<"End of loop"<<endl;
        }
        
    return 0;
}