#pragma once

#include <string>

vec client_send_cmd(int sd, const string &cmd, const vec &msg);
void client_insert(int sd, const string &key, const string &val);
void client_remove(int sd, const string &key, const string &val);
void client_contains(int sd, const string &key, const string &val);
