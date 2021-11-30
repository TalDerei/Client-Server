/**
 * @file server_commands.h 
 */

#ifndef SERVER_COMMANDS_DEF
#define SERVER_COMMANDS_DEF

#pragma once

#include "vec.h"
#include "server_storage.h"

/**
 * @brief Server command servering the Insert API call 
 * 
 * @param sd      The socket onto which the result should be written
 * @param req     The unencrypted contents of the request
 * @param storage The Storage object
 * @return        false, to indicate that the server shouldn't stop 
 */
bool server_cmd_kvi(int sd, const vec &req, Storage &storage);

/**
 * @brief Server command servering the Remove API call 
 * 
 * @param sd      The socket onto which the result should be written
 * @param req     The unencrypted contents of the request
 * @param storage The Storage object
 * @return        false, to indicate that the server shouldn't stop 
 */
bool server_cmd_kvg(int sd, const vec &req, Storage &storage);

/**
 * @brief Server command servering the Contains API call 
 * 
 * @param sd      The socket onto which the result should be written
 * @param req     The unencrypted contents of the request
 * @param storage The Storage object
 * @return        false, to indicate that the server shouldn't stop 
 */
bool server_cmd_kvd(int sd, const vec &req, Storage &storage);

#endif