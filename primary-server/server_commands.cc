#include <iostream>
#include <string>

#include "server_commands.h"
#include "server_storage.h"
#include "net.h"

using namespace std;

/// Respond to an ALL command by generating a list of all the usernames in the
/// Auth table and returning them, one per line.
///
/// @param sd      The socket onto which the result should be written
/// @param storage The Storage object, which contains the auth table
/// @param req     The unencrypted contents of the request
///
/// @returns false, to indicate that the server shouldn't stop
bool server_cmd_kvi(int sd, const vec &req, Storage &storage) {
    std::string key_str = "";
    std::string val_str = "";
    cout << "test! " << endl;
    cout << "size! " << req.size() << endl;
    unsigned char ulen[4] = {req.at(0), req.at(1), req.at(2), req.at(3)};
    int usize = *(int*) ulen;
    cout << "usize: " << usize << endl;
    for (int i=4; i < usize+4; i++) {
        key_str += req[i];
        cout << "req[i]: " << req.at(i) << endl;
    }
    unsigned char plen[4] = {req.at(usize+4), req.at(usize+5), req.at(usize+6), req.at(usize+7)};
    int psize = *(int*) plen;
    for (int i=usize+8; i < usize+8+psize; i++) {
        val_str += req[i];
    }

    cout << "key_str: " << key_str << endl;
    cout << "val_str: " << val_str << endl;

    int key = stoi(key_str);
    int val = stoi(val_str);

    cout << "key: " << key << endl;
    cout << "val: " << val << endl;
    vec status = storage.kv_insert(key, val);

    cout << "sending..." << endl;
    send_reliably(sd, status);
    cout << "sent..." << endl;
    return false;
}

bool server_cmd_kvg(int sd, const vec &req, Storage &storage) {
    std::string key_str = "";
    unsigned char ulen[4] = {req.at(0), req.at(1), req.at(2), req.at(3)};
    int usize = *(int*) ulen;
    for (int i=4; i < usize+4; i++) {
        key_str += req[i];
    }

    cout << "key_str: " << key_str << endl;

    int key = stoi(key_str);

    cout << "key: " << key << endl;

    pair<bool, vec> result = storage.kv_get(key);
    send_reliably(sd, result.second);
    return false;
}

bool server_cmd_kvd(int sd, const vec &req, Storage &storage) {
    std::string key_str = "";
    unsigned char ulen[4] = {req.at(0), req.at(1), req.at(2), req.at(3)};
    int usize = *(int*) ulen;
    for (int i=4; i < usize+4; i++) {
        key_str += req[i];
    }

    cout << "key_str: " << key_str << endl;

    int key = stoi(key_str);

    cout << "key: " << key << endl;

    vec result = storage.kv_delete(key);
    send_reliably(sd, result);
    return false;
}
