/**
 * @file client_commands.cc
 */

#include <iostream>
#include <string>

#include "net.h"
#include "protocols.h"
#include "vec.h"

#include "client_commands.h"

using namespace std;

/**
 * @brief Send vector representation of key/value pair to server
 * 
 * @param sd  socket descriptor
 * @param cmd API command (insert/remove/contains)
 * @param msg key/value pair
 * @return    vec 
 */
vec client_send_cmd(int sd, const string &cmd, const vec &msg) {
    /** Append cmd and msg to vec */
    vec req;
    vec_append(req, cmd);
    vec_append(req, msg.size());
    vec_append(req, msg);
    cout << "req.size(): " << req.size() << endl;
    for (unsigned int i=0; i < req.size(); i++) {
        cout << "req: " << req.at(i) << endl;
    }

    /** Send vector packet to server on specified socket descriptor */
    send_reliably(sd, req);
    vec res = reliable_get_to_eof(sd);
    return res;
}

/**
 * @brief Insert API command instructing server to insert key/value pair into lazy linked-list
 * 
 * @param sd  socket descriptor
 * @param key key
 * @param val value
 */
void client_insert(int sd, const string &key, const string &val) {
    /** Key/Value pair */
    cout << "Client Key: " << key << endl;
    cout << "Client Val: " << val << endl;  

    /** Append key/value to msg vector */
    vec msg;
    vec_append(msg, key.length());
    vec_append(msg, key);
    vec_append(msg, val.length());
    vec_append(msg, val);
    auto res = client_send_cmd(sd, REQ_KVI, msg);
    for (unsigned int i = 0; i < res.size(); i++) {
        cout << "res[i]: " << res.at(i) << endl;
    }
    //check_err_response(res);
}

/**
 * @brief Remove API command instructing server to remove key/value pair from lazy linked-list
 * 
 * @param sd  socket descriptor
 * @param key key
 * @param val value
 */
void client_remove(int sd, const string &key, const string &val) {
    /** Append key to msg vector */   
    vec msg;
    vec_append(msg, key.length());
    vec_append(msg, key);
    auto res = client_send_cmd(sd, REQ_KVD, msg);
    for (unsigned int i = 0; i < res.size(); i++) {
        cout << "res[i]: " << res.at(i) << endl;
    }
    //check_err_response(res);
}

/**
 * @brief Contains API command instructing server to check if key/value pair exists in lazy linked-list
 * 
 * @param sd  socket descriptor
 * @param key key
 * @param val value
 */
void client_contains(int sd, const string &key, const string &val) {
    /** Append key to msg vector */   
    vec msg;
    vec_append(msg, key.length());
    vec_append(msg, key);
    auto res = client_send_cmd(sd, REQ_KVG, msg);
    for (unsigned int i = 0; i < res.size(); i++) {
        cout << "res[i]: " << res.at(i) << endl;
    }
    //check_err_response(res);
}
