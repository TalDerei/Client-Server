#include <iostream>
#include <string>

#include "net.h"
#include "protocol.h"
#include "vec.h"

#include "client_commands.h"

using namespace std;

vec client_send_cmd(int sd, const string &cmd, const vec &msg) {
    vec req;
    vec_append(req, cmd);
    vec_append(req, msg.size());
    vec_append(req, msg);
    cout << "test!!! " << endl;
    cout << "size!!! " << req.size() << endl;
    for (unsigned int i=0; i < req.size(); i++) {
        cout << "reqq: " << req.at(i) << endl;
    }
    send_reliably(sd, req);
    cout << "Sent!" << endl;
    vec res;
    cout << "will recieve!" << endl;
    res = reliable_get_to_eof(sd);
    cout << "recieved!" << endl;
    return res;
}

void client_insert(int sd, const string &key, const string &val) {
    vec msg;
    cout << "Client Key: " << key << endl;
    cout << "Client Val: " << val << endl;
    vec_append(msg, key.length());
    vec_append(msg, key);
    vec_append(msg, val.length());
    vec_append(msg, val);
    auto res = client_send_cmd(sd, REQ_KVI, msg);
    for (unsigned int i=0; i < res.size(); i++) {
        cout << "res[i]: " << res.at(i) << endl;
    }
    //check_err_response(res);
}

void client_remove(int sd, const string &key, const string &val) {
    vec msg;
    vec_append(msg, key.length());
    vec_append(msg, key);
    auto res = client_send_cmd(sd, REQ_KVD, msg);
    for (unsigned int i=0; i < res.size(); i++) {
        cout << "res[i]: " << res.at(i) << endl;
    }
    //check_err_response(res);
}

void client_contains(int sd, const string &key, const string &val) {
    vec msg;
    vec_append(msg, key.length());
    vec_append(msg, key);
    auto res = client_send_cmd(sd, REQ_KVG, msg);
    for (unsigned int i=0; i < res.size(); i++) {
        cout << "res[i]: " << res.at(i) << endl;
    }
    //check_err_response(res);
}
