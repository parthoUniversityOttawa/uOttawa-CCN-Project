/************************************************************************************
 * University Of Ottawa
 * Project Presentation for Computer Communications Network.
 *
 * This software embodies materials and concepts which are
 * Used for the sole purpose to demonstrate - TCP- Socket Programming Interface.
 * Furthermore, This software is capable to perform Full duplex communication in Intra
 * domain Infrastructure
 *
 *
 * Designed and Developed by Uottawa MEng Students
 *
 * file          applicationOne.hpp
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
#define PORT 8080
#define SUCCESS true
#define FAILURE false
typedef void * (*ThreadFunPtr)(void *);

using namespace std;


class applicationOne
{

/*
 * Member function Declaration
 */
private:


	void appGoesOffline();
	void * dispatcher(void *arg);
	void *recieve (void *arg);


public:
	applicationOne():sockFD(0){}
	bool initalize ();
	bool execute(applicationOne *objPtrHandler);
	int sendThreadExit(int signo);
	int receiveThreadExit(int signo);
	void systemIPDisaply();
	~applicationOne();

/*
 * Member variable declaration
 */
private:
	int sockFD;
	struct sockaddr_in serv_addr;
public:
	pthread_t tid[2];

};
void sig_handler(int signo);
void destory();
applicationOne *applicationOneOBJ = NULL;

#endif /* UOTTAWA_CCN_PROJECT_CHATAPP_INC_APPLICATIONTWO_HPP_ */
