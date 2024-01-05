#include "args.h"

int constructUrlAnonymous(const char *url_arg, int start_url_path_pos, struct url *result)
{
    strcpy(result->user, "anonymous");
    strcpy(result->password, "pass");
    strncpy(result->host, url_arg + FTP_HEADER, start_url_path_pos);

    endString(result->user, 9);
    endString(result->password, 4);
    endString(result->host, start_url_path_pos);
    return 0;
}

int constructUrlNotAnonymous(const char *url_arg, int start_url_path_pos,int user_password_pos, struct url *result)
{
    int start_host_pos = find_first('@', url_arg + FTP_HEADER);
    if (start_host_pos < 0)
        return -1;

    strncpy(result->user, url_arg + FTP_HEADER, user_password_pos);
    strncpy(result->password, url_arg + FTP_HEADER + user_password_pos + 1, start_host_pos - user_password_pos - 1);
    strncpy(result->host, url_arg + FTP_HEADER + start_host_pos + 1, start_url_path_pos - start_host_pos - 1);

    endString(result->user, user_password_pos);
    endString(result->password, start_host_pos - user_password_pos - 1);
    endString(result->host, start_url_path_pos - start_host_pos - 1);
    return 0;
}

int parseArguments(const char *url_arg, struct url *result)
{
    char * filename;

    printf("Parsing Arguments ...\n");
    int user_password_pos = find_first(':', url_arg + 6);
    int start_url_path_pos = find_first('/', url_arg + 6);

    //Doesn't follow the structure of ftp
    if (start_url_path_pos < 0)
    {
        return -1;
    }
    //User and password not defined in the arguments
    if (user_password_pos < 0)
    {
        printf("anonymous\n");
        constructUrlAnonymous(url_arg, start_url_path_pos, result);
    }
    //User and password defined in the arguments
    else
    {
        printf("not anonymous\n");
        constructUrlNotAnonymous(url_arg, start_url_path_pos,user_password_pos,result);
    }
    strcpy(result->url_path, url_arg + FTP_HEADER + start_url_path_pos + 1);
  
    
    filename = strrchr(result->url_path, '/');
    if(filename == NULL) strcpy(result->filename, result->url_path); 
    else strcpy(result->filename, filename + 1);
    
    getIp(result);

    return 0;
}
