#include "applicationTwo.hpp"

#define PORT 8080

void destory(int signo) {

	int status = 0;

	if (signo == SIGUSR1) {
		applicationTwoOBJ->sendThreadExit(signo);
	} else if (signo == SIGUSR2) {
		applicationTwoOBJ->receiveThreadExit(signo);
	} else if (signo == SIGINT) {
		cout << "[ALERT]- !Received Temrination Signal!" << endl;
		status = pthread_kill(applicationTwoOBJ->tid[0], SIGUSR1);
		if (status < 0)
			perror("send pthread_kill failed");
		status = pthread_kill(applicationTwoOBJ->tid[1], SIGUSR2);
		if (status < 0)
			perror("recv pthread_kill failed");
	} else {
		cout << "[ERROR]- Undesired behaviour - SHUTDOWN" << endl;
		exit(0);
	}
}

void sig_handler(int signo) {

	destory(signo);

}

void applicationTwo::appGoesOffline() {
	std::cout << "[CRITICAL]- Application 1 Went !OFFLINE!" << std::endl;
	std::cout << "[ALERT]- !I am Shutting Down!" << std::endl;
	int status = pthread_kill(tid[0], SIGUSR1);
	if (status < 0)
		perror("send pthread_kill failed");
	status = pthread_kill(tid[1], SIGUSR2);
	if (status < 0)
		perror("recv pthread_kill failed");
}

int applicationTwo::sendThreadExit(int signo) {
	std::cout << "[ALERT]- !Stopping!-Dispatcher Thread" << std::endl;
	sleep(1);
	string msgInformation = "App2 going Offline";
	send(conSocket, msgInformation.c_str(), msgInformation.length(), 0);
	pthread_exit(0);

}

int applicationTwo::receiveThreadExit(int signo) {
	std::cout << "[ALERT]- !Stopping! Receiver Thread" << std::endl;
	pthread_exit(0);

}

void * applicationTwo::dispatcher(void *arg) {
	cout << "[INFO] - !STARTS! Dispatcher Thread " << endl;
	string msgInformation;

	if (signal(SIGUSR1, destory) == SIG_ERR)
		cout << "[SYSTEM ERROR]-can't catch SIGUSR1" << endl;

	while (1) {
		cin >> msgInformation;
		send(conSocket, msgInformation.c_str(), msgInformation.length(), 0);
	}

}

void * applicationTwo::recieve(void *arg) {
	cout << "[INFO] !START! - Receiver Thread" << endl;
	char buffer[1024] = { 0 };

	string conTerminateReqMsg = "App1 going Offline";

	if (signal(SIGUSR2, destory) == SIG_ERR)
		cout << "[SYSTEM ERROR]- can't catch SIGUSR2" << endl;

	while (1) {
		read(conSocket, buffer, 1024);

		cout << "[INFO] Message from Application 2:" << buffer << endl;
		if (strcmp(buffer, conTerminateReqMsg.c_str()) == 0) {
			close(conSocket);
			cout << "[WARN]-Application 2 Requested for Disconnection!" << endl;
			appGoesOffline();
			break;
		}
		memset(buffer,0,1024);
	}
}

bool applicationTwo::initalize() {

	int opt = 1;
	if ((serverSockFD = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		cout << "[SYSTEM ERROR]- cannot obtain valid FD" << endl;
		return FAILURE;
	}

	if (setsockopt(serverSockFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
			sizeof(opt))) {
		perror("setsockopt");
		exit (EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT);

	// Forcefully attaching socket to the port 8080
	if (bind(serverSockFD, (struct sockaddr *) &address, sizeof(address)) < 0) {
		perror("bind failed");
		exit (EXIT_FAILURE);
	}
	if (listen(serverSockFD, 3) < 0) {
		perror("listen");
		exit (EXIT_FAILURE);
	}

	return SUCCESS;

}

bool applicationTwo::execute(applicationTwo *objPtrHandler) {

	int addrlen = sizeof(address);
	if((conSocket=accept(serverSockFD, (struct sockaddr *) &address, (socklen_t*) &addrlen)) <0)
	{
		perror("accept");
		exit (EXIT_FAILURE);
	}
	/*
	 * Thread create
	 */

	if (pthread_create(&tid[0], NULL,
			(ThreadFunPtr) &applicationTwo::dispatcher, objPtrHandler) != 0) {
		cout << "[SYSTEM ERROR]- Failed to create Dispatcher thread\n" << endl;
	}

	if (pthread_create(&tid[1], NULL, (ThreadFunPtr) &applicationTwo::recieve,
			objPtrHandler) != 0) {
		cout << "[SYSTEM ERROR]- Failed to create Receiver thread\n" << endl;
	}

	int i = 0;
	while (i < 2) {
		pthread_join(tid[i++], NULL);
		printf("Thread %d:\n", i);
	}
	return SUCCESS;
}

int main(int argc, char const *argv[]) {

	if (signal(SIGINT, sig_handler) == SIG_ERR)
		cout << "[SYSTEM ERROR] -can't catch SIGINT" << endl;

	applicationTwoOBJ = new applicationTwo;
	applicationTwoOBJ->initalize();
	applicationTwoOBJ->execute(applicationTwoOBJ);
	/*
	 * Thread create
	 */

	cout << "Final program termainate" << endl;
	//close(sock);
	/*send(sock , hello , strlen(hello) , 0 );
	 printf("Hello message sent\n");
	 valread = read( sock , buffer, 1024);
	 printf("%s\n",buffer ); */
	return 0;
}

