/**
 * @file client_commands.h 
 */

#ifndef CLIENT_COMMANDS_DEF
#define CLIENT_COMMANDS_DEF

#pragma once

#include <string>

/**
 * @brief Send vector representation of key/value pair to server
 * 
 * @param sd  socket descriptor
 * @param cmd API command (insert/remove/contains)
 * @param msg key/value pair
 * @return    vec 
 */
vec client_send_cmd(int sd, const string &cmd, const vec &msg);

/**
 * @brief Insert API command instructing server to insert key/value pair into lazy linked-list
 * 
 * @param sd  socket descriptor
 * @param key key
 * @param val value
 */
void client_insert(int sd, const string &key, const string &val);

/**
 * @brief Remove API command instructing server to remove key/value pair from lazy linked-list
 * 
 * @param sd  socket descriptor
 * @param key key
 * @param val value
 */
void client_remove(int sd, const string &key, const string &val);

/**
 * @brief Contains API command instructing server to check if key/value pair exists in lazy linked-list
 * 
 * @param sd  socket descriptor
 * @param key key
 * @param val value
 */
void client_contains(int sd, const string &key, const string &val);

#endif