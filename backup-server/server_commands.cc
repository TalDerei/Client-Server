#include <iostream>
#include <string>

#include "server_commands.h"
#include "server_storage.h"
#include "net.h"
#include <mutex>

using namespace std;

/** 
 * Backup server loading log file from primary seerver (after primary server crashes/restarts) 
 */
bool server_cmd_dor(int sd, const vec &disk, Storage &storage) {
    mutex m;
    m.lock();
    storage.load(disk);
    m.unlock();
    return false;
}

/* update API call from primary server */
bool server_cmd_pvi(int sd, const vec &req, Storage &storage) {
    /** same as server_cmd_kvi */
    std::string key_str = "";
    std::string val_str = "";
    unsigned char ulen[4] = {req.at(0), req.at(1), req.at(2), req.at(3)};
    int usize = *(int*) ulen;
    for (int i=4; i < usize+4; i++) {
        key_str += req[i];
    }
    unsigned char plen[4] = {req.at(usize+4), req.at(usize+5), req.at(usize+6), req.at(usize+7)};
    int psize = *(int*) plen;
    for (int i = usize+8; i < usize+8+psize; i++) {
        val_str += req[i];
    }
    int key = stoi(key_str);
    int val = stoi(val_str);
    /***************************/
    std::cout << "PVI!" << std::endl;
    std::cout << "key: " << key << std::endl;
    std::cout << "val: " << val << std::endl;

    /** Call insert() on storage object represented by hash table */
    bool from_primer = true;
    vec status = storage.kv_insert(key, val, from_primer);

    /** Send response to client */
    send_reliably(sd, status);
    return false;
}

bool server_cmd_pvd(int sd, const vec &req, Storage &storage) {
    /** same as server_cmd_kvd */
    std::string key_str = "";
    unsigned char ulen[4] = {req.at(0), req.at(1), req.at(2), req.at(3)};
    int usize = *(int*) ulen;
    for (int i=4; i < usize+4; i++) {
        key_str += req[i];
    }
    int key = stoi(key_str);
    /***************************/

    /** Call remove() on storage object represented by hash table */
    bool from_primer = true;
    std::pair<bool, vec> result = storage.kv_delete(key, from_primer);

    /** Send response to client */
    send_reliably(sd, result.second);
    return false;
}


/**
 * @brief Server command servering the Insert API call 
 * 
 * @param sd      The socket onto which the result should be written
 * @param req     The unencrypted contents of the request
 * @param storage The Storage object
 * @return        false, to indicate that the server shouldn't stop 
 */
bool server_cmd_kvi(int sd, const vec &req, Storage &storage) {
    /** Parse 'req' vector into strings */
    std::string key_str = "";
    std::string val_str = "";
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
    for (int i = usize+8; i < usize+8+psize; i++) {
        val_str += req[i];
    }

    /** Store integer representation of key and value */
    int key = stoi(key_str);
    int val = stoi(val_str);
    cout << "Key: " << key << endl;
    cout << "Value: " << val << endl;

    /** Call insert() on storage object represented by hash table */
    bool from_primer = false;
    vec status = storage.kv_insert(key, val, from_primer);

    /** Send response to client */
    cout << "Sending response to client..." << endl;
    send_reliably(sd, status);
    cout << "Sent!" << endl;
    return false;
}

/**
 * @brief Server command servering the Remove API call 
 * 
 * @param sd      The socket onto which the result should be written
 * @param req     The unencrypted contents of the request
 * @param storage The Storage object
 * @return        false, to indicate that the server shouldn't stop 
 */
bool server_cmd_kvg(int sd, const vec &req, Storage &storage) {
    /** Parse 'req' vector into strings */
    std::string key_str = "";
    unsigned char ulen[4] = {req.at(0), req.at(1), req.at(2), req.at(3)};
    int usize = *(int*) ulen;
    for (int i=4; i < usize+4; i++) {
        key_str += req[i];
    }

    /** Store integer representation of key */
    int key = stoi(key_str);
    cout << "Key: " << key << endl;

    /** Call contains() on storage object represented by hash table */
    pair<bool, vec> result = storage.kv_get(key);

    /** Send response to client */
    send_reliably(sd, result.second);
    return false;
}

/**
 * @brief Server command servering the Contains API call 
 * 
 * @param sd      The socket onto which the result should be written
 * @param req     The unencrypted contents of the request
 * @param storage The Storage object
 * @return        false, to indicate that the server shouldn't stop 
 */
bool server_cmd_kvd(int sd, const vec &req, Storage &storage) {
    /** Parse 'req' vector into strings */
    std::string key_str = "";
    unsigned char ulen[4] = {req.at(0), req.at(1), req.at(2), req.at(3)};
    int usize = *(int*) ulen;
    for (int i=4; i < usize+4; i++) {
        key_str += req[i];
    }

    /** Store integer representation of key */
    int key = stoi(key_str);
    cout << "key: " << key << endl;

    /** Call remove() on storage object represented by hash table */
    bool from_primer = false;
    std::pair<bool, vec> result = storage.kv_delete(key, from_primer);

    /** Send response to client */
    send_reliably(sd, result.second);
    return false;
}
