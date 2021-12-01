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
    cout << "Need to implement!" << endl;
}

/**
 * @brief Write the entire Storage object to the file specified by this.filename.
 * To ensure durability, Storage must be persisted in two steps.  First, it
 * must be written to a temporary file (this.filename.tmp).  Then the
 * temporary file can be renamed to replace the older version of the Storage object
 */
void Storage::persist(const int &key, const int &val) {
    FILE *f = fopen(fields->filename.c_str(), "w");
    fwrite (&key, sizeof(int), 1, f);
    fwrite (&val, sizeof(int), 1, f);
    fwrite("\n", sizeof(char), 1, f);
    fclose (f);
    cout << "Persisted data!" << endl;
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

    if (fields->lazylist.parse_insert(key_ptr, val_ptr)) {
        persist(key, val);
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
        persist(key, 0);
        return {true, vec_from_string(RES_OK)};
    }

    return {false, vec_from_string(RES_ERR_KEY)};
};
