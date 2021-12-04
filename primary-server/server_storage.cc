/**
 * @file server_storage.cc 
 */

#include <iostream>
#include <unordered_map>
#include <utility>

#include "protocol.h"
#include "vec.h"
#include "server_storage.h"
#include "../lazy-list/lazyList.h"
#include "file.h"

using namespace std;

/**
 * @brief Storage::Internal is the private struct that holds all of the fields of the
 * Storage object. Organizing the fields as an Internal is part of the PIMPL pattern.
 */
struct Storage::Internal {
    /** lazylist object */
    lazyList<string, string> lazylist;

    /** Filename is the name of the file from which the Storage object was loaded,
     * and to which we persist the Storage object every time it changes */
    string filename = "";

    /* file pointer to keep file open */
    FILE* fp = nullptr;

    /* API commands in file as an unique 8-byte code */
    inline static const string KVINSERT = "KVINSERT";
    inline static const string KVDELETE = "KVDELETE";

    /**
     * @brief Construct the Storage::Internal object by setting the filename 
     * 
     * @param fname The name of the file that should be used to load/store the data
     */
    Internal(string fname)
      : lazylist(), filename(fname) {}
};



/**
 * @brief Construct a new Storage::Storage object
 * 
 * @param fname The name of the file that should be used to load/store the data
 */
Storage::Storage(const string &fname) 
    : fields(new Internal(fname)) {}

/** 
 *  Destructor for the storage object
 */
Storage::~Storage() = default;


/** Initialize lazy list data structure */
void Storage::init_lazylist() {
    fields->lazylist.initialize();
}

/**
 * @brief Populate the Storage object by loading this.filename. 
 * @return false if any error is encountered in the file, and true 
 *         otherwise.  Note that a non-existent file is not an error.
 */
bool Storage::load() {
    if (fields->filename.empty()) return false;
    bool has_log = false;

    /* Read a data file if it exists */
    if (file_exists(fields->filename)) {
        has_log = true;
        vec disk = load_entire_file(fields->filename);
        unsigned int total = disk.size();
        unsigned int n = 0;
        cout << "Reading datafile..." << endl;
        while (true) {
            std::string prefix(disk.begin()+n, disk.begin()+n+8);
            cout << "prefix: " << prefix << endl;

            /* Read INSERT command */
            if (prefix == fields->KVINSERT) {
                /* prefix length */
                n += 8;
                /* key length */
                unsigned char kstr[4] = {disk.at(n), disk.at(n+1), disk.at(n+2), disk.at(n+3)};
                int key = *(int*) kstr;
                n += 4;
                /* value length */
                unsigned char vstr[4] = {disk.at(n), disk.at(n+1), disk.at(n+2), disk.at(n+3)};
                int val = *(int*) vstr;
                n += 4;
                /* execute a command */
                fields->lazylist.parse_insert(key, val);
            }

            /* Read DELETE command */
            else if (prefix == fields->KVDELETE) {
                /* prefix length */
                n += 8;
                /* key length */
                unsigned char kstr[4] = {disk.at(n), disk.at(n+1), disk.at(n+2), disk.at(n+3)};
                int key = *(int*) kstr;
                n += 8;
                /* execute a command */
                fields->lazylist.parse_delete(key);
            }

            else {
                cout << "something wrong!" << endl;
                n += 16;
            }

            /* break condition */
            cout << "n: " << n << endl;
            cout << "total: " << total << endl;
            if (n >= total) break;
        }
    }

    if (has_log) {
        fields->fp = fopen(fields->filename.c_str(), "a");
    } else {
        fields->fp = fopen(fields->filename.c_str(), "w");
    }
    cout << "Open initial backup file successfully!" << endl;
    return true;
}

/**
 * @brief Write the entire Storage object to the file specified by this.filename.
 * To ensure durability, Storage must be persisted in two steps.  First, it
 * must be written to a temporary file (this.filename.tmp).  Then the
 * temporary file can be renamed to replace the older version of the Storage object
 */
void Storage::persist() {
    FILE *f = fopen(fields->filename.c_str(), "w+");
    fclose(f);
}

void Storage::persist(string prefix, const int &key, const int &val) {
    fputs(prefix.c_str(), fields->fp);
    fwrite (&key, sizeof(int), 1, fields->fp);
    fwrite (&val, sizeof(int), 1, fields->fp);
    //fwrite("\n", sizeof(char), 1, fields->fp);
    fflush(fields->fp);
    cout << "persisted data!" << endl;
}

/**
 * @brief Shut down the storage when the server stops.
 */
void Storage::shutdown() {
    fields->lazylist.set_delete_l();
    exit(0);
}

/**
 * @brief  Create a new key/value mapping in the lazy list
 * 
 * @param key The key whose mapping is being created
 * @param val  The value to copy into the lazy list
 * @return A vec with the result message 
 */
vec Storage::kv_insert(const int &key, const int &val) {
    val_t key_ptr = (val_t)key;
    val_t val_ptr = (val_t)val;

    cout << "kv_insert function!" << endl;
    if (fields->lazylist.parse_insert(key_ptr, val_ptr)) {
        cout << "persist!" << endl;
        persist(fields->KVINSERT, key, val);
        cout << "wrote file..." << endl;
        return vec_from_string(RES_OK);
    }
    return vec_from_string(RES_ERR_KEY);
};

/**
 * @brief Get a copy of the value to which a key is mapped
 * 
 * @param key The key whose value is being fetched
 * @return pair<bool, vec> 
 */
pair<bool, vec> Storage::kv_get(const int &key) {
    val_t key_ptr = (val_t)key;

    pair<int, int> success = fields->lazylist.parse_find(key_ptr);

    if (success.second) {
        return {true, vec_from_string(to_string(success.first))};
    }

    return {false, vec_from_string(RES_ERR_KEY)};
};

/**
 * @brief Delete a key/value mapping
 * 
 * @param key The key whose value is being deleted
 * @return vec 
 */
pair<bool, vec> Storage::kv_delete(const int &key) {
    val_t key_ptr = (val_t)key;
    
    if (fields->lazylist.parse_delete(key_ptr)) {
        persist(fields->KVDELETE, key, 0);
        return {true, vec_from_string(RES_OK)};
    }

    return {false, vec_from_string(RES_ERR_KEY)};
};
