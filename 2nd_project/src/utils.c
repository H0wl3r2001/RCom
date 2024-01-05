#include "utils.h"

int find_first(char del, const char *str)
{
    for (size_t i = 0; i < strlen(str); i++)
    {
        if (str[i] == del)
            return i;
    }
    return -1;
}

int removeHeadTail(char *pos, char *cleaned)
{
    size_t len = strlen(pos);

    strncpy(cleaned, pos + 1, len - 5);
    cleaned[len - 5] = '\0';

    return 0;
}

int endString(char * str, int size)
{
    str[size] = '\0';
    return 0;
}

void showUrl(struct url url_res)
{
    printf("USER: %s\n", url_res.user);
    printf("PASSWORD: %s\n", url_res.password);
    printf("HOST: %s\n", url_res.host);
    printf("URL_PATH: %s\n", url_res.url_path);
    printf("FILENAME: %s\n", url_res.filename);
}

int getIp(struct url * url_res)
{
    struct hostent *hostent;
    if ((hostent = gethostbyname(url_res->host)) == NULL)
    {
        herror("gethostbyname()");
        return -1;
    }

    strcpy(url_res->host_name, hostent->h_name);
    strcpy(url_res->ip, inet_ntoa(*((struct in_addr *)hostent->h_addr)));

    return 0;
}