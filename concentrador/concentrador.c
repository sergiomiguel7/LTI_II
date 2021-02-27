
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "api.h"

//functions declaration 
int connectSocket();



int serverFd;


int main(){

    sockManager = connectSocket();
    if(sockManager > 0){
        printf("Socket connected");
    }

}


/*
*   function to create a ipv4 socket connection
*   returns the socket file descriptor
*/
int connectSocket(){
    int sock = 0;

    //try to create a new default socket with ipv4 domain
    if(sock = socket(AF_INET, SOCK_STREAM, 0) < 0){
        printf("Socket creation error \n");
		return -1;
    }

    memset(&servAddr, '0', sizeof(servAddr));

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary form to config socket
	if (inet_pton(AF_INET, ADDR, &servAddr.sin_addr) <= 0)
	{
		printf("Invalid address/ Address not supported \n");
		return -1;
	}
    //servAddr configured with sucess, now connect socket
	if (connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
	{
		printf("Connection Failed \n");
		return -1;
	}

	return sock;
}
