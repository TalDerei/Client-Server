/**
 * @file net.h 
 */

#ifndef NET_SERVER_DEF
#define NET_SERVER_DEF

#include <arpa/inet.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>

/** Error message */
void error_message_and_exit(std::size_t, std::size_t, const char *);

/** Handling multiple clients */
bool handle_client_input(int);

/** Send response to client */
void echo_server(int, bool);

/** Create server socket for client to connect to */
int create_server_socket(std::size_t);

#endif