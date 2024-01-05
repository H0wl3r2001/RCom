#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "args.h"
#include "socket_connection.h"

#define PORT 21


int main(int argc, char *argv[])
{
    struct url url_res;
    struct serverResponse serverRes;
    int sockfd, sockPassMode;
    char temp[MAX];

    if (argc > 2)
        return 1;
    
    if (parseArguments(argv[1], &url_res) < 0)
        return -1;

    showUrl(url_res);

    if (startConnection(url_res.ip, PORT, &sockfd) < 0)
    {
        perror("Error starting the connection with sockets");
        return -1;
    }

    printf("Started connection...\n");

    FILE *socketFile = fdopen(sockfd, "r+");
    if (socketFile == NULL)
    {
        perror("Error opening file");
        return (-1);
    }

    //Initial message
    printf("Initial message\n");
    readResponse(socketFile, temp);
    memset(temp,0,strlen(temp));
    //Login
    printf("Login\n");
    login_(&url_res, socketFile, sockfd);

    //Entering in passive mode
    printf("Passive Mode\n");
    passiveMode(sockfd, socketFile, &serverRes);
    
    //Starting new socket with the ip and port given by the command pasv
    if (startConnection(serverRes.ip, serverRes.port, &sockPassMode) < 0)
    {
        perror("Error starting the connection with sockets");
        return -1;
    }

    //Download file

    downloadFile(&url_res, socketFile, sockPassMode , sockfd);
    printf("File downloaded\n");
    closeConnection(sockfd);
    closeConnection(sockPassMode);

    return 0;
}
