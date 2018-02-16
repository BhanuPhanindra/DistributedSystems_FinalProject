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

//global Variables
int customer_count = 0;
int rc,n; //variable used to store return value for error handling
long t; //iteration variable
int sockfd, portno; //Socket descriptor
struct sockaddr_in serv_addr, cli_addr; 
void error(const char *msg);

// Created a class for customer with properties account number, balance, customer name and mutex for his account
class customer
{
public:
    long int account_number;
	double balance;
	string name;
	//Mutex and functions to lock and unlock above variables
	pthread_mutex_t mutex; //A mutex is created for every customer
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
      
    while(1)
    {
    	bzero(buffer,256); //clearing buffer
    	n = read(newsockfd,buffer,255); //Reading data from client
     	if (n < 0) error("ERROR reading from socket");
     
     	//sleep(2);
     	cout<<"Here is the message:\n"<<buffer<<endl;
     	//sleep(2);


        //Performing transaction by spliting each part in transaction line

     	istringstream iss(buffer);
		
		int y = 0; //loop iteration variable
		//Temporary variables to hold value from buffer
		int timestamp; 
		long int account_number;
		string type;
		float amount;
		//Do while loop to copy each part of buffer in to above variables
		do
    {
        string subs;

        iss >> subs;
        y++;
       // cout << "Substring: " << subs<< endl;
        if(subs != "" && y==1)
        {
        	//test = stoi(subs);
         	timestamp = stoi(subs);
        	//cout<<timestamp<<endl;
        }
        else if(subs != "" && y==2)
        {
        	account_number = stoi(subs);
        	//cout<<"this is accout number"<<account_number<<endl;
        }
        else if(subs != "" && y==3)
        {
        	type = subs;
        	//cout<<"transacion type "<<type<<endl;
        }
        else if(subs != "" && y==4)
        {
        	amount = stoi(subs);
        	//cout<<"amount to deal"<<amount<<endl;
        }

        //finding account with requested account number
        

    } while (iss); //End of do while loop

    if(type == "x") // X indicates end of transactions for that customer
    {
    	cout<<"All Transaction are Done";
    	close(newsockfd);
     	close(sockfd);
   		pthread_exit(NULL);
    }

    unsigned int i=0;
        for(i=0; i<customer_list.size(); i++)
	{
		
		if(customer_list[i].account_number == account_number) //Finding the customer with given account number
		{
			//Locking particular customer Data Structure
			customer_list[i].lock();
			cout<<"Request received from "<<customer_list[i].name<<endl;
			if(type == "w") //W indicates withdraw of funds
			{
				
				//Checking if funds are available
				if(customer_list[i].balance>amount)
				{
					customer_list[i].balance = customer_list[i].balance - amount;
					cout<<"balance after withdraw in "<<customer_list[i].name<<"account is::"<<endl;
					cout<<customer_list[i].balance<<endl; 
				}
				
				else
				{
					cout<<"Transaction failed:: Insufficient funds in your account";
				}
			}
			if(type == "d")// Depositing funds
			{
				//pthread_mutex_lock(&customer_list[i].mutex);
				customer_list[i].balance = customer_list[i].balance + amount;
				cout<<"balance after deposite in "<<customer_list[i].name<<"account is::"<<endl;
				cout<<customer_list[i].balance<<endl;
				log = "balance after deposite in " + customer_list[i].name+"account is:: " + to_string(customer_list[i].balance);
				//pthread_mutex_unlock(&customer_list[i].mutex);
			}
			customer_list[i].unlock();// Unlocking resourse held
			break;
		}

	} //End of for loop

	if(i == customer_list.size())
	{
		cout<<"no such account with given account number";
		n = write(newsockfd,"no such account with given account number",41);
		if (n < 0) error("ERROR writing to socket");
	}
	else
	{
		//n = write(newsockfd,log,sizeof(log));
		n = write(newsockfd,"Transaction done Successfully.",30);
		if (n < 0) error("ERROR writing to socket");
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
	fstream handle;
	handle.open(argv[2]); //Path of file entered as command line argument is opened
	string buffer1; //temporary string to store contents of file

//Using while loop to read each line of file
while(getline(handle,buffer1))
	{
		
		cout<<buffer1<<endl;
		istringstream iss(buffer1);
		customer temp_customer;
		int y = 0; //iteration variable
		//Moving contents of file to data structure
		do
    {
        string subs; //used to hold parts of string

        iss >> subs;
        y++;
       // cout << "Substring: " << subs<< endl;
        if(subs != "" && y==1)
        {
        	//test = stoi(subs);
        	temp_customer.account_number = stoi(subs);
        }
        else if(subs != "" && y==2)
        {
        	temp_customer.name = subs;
        }
        else if(subs != "" && y==3)
        {
        	temp_customer.balance = stoi(subs);
        }

    } while (iss);
    customer_list.push_back(temp_customer); //Adding temporary object to data structure
	}

	//Printing contents of customer data structure
	for(unsigned int i=0; i<customer_list.size(); i++)
	{
		cout<<"names of customer::"<<customer_list[i].name<<endl;
		cout<<"account_number of customer::"<<customer_list[i].account_number<<endl;
		cout<<"balance of customer::"<<customer_list[i].balance<<endl;
	}

	//Setting up socket

	pthread_t threads[5];
  
     //socklen_t clilen;
     char buffer[256];
        
     if (argc < 3) 
     {
         fprintf(stderr,"ERROR, no path provided. Usage ./filename [port] [records.txt path]\n");
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

   //Creating threads in loop
   for(t=0;t<50;t++)
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