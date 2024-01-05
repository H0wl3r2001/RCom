#ifndef ARGS_H
#define ARGS_H

#include "utils.h"

int constructUrlAnonymous(const char *url_arg, int start_url_path_pos, struct url *result);
int constructUrlNotAnonymous(const char *url_arg, int start_url_path_pos,int user_password_pos, struct url *result);
int parseArguments(const char *url_arg, struct url *result);

#endif