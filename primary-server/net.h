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

#include "vec.h"
#include <functional>

/** Error message */
void error_message_and_exit(std::size_t, std::size_t, const char *);

/** Handling multiple clients */
bool handle_client_input(int);

/** Send response to client */
void echo_server(int, bool);

/** Create server socket for client to connect to */
int create_server_socket(std::size_t);




/// Print an error message that combines some application-specific text with the
/// standard unix error message that accompanies errno.
///
/// @param err    The error code that was generated by the program
/// @param prefix The text to display before the error message
void sys_error(int err, const char *prefix);
/// Perform a reliable read when we have a guess about how many bytes we might
/// get, but it's OK if the socket EOFs before we get that many bytes.
///
/// @param sd   The socket from which to read
/// @param pos  The start of the vector where datashould go.  It is assumed to
///             be pre-allocated to amnt or more.
/// @param amnt The maximum number of bytes to get
///
/// @returns The actual number of bytes read, or -1 on a non-eof error
int reliable_get_to_eof_or_n(int sd, vec::iterator pos, int amnt);

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

/// Given a listening socket, start calling accept() on it to get new
/// connections.  Each time a connection comes in, use the provided handler to
/// process the request.  Note that this is not multithreaded.  Only one client
/// will be served at a time.
///
/// @param sd The socket file descriptor on which to call accept
/// @param handler A function to call when a new connection comes in
void accept_client(int sd, std::function<bool(int)> handler);
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

#endif