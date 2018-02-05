#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#define MAXPENDING 5
# define BUFFERSIZE 32
int main() {
        /*CREATE A TCP SOCKET*/
        int serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket < 0) {
                printf("Error while server socket creation");
                exit(0);
        }
        printf("Server Socket Created\n");
        /*CONSTRUCT LOCAL ADDRESS STRUCTURE*/
        struct sockaddr_in serverAddress, clientAddress;
        memset( & serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(1234);

        //serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_addr.s_addr = inet_addr("172.18.18.7");
        printf("Server address assigned\n");
        int temp = bind(serverSocket, (struct sockaddr * ) & serverAddress, sizeof(serverAddress));
        if (temp < 0) {
                printf("Error while binding\n");
                exit(0);
        }
        printf("Binding successful\n");
        int temp1 = listen(serverSocket, MAXPENDING);
        if (temp1 < 0) {
                printf("Error in listen");
                exit(0);
        }
        printf("Now Listening\n");
        char msg[BUFFERSIZE];
        int clientLength = sizeof(clientAddress);
	int socket_array[2];
	
	for(int i=0;i<2;i++){
		socket_array[i] = accept(serverSocket, (struct sockaddr * ) & clientAddress, & clientLength);
		if (clientLength < 0) {
		        printf("Error in client socket");
		        exit(0);
        	}
		printf("Handling Client %d %s\n",(i+1), inet_ntoa(clientAddress.sin_addr));		
	}    

	for(int i=0;i<2;i++){		
		int temp2 = recv(socket_array[i], msg, BUFFERSIZE, 0);
		if (temp2 < 0) {
		        printf("problem in temp 2");
		        exit(0);
		}
		printf("%s\n", msg);
		printf("ENTER MESSAGE FOR CLIENT %d\n",i+1);
		gets(msg);
		int bytesSent = send(socket_array[i], msg, strlen(msg), 0);
		if (bytesSent != strlen(msg)) {
		        printf("Error while sending message to client");
		        exit(0);
		}
		close(socket_array[i]);
	}      
        
        close(serverSocket);
        
}
