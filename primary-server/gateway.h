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
    string bname = "backup";

public:

/** Default constructor */
    Gateway() {}

/** Initialize gateway */
    void connect_to_backup_server() {
        cout << "Initializing Gateway!" << endl;
        sd = connect_to_server(bname, bport);
    }

/** send the message to backup server */
    vec send_message(const vec &msg) {
        send_reliably(sd, msg);
        vec res = reliable_get_to_eof(sd);
        return res;
    }
};
