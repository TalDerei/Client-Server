/**
 * @file gateway.h
 * 
 *
 */
#include "net.h"

using namespace std;

class Gateway {
    int sd;
    int bport = 8888;
    string bname = "localhost";

public:

/** Default constructor */
    Gateway() {}

/** send the message to backup server */
    vec communicate(const vec &req) {
        cout << "test gateway!" << endl;
        cout << "size: " << req.size() << endl;
        for (unsigned int i=0; i < req.size(); i++) {
            cout << "req[i]:: " << req.at(i) << endl;
        }
        sd = connect_to_server(bname, bport);
        send_reliably(sd, req);
        vec res = reliable_get_to_eof(sd);
        return res;
    }

    vec send_message(const string &cmd, const int &key) {
        std::cout << "send_message(PVD?)" << cmd << std::endl;
        vec req;
        /* set up key */
        string k = to_string(key);
        string v = to_string(0);

        /* set up message */
        vec msg = set_msg(k, v);

        /* set up request */
        vec_append(req, cmd);
        vec_append(req, msg.size());
        vec_append(req, msg);

        /* send via socket */
        cout << "comm!" << endl;
        vec res = communicate(req);
        return res;
    }

    vec send_message(const string &cmd, const int &key, const int &val) {
        std::cout << "send_message(PVI?)" << cmd << std::endl;
        vec req;
        /* set up key and value */
        string k = to_string(key);
        string v = to_string(val);

        /* set up message */
        vec msg = set_msg(k, v);

        /* set up request */
        vec_append(req, cmd);
        vec_append(req, msg.size());
        vec_append(req, msg);

        /* send via socket */
        cout << "comm!!!!" << endl;
        vec res = communicate(req);
        return res;
    }

    vec set_msg(const string &key, const string &val) {
        vec msg;
        vec_append(msg, key.length());
        vec_append(msg, key);
        vec_append(msg, val.length());
        vec_append(msg, val);
        return msg;
    }
};
