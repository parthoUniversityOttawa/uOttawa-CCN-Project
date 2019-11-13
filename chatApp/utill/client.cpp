// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <iostream>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include<signal.h>
#include <pthread.h>

using namespace std;

#define PORT 8080 
pthread_t tid[2];
int sock;
void sig_handler(int signo)
{
  if (signo == SIGINT)
    printf("received SIGINT\n");
 int  status = pthread_kill( tid [0], SIGUSR1);
  if ( status <  0)
     perror("send pthread_kill failed");
  status = pthread_kill( tid [1], SIGUSR2);
    if ( status <  0)
       perror("recv pthread_kill failed");

}

void sendThreadExit(int signo)
{
	std::cout<<"send thread END"<<std::endl;
	string msgInformation = "App1 going Offline";
	send(sock , msgInformation.c_str() , msgInformation.length() ,0 );

	pthread_exit(0);

}

void appGoesOffline()
{
	int  status = pthread_kill( tid [0], SIGUSR1);
	  if ( status <  0)
	     perror("send pthread_kill failed");
	  status = pthread_kill( tid [1], SIGUSR2);
	    if ( status <  0)
	       perror("recv pthread_kill failed");
}

void receiveThreadExit(int signo)
{
	cout<<"receive thread END"<<endl;
	pthread_exit(0);

}

void * send(void *arg)
{
	cout<<"send thread START"<<endl;
	string msgInformation;


	if (signal(SIGUSR1, sendThreadExit) == SIG_ERR)
		  printf("\ncan't catch SIGINT\n");

	int socketFD = *((int *)arg);
	while(1)
	{
	cin >> msgInformation ;
	send(socketFD , msgInformation.c_str() , msgInformation.length() ,0 );
	}

}

void *recieve (void *arg)
{
	cout<<"receive thread START"<<endl;
	char buffer[1024] = {0};
	int socketFD = *((int *)arg);
	string conTerminateReqMsg = "App2 going Offline";

	if (signal(SIGUSR2, receiveThreadExit) == SIG_ERR)
			  printf("\ncan't catch SIGINT\n");

	while(1)
	{
	read( socketFD , buffer, 1024);

	cout<<"Message RCV:"<<buffer<<endl;
	if(strcmp(buffer,conTerminateReqMsg.c_str()) == 0)
	{
		close (socketFD);
		cout << "APP2 disconnected" <<endl;
		appGoesOffline();
		break;
	}
}
}


int main(int argc, char const *argv[]) 
{ 

	if (signal(SIGINT, sig_handler) == SIG_ERR)
	  printf("\ncan't catch SIGINT\n");

	sock = 0;
	struct sockaddr_in serv_addr; 
//	char *hello = "tu Pagala ha@@#";
//	char buffer[1024] = {0};
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 



	/*
	 * Thread create
	 */



	if(pthread_create(&tid[0], NULL, send, &sock) != 0)
	{
		printf("Failed to create thread\n");
	}

	if(pthread_create(&tid[1], NULL, recieve, &sock) != 0)
	{
		printf("Failed to create thread\n");
	}


	int i = 0;
	  while(i< 2)
	  {
		 pthread_join(tid[i++],NULL);
		 printf("Thread %d:\n",i);
	  }

	  cout<<"Final program termainate"<<endl;
	  //close(sock);
	/*send(sock , hello , strlen(hello) , 0 );
	printf("Hello message sent\n"); 
	valread = read( sock , buffer, 1024); 
	printf("%s\n",buffer ); */
	return 0; 
} 

