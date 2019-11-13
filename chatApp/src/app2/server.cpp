// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>
#include <iostream>
#include <signal.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#define PORT 8080 

using namespace std;

int new_socket;

pthread_t tid[2];

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

void appGoesOffline()
{
	int  status = pthread_kill( tid [0], SIGUSR1);
	  if ( status <  0)
	     perror("send pthread_kill failed");
	  status = pthread_kill( tid [1], SIGUSR2);
	    if ( status <  0)
	       perror("recv pthread_kill failed");
}

void sendThreadExit(int signo)
{
	std::cout<<"send thread END "<<std::endl;

	string msgInformation = "App2 going Offline";
		send(new_socket , msgInformation.c_str() , msgInformation.length() ,0 );

	pthread_exit(0);

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
	//string buffer;
	string conTerminateReqMsg = "App1 going Offline";
	int socketFD = *((int *)arg);

	if (signal(SIGUSR2, receiveThreadExit) == SIG_ERR)
			  printf("\ncan't catch SIGINT\n");

	while(1)
	{
	read( socketFD , buffer, 1024);

	cout<<"Message RCV:"<<buffer<<endl;
	if(strcmp(buffer,conTerminateReqMsg.c_str()) == 0)
	{
		close (socketFD);
		cout << "APP1 disconnected" <<endl;
		appGoesOffline();
		break;
	}

	//buffer.clear();
	}



}

int getSystemIP(string &msg , int fd_sockt)
{
	 char ip_address[15];
	    int fd;
	    struct ifreq ifr;

	    /*AF_INET - to define network interface IPv4*/
	    /*Creating soket for it.*/
	    //fd = socket(AF_INET, SOCK_DGRAM, 0);

	    /*AF_INET - to define IPv4 Address type.*/
	    ifr.ifr_addr.sa_family = AF_INET;

	    /*eth0 - define the ifr_name - port name
	    where network attached.*/
	    memcpy(ifr.ifr_name, "eth1", IFNAMSIZ-1);

	    /*Accessing network interface information by
	    passing address using ioctl.*/
	    ioctl(fd_sockt, SIOCGIFADDR, &ifr);
	    /*closing fd*/
	   // close(fd);

	    /*Extract IP Address*/
	    strcpy(ip_address,inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	    printf("APP2 Address is: %s\n",ip_address);

	    return 0;

}


int main(int argc, char const *argv[]) 
{ 
	if (signal(SIGINT, sig_handler) == SIG_ERR)
		  printf("\ncan't catch SIGINT\n");


	int server_fd, valread;
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	char str[INET6_ADDRSTRLEN];
	



	// Creating socket file descriptor 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	string ipAdd;
	getSystemIP(ipAdd,server_fd);
	// Forcefully attaching socket to the port 8080 
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
	


	// Forcefully attaching socket to the port 8080 
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	if (listen(server_fd, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
					(socklen_t*)&addrlen))<0) 
	{ 
		perror("accept"); 
		exit(EXIT_FAILURE); 
	} 


	/*
		 * Thread create
		 */



		if(pthread_create(&tid[0], NULL, send, &new_socket) != 0)
		{
			printf("Failed to create thread\n");
		}

		if(pthread_create(&tid[1], NULL, recieve, &new_socket) != 0)
		{
			printf("Failed to create thread\n");
		}


		int i = 0;
		  while(i< 2)
		  {
			 pthread_join(tid[i++],NULL);
			 printf("%d:\n",i);
		  }

		  cout<<"Final program termainate"<<endl;
		  close(server_fd);
		  close (new_socket);



	/*valread = read( new_socket , buffer, 1024);
	printf("%s\n",buffer ); 
	send(new_socket , hello , strlen(hello) , 0 ); 
	printf("Hello message sent\n"); */
	return 0; 
} 

