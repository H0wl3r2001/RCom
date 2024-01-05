#ifndef SOCKET_CONNECTION_H
#define SOCKET_CONNECTION_H

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <fcntl.h>

#include "utils.h"

struct serverResponse
{
    char ip[MAX];
    int port;
};

int startConnection(char *ip, int port, int *sockfd);
int getIpAndPort(char *response, struct serverResponse *serverRes);
int parsingServerResponse(char *buffer, struct serverResponse *serverRes);
int readResponse(FILE *socketFile, char * buffer);
int writeCmd(char *cmd, int socketFile);
int user_(struct url *url_res, FILE *socketFile, int socketFd);
int password_(struct url *url_res, FILE *socketFile, int socketFd);
int login_(struct url *url_res, FILE *socketFile, int socketFd);
int readFile(int socketFd, char *filename);
int downloadFile(struct url * url_res, FILE *socketFile, int sockPassMode, int sockfd);
int startPassiveMode(int sockfd);
int passiveMode(int sockfd, FILE * socketFile, struct serverResponse * serverRes);
int closeConnection(int sockfd);

#endif