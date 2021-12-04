/**
 * @file server_parsing.cc 
 */

#include <iostream>
#include <string>

#include "vec.h"
#include "net.h"
#include "protocol.h"
#include "server_commands.h"
#include "server_storage.h"

/**
 * @brief Serve client connection and execute requested API
 * 
 * @param sd      The socket on which communication with the client takes place
 * @param storage The Storage object with which clients interact
 * @return true if the server should halt immediately, false otherwise 
 */
bool serve_client(int sd, Storage &storage) {
    /* request block */
    vec res(LEN_RKBLOCK);

    /* get request from socket */
    reliable_get_to_eof_or_n(sd, res.begin(), LEN_RKBLOCK);

    /* recognize API command */
    std::string cmd = "";
    unsigned int j = 0;
    unsigned char alen[4];
    for (unsigned i=0; i < LEN_RKBLOCK; i++) {
        if (i < 3) {
            cmd += res[i];
        } else if (i >= 3 && i < 7) {
            alen[j] = res[i];
            j++;
        }
    }

    /* parse content */
    int alen_int = *(int*) alen;
    vec msg(alen_int);
    reliable_get_to_eof_or_n(sd, msg.begin(), alen_int);

    /* execute a command */
    std::vector<std::string> s = {REQ_KVI, REQ_KVG, REQ_KVD};
    decltype(server_cmd_kvi) *cmds[] = {server_cmd_kvi, server_cmd_kvg, server_cmd_kvd};
    for (size_t i = 0; i < s.size(); ++i) {
        if (cmd == s[i]) {
            return cmds[i](sd, msg, storage);
        }
    }
    return true;
}
