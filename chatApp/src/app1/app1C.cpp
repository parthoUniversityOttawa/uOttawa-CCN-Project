
#include "applicationOne.hpp"

#define PORT 8080 

void destory(int signo) {

	int status =0;

	if (signo == SIGUSR1) {
		applicationOneOBJ->sendThreadExit(signo);
	} else if (signo == SIGUSR2) {
		applicationOneOBJ->receiveThreadExit(signo);
	} else if (signo == SIGINT) {
		cout << "[ALERT]- !Received Temrination Signal!" << endl;
		status = pthread_kill(applicationOneOBJ->tid[0], SIGUSR1);
		if (status < 0)
			perror("send pthread_kill failed");
		status = pthread_kill(applicationOneOBJ->tid[1], SIGUSR2);
		if (status < 0)
			perror("recv pthread_kill failed");
	}
	else
	{
		cout << "[ERROR]- Undesired behaviour - SHUTDOWN" << endl;
		exit(0);
	}
}



void sig_handler(int signo) {

	destory(signo);

}

void applicationOne::appGoesOffline() {
	std::cout << "[CRITICAL]- Application 2 Went !OFFLINE!" << std::endl;
	std::cout << "[ALERT]- !I am Shutting Down!" << std::endl;
	int status = pthread_kill(tid[0], SIGUSR1);
	if (status < 0)
		perror("send pthread_kill failed");
	status = pthread_kill(tid[1], SIGUSR2);
	if (status < 0)
		perror("recv pthread_kill failed");
}

int applicationOne::sendThreadExit(int signo) {
	std::cout << "[ALERT]- !Stopping!-Dispatcher Thread" << std::endl;
	sleep(1);
	string msgInformation = "App1 going Offline";
	send(sockFD, msgInformation.c_str(), msgInformation.length(), 0);
	pthread_exit(0);

}

int applicationOne::receiveThreadExit(int signo) {
	std::cout << "[ALERT]- !Stopping! Receiver Thread" << std::endl;
	pthread_exit(0);

}


void * applicationOne::dispatcher(void *arg) {
	cout << "[INFO] - !STARTS! Dispatcher Thread " << endl;
	string msgInformation;

	if (signal(SIGUSR1, destory) == SIG_ERR)
		cout << "[SYSTEM ERROR]-can't catch SIGUSR1" << endl;

	while (1) {
		cin >> msgInformation;
		send(sockFD, msgInformation.c_str(), msgInformation.length(), 0);
	}

}

void * applicationOne::recieve(void *arg) {
	cout << "[INFO] !START! - Receiver Thread" << endl;
	char buffer[1024] = { 0 };

	string conTerminateReqMsg = "App2 going Offline";

	if (signal(SIGUSR2, destory) == SIG_ERR)
		cout << "[SYSTEM ERROR]- can't catch SIGUSR2" << endl;

	while (1) {
		read(sockFD, buffer, 1024);

		cout << "[INFO] Message from Application 2:" << buffer << endl;
		if (strcmp(buffer, conTerminateReqMsg.c_str()) == 0) {
			close(sockFD);
			cout << "[WARN]-Application 2 Requested for Disconnection!" << endl;
			appGoesOffline();
			break;
		}
	}
}

bool applicationOne::initalize() {

	if ((sockFD = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		cout << "[SYSTEM ERROR]- can't catch SIGUSR2" << endl;
		return FAILURE;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		cout << "[ERROR]-Invalid address/ Address not supported " << endl;
		return FAILURE;
	}

	return SUCCESS;

}

bool applicationOne::execute(applicationOne *objPtrHandler) {
	if (connect(sockFD, (struct sockaddr *) &serv_addr, sizeof(serv_addr))
			< 0) {
		cout << "[ERROR] - Connection Failed \n" << endl;
		return FAILURE;
	}

	/*
	 * Thread create
	 */

	if (pthread_create(&tid[0], NULL,
			(ThreadFunPtr) &applicationOne::dispatcher, objPtrHandler) != 0) {
		cout << "[SYSTEM ERROR]- Failed to create Dispatcher thread\n" << endl;
	}

	if (pthread_create(&tid[1], NULL, (ThreadFunPtr) &applicationOne::recieve,
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

	applicationOneOBJ = new applicationOne;
	applicationOneOBJ->initalize();
	applicationOneOBJ->execute(applicationOneOBJ);
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

