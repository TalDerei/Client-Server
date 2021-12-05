/**
 * @file server_parsing.h 
 */

#ifndef SERVER_PARSING_DEF
#define SERVER_PARSING_DEF

#include "server_storage.h"

/**
 * @brief Serve client connection and execute requested API
 * 
 * @param sd      The socket on which communication with the client takes place
 * @param storage The Storage object with which clients interact
 * @return true if the server should halt immediately, false otherwise 
 */
bool serve_client(int sd, Storage &storage);

#endif