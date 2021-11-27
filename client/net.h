/**
 * @file net.h 
 */

#ifndef NET_CLIENT_DEF
#define NET_CLIENT_DEF

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <netdb.h>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

/** Connect to primary or backup server */
int connect_to_server(string, size_t);

/** Execute API function on primary or backup server */
void write_to_server(int, const char *);

#endif