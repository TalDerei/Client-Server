/**
 * @file server_storage.h
 */


#ifndef SERVER_STORAGE_DEF
#define SERVER_STORAGE_DEF

#pragma once

#include <memory>
#include <string>
#include <utility>
#include "vec.h"
#include "server_storage.h"


/**
 * @brief Storage is the main data type managed by the server.
 * The public interface of Storage provides functions that correspond 1:1 with
 * the data requests that a client can make.  In that manner, the server
 * command handlers need only parse a request, send its parts to the Storage
 * object, and then format and return the result.
 * 
 * Storage is a persistent object.  For the time being, persistence is achieved
 * by writing the entire object to disk in response to SAV messages.  We use a
 * relatively simple binary wire format to write every K/V paur to disk
 */

class Storage {
  /** Typedef for integer pointer */
  typedef intptr_t val_t;

  /**
   * @brief Internal is the class that stores all the members of a Storage object. 
   * To avoid pulling too much into the .h file, we are using the PIMPL pattern
   */
  struct Internal;

  /** A reference to the internal fields of the Storage object */
  std::unique_ptr<Internal> fields;

public:
    /** Construct an empty object and specify the file from which it should be
     * loaded.  To avoid exceptions and errors in the constructor, the act of
     * loading data is separate from construction.
     */
    Storage(const std::string &fname);

    /** Destructor for the storage object. */
    ~Storage();

    /** Initialize lazy list data structure */
    void init_lazylist();

    /* is it backup server? */
    bool is_backup();


    /** Request log from primary seerver */
    bool do_request();

    /**
     * @brief Populate the Storage object by loading this.filename. 
     * @return false if any error is encountered in the file, and true 
     *         otherwise.  Note that a non-existent file is not an error.
     */
    bool load(const vec &disk);

    /**
     * @brief Write the entire Storage object to the file specified by this.filename.
     * To ensure durability, Storage must be persisted in two steps.  First, it
     * must be written to a temporary file (this.filename.tmp).  Then the
     * temporary file can be renamed to replace the older version of the Storage object
     */
    void persist(std::string cmd, const int &key, const int &val);

    /**
     * @brief Shut down the storage when the server stops.
     */
    void shutdown();

    /**
     * @brief  Create a new key/value mapping in the lazy list
     * 
     * @param key The key whose mapping is being created
     * @param val  The value to copy into the lazy list
     * @return A vec with the result message 
     */
    vec kv_insert(const int &key, const int &val, bool from_primer);

    /**
     * @brief Get a copy of the value to which a key is mapped
     * 
     * @param key The key whose value is being fetched
     * @return pair<bool, vec> 
     */
    std::pair<bool, vec> kv_get(const int &key);

    /**
     * @brief Delete a key/value mapping
     * 
     * @param key The key whose value is being deleted
     * @return vec 
     */
    std::pair<bool, vec> kv_delete(const int &key, bool from_primer);
};

#endif
