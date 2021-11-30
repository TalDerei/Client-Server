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

#include "vec.h"

using namespace std;

/** Connect to primary or backup server */
int connect_to_server(string, size_t);

/** Execute API function on primary or backup server */
void write_to_server(int, const char *);



/// Send a vector of data over a socket.
///
/// @param sd  The socket on which to send
/// @param msg The message to send
///
/// @returns True if the whole vector was sent, false otherwise
bool send_reliably(int sd, const vec &msg);

/// Send a string over a socket.
///
/// @param sd  The socket on which to send
/// @param msg The message to send
///
/// @returns True if the whole string was sent, false otherwise
bool send_reliably(int sd, const std::string &msg);

/// Perform a reliable read when we are not sure how many bytes we are going
/// to receive.
///
/// @param sd     The socket from which to read
/// @param buffer The buffer into which the data should go.  It is assumed
/// to be
///               pre-allocated to max bytes.
/// @param max    The maximum number of bytes to get
///
/// @returns A vector with the data that was read, or an empty vector on
/// error
vec reliable_get_to_eof(int sd);

#endif