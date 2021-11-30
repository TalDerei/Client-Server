#pragma once

#include "vec.h"
#include "server_storage.h"

/// Respond to an ALL command by generating a list of all the usernames in the
/// Auth table and returning them, one per line.
///
/// @param sd      The socket onto which the result should be written
/// @param storage The Storage object, which contains the auth table
/// @param req     The unencrypted contents of the request
///
/// @returns false, to indicate that the server shouldn't stop
bool server_cmd_kvi(int sd, const vec &req, Storage &storage);
bool server_cmd_kvg(int sd, const vec &req, Storage &storage);
bool server_cmd_kvd(int sd, const vec &req, Storage &storage);
