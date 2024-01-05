#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX 64
#define FTP_HEADER 6
#define CMD_SIZE 5

struct url
{
    char user[MAX];
    char password[MAX];
    char host[MAX];
    char host_name[MAX];
    char url_path[MAX];
    char filename[MAX];
    char ip[MAX];
};

int find_first(char del, const char *str);
int removeHeadTail(char *pos, char *cleaned);
int endString(char * str, int size);
void showUrl(struct url url_res);
int getIp(struct url *url_res);

#endif
