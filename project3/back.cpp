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

pthread_mutex_t mut;
long int account_numberGenerator = 0;
int clientCount;
int sockfd, portno,n; //Socket descriptor
struct sockaddr_in serv_addr, cli_addr; 
void error(const char *msg);
void cast(int newsockfd, string st);

// Created a class for customer with properties account number, balance, customer name and mutex for his account
class customer
{
public:
    long int num1;  //Account Number
	long int num2;    //Balance
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
    //before taking command run 2pc and then commit()
    //receive vote request from front end

    //Reply to vote request as vote
    
    //Responce from front end as commit or abort

    //Follow commit or abort and send acknoledgement at end of code

    //If commit execute the following code else skip to end of loop
    char cmd[256];
    cout<<"Waiting for command"<<endl;
    bzero(cmd,256); //clearing buffer
    n = read(newsockfd,cmd,256); //Reading data from client
     if (n < 0) error("ERROR reading from socket");
    cout<<"front sent::"<<endl;
    cout<<cmd<<endl;
    //string check(buffer);
    cout<<"Waiting for command2"<<endl;
    memset(&buffer,0,256);
     //clearing buffer
    //char bufff[256];
    n = read(newsockfd,buffer,256); //Reading data from front
     //if (n < 0) error("ERROR reading from socket");
    cout<<"front sent::"<<buffer<<endl;
    string check(buffer);
    if(check.compare("") == 0)
    {
        cout<<"will this print"<<endl;
        goto exitt;
    }

    string temp(buffer);
    if(temp.compare("vote_request") == 0)
    {
        cout<<"voted \n";
        string st = "vote";
        send(newsockfd, st.c_str() , st.length() ,0);

    }
    else
    {
        cout<<"something went wrong with vote_request";
    }


        //Performing transaction by spliting each part in transaction line

     	istringstream iss(cmd);
		
		int y = 0; //loop iteration variable
		//Temporary variables to hold value from buffer
		long int num2; 
		long int num1;
		string command;
		//float amount;
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
         	command = subs;
        	//cout<<timestamp<<endl;
        }
        else if(subs != "" && y==2)
        {
        	num1 = stoi(subs);
        	//cout<<"this is accout number"<<account_number<<endl;
        }
        else if(subs != "" && y==3)
        {
        	num2 = stoi(subs);
        	//cout<<"transacion type "<<type<<endl;
        }

        //finding account with requested account number
        

    } while (iss); //End of do while loop



//reading decesion
    memset(&buffer,0,256); //clearing buffer
    n = read(newsockfd,buffer,255); //Reading data from front
     if (n < 0) error("ERROR reading from socket");
    cout<<"front sent::"<<endl;
    cout<<buffer<<endl;
    string temp2(buffer);

    if(temp2.compare("commit") == 0)
{ 
   //executing command
    cout<<"under execution \n";
    cout<<command<<endl;

    if(command.compare("QUIT") == 0)
    {
        cout<<"in quit \n";
        n = write(newsockfd,"OK",2);
        if (n < 0) error("ERROR writing to socket");
        //Code to exit thread
        close(newsockfd);
        close(sockfd);
        pthread_exit(NULL);
    }
        cout<<"Before create \n";
         if(command.compare("CREATE") == 0)
        {
            cout<<"IN create";
            customer temp_customer;
                        temp_customer.num2 = num1; //Balance is entered immediately after create
                        char buf[256];
                        pthread_mutex_lock(&mut);
                        account_numberGenerator++;
                        temp_customer.num1 = account_numberGenerator;
                        
                        string st = "ok ";
                        st.append(to_string(temp_customer.num1));
                        send(newsockfd, st.c_str() , st.length() ,0);
                        customer_list.push_back(temp_customer); //Adding temporary object to data structure
                        pthread_mutex_unlock(&mut);
                        
                        
        }
        
        else if(command.compare("UPDATE") == 0)
        {
            cout<<"IN update \n";
            //For loop to find customer with given account number

            int flag = 1;                //if exists update and return status
         for(int i=0; i<account_numberGenerator; i++)
         {
             if(customer_list[i].num1 == num1)
            {
                //cout<<"Account found"<<endl;
                customer_list[i].lock();
                flag = 0;
                customer_list[i].num2 = num2; //Updating account
                string st;
                st = "OK ";
                st.append(to_string(customer_list[i].num2));
                customer_list[i].unlock();
                cout<<st<<endl;
                send(newsockfd, st.c_str() , st.length() ,0);                                
                 break;

            }
                                 
        }

        if(flag == 1)
                {
                    cout<<"no account found";
                    string st = "No account";
                    send(newsockfd, st.c_str() , st.length() ,0);
                }
                        
       } 
        else if(command.compare("QUERY") == 0)
        {
            int flag = 1;
            cout<<"query \n";
            for(int i=0; i<account_numberGenerator; i++)
            {
                if(customer_list[i].num1 == num1)
                {
                    flag = 0;
                    cout<<"Account found"<<endl;
                                //customer_list[i].num2 = num2; 
                                string st = "ok ";
                                st.append(to_string(customer_list[i].num2));
                                send(newsockfd, st.c_str() , st.length() ,0);
                                
                             break;
                }
                
            }
            if(flag == 1)
                {
                    cout<<"no account found";
                    string st = "No account";
                    send(newsockfd, st.c_str() , st.length() ,0);
                }
        }
    

        else
        {
            cout<<"Proper command not received. Terminating ..."<<endl;
            string st = "Incorrect command. Please check input.";
            send(newsockfd, st.c_str() , st.length() ,0);

        }             
}

      else if(temp2.compare("abort") == 0)
      {
        cout<<"Abort received"<<endl;
      } 

      else
      {
        cout<<"received not commit not abort"<<endl;
      }

    
     }
 	exitt:
     close(newsockfd);
     close(sockfd);

   pthread_exit(NULL);
}

void cast(int newsockfd, string st)
{
    char buf[256];
        memset(&buf,0,256);
        st.copy(buf,st.length(),0);
        n = write(newsockfd,st.c_str(),256);
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{

	//Setting up socket

	pthread_t threads[1];
  
     //socklen_t clilen;
     char buffer[256];
        
     if (argc < 3) 
     {
         fprintf(stderr,"ERROR, no path provided. Usage ./filename [port] [client count] \n");
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
     portno = atoi(argv[1]);
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
   cout<<"program  closedd";


}