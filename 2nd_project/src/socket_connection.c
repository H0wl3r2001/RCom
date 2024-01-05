#include "socket_connection.h"

int startConnection(char *ip, int port, int *sockfd)
{

    struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip); /*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(port);          /*server TCP port must be network byte ordered */

    /*open a TCP socket*/
    if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket()");
        exit(-1);
    }
    /*connect to the server*/
    if (connect(*sockfd,
                (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0)
    {
        perror("connect()");
        exit(-1);
    }

    return 0;
}

int getIpAndPort(char *response, struct serverResponse *serverRes)
{
    int numBytesIp = 3;
    size_t size = 0, len;
    int bytes[2];

    char *token = strtok(response, ",");
    len = strlen(token);

    strcpy(serverRes->ip, token);
    serverRes->ip[len] = '.';
    size = len;

    while (token != NULL)
    {
        //Last 2 bytes
        if (numBytesIp == 1)
        {
            token = strtok(NULL, ",");
            strcpy(serverRes->ip + size + 1, token);

            len = strlen(token);
            size += len + 1;
            endString(serverRes->ip, size);

            numBytesIp--;
        }
        else if (numBytesIp > 0)
        {
            token = strtok(NULL, ",");
            strcpy(serverRes->ip + size + 1, token);

            len = strlen(token);
            size += len + 1;
            serverRes->ip[size] = '.';

            numBytesIp--;
        }
        //First byte of port
        else
        {
            token = strtok(NULL, ",");
            bytes[0] = atoi(token);
            break;
        }
    }

    token = strtok(NULL, ")");
    bytes[1] = atoi(token);

    serverRes->port = bytes[0] * 256 + bytes[1];

    return 0;
}

int parsingServerResponse(char *buffer, struct serverResponse *serverRes)
{
    char *pos = strchr(buffer, '(');
    char data[MAX];

    removeHeadTail(pos, data);
    getIpAndPort(data, serverRes);
    return 0;
}

int readResponse(FILE *socketFile, char *buffer)
{

    char *result;
    char status_code[4];

    while (buffer[3] != ' ')
    {
        memset(buffer, 0, MAX);
        result = fgets(buffer, MAX, socketFile);
        puts(buffer);
    }

    strncpy(status_code, buffer, 3);
    endString(status_code, 3);
    printf("status_code:%s\n", status_code);

    if (strcmp(status_code, "550") == 0)
    {
        printf("File unavailable.");
        return -1;
    }
    else if (strcmp(status_code, "530") == 0)
    {
        printf("Not logged in.");
        return -1;
    }
    else
        fflush(socketFile);
    return 0;
}

int writeCmd(char *cmd, int socketFile)
{
    int numBytesWritten = send(socketFile, cmd, strlen(cmd), 0);
    if (numBytesWritten < 0)
    {
        printf("send() failed");
        return -1;
    }
    return 0;
}

int user_(struct url *url_res, FILE *socketFile, int socketFd)
{
    printf("user\n");
    char temp[MAX];
    char user[MAX] = "user ";
    size_t len = strlen(url_res->user);

    strncpy(user + CMD_SIZE, url_res->user, len);
    len += CMD_SIZE;
    user[len] = '\n';

    writeCmd(user, socketFd);
    readResponse(socketFile, temp);
    memset(temp, 0, strlen(temp));
    return 0;
}

int password_(struct url *url_res, FILE *socketFile, int socketFd)
{
    printf("password\n");
    char pass[MAX] = "pass ";
    char temp[MAX];
    size_t len = strlen(url_res->password);

    strncpy(pass + CMD_SIZE, url_res->password, len);
    len += CMD_SIZE;
    pass[len] = '\n';

    writeCmd(pass, socketFd);
    readResponse(socketFile, temp);
    memset(temp, 0, strlen(temp));

    return 0;
}

int login_(struct url *url_res, FILE *socketFile, int socketFd)
{
    user_(url_res, socketFile, socketFd);
    password_(url_res, socketFile, socketFd);
    return 0;
}

int readFile(int socketFd, char *filename)
{
    char buffer[MAX];
    int numbytes;

    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0777);

    while ((numbytes = recv(socketFd, buffer, MAX, 0)) > 0)
    {
        write(fd, buffer, numbytes);
    }

    printf("Downloader: File downloaded\n");

    close(fd);
    return 0;
}

int downloadFile(struct url * url_res, FILE *socketFile, int sockPassMode, int sockfd)
{
    char retr[MAX] = "retr ";
    char temp[MAX];

    size_t len = strlen(url_res->url_path);

    strncpy(retr + CMD_SIZE, url_res->url_path, len);
    len += CMD_SIZE;
    retr[len] = '\n';

    writeCmd(retr, sockfd);

    readResponse(socketFile, temp);
    memset(temp, 0, strlen(temp));
    readFile(sockPassMode, url_res->filename);

    readResponse(socketFile, temp);
    memset(temp, 0, strlen(temp));

    return 0;
}

int startPassiveMode(int sockfd)
{
    char pasv[MAX] = "pasv\n";
    writeCmd(pasv, sockfd);
    return 0;
}

int passiveMode(int sockfd, FILE *socketFile, struct serverResponse *serverRes)
{
    char buffer[MAX];

    startPassiveMode(sockfd);
    readResponse(socketFile, buffer);
    parsingServerResponse(buffer, serverRes);

    return 0;
}

int closeConnection(int sockfd)
{
    if (close(sockfd) < 0)
    {
        perror("close()");
        exit(-1);
    }
    return 0;
}
