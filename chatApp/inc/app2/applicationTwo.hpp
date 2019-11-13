/************************************************************************************
 * University Of Ottawa
 * Project Presentation for Computer Communications Network.
 *
 * This software embodies materials and concepts which are
 * Used for the sole purpose to demonstrate - TCP -Socket Programming Interface.
 * Furthermore, This software is capable to perform Full duplex communication in Intra
 * domain Infrastructure
 *
 *
 * Designed and Developed by Uottawa MEng Students
 *
 * file          applicationTwo.hpp
 * Developers:   Partho Ghosal. (300130532)
 * 				 Ashwini Upasani (300126565)
 * 				 Navdeep Kaur Dillon (300108180)
 * 				 Rahul Ranjan (300124075)
 *
 ************************************************************************************/

#ifndef UOTTAWA_CCN_PROJECT_CHATAPP_INC_APPLICATIONTWO_HPP_
#define UOTTAWA_CCN_PROJECT_CHATAPP_INC_APPLICATIONTWO_HPP_

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include<signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#define PORT 8080
#define SUCCESS true
#define FAILURE false
typedef void * (*ThreadFunPtr)(void *);

using namespace std;

class applicationTwo
{

/*
 * Member function Declaration
 */
private:


	void appGoesOffline();
	void * dispatcher(void *arg);
	void *recieve (void *arg);


public:
	applicationTwo():serverSockFD(0),conSocket(0){}
	bool initalize ();
	bool execute(applicationTwo *objPtrHandler);
	int sendThreadExit(int signo);
	int receiveThreadExit(int signo);
	void systemIPDisaply();
	~applicationTwo();

/*
 * Member variable declaration
 */
private:
	int serverSockFD;
	int conSocket;
	struct sockaddr_in address;
public:
	pthread_t tid[2];

};
void sig_handler(int signo);
void destory();
applicationTwo *applicationTwoOBJ = NULL;





#endif /* GIT_UOTTAWA_CCN_PROJECT_CHATAPP_INC_APPLICATIONTWO_HPP_ */
