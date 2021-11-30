/**
 * @file server_storage.cc 
 */

#include <iostream>
#include <unordered_map>
#include <utility>

#include "hashtable.h"
#include "protocol.h"
#include "vec.h"

#include "server_storage.h"

using namespace std;

/**
 * @brief Storage::Internal is the private struct that holds all of the fields of the
 * Storage object.  Organizing the fields as an Internal is part of the PIMPL pattern.
 */
struct Storage::Internal {
  /// The map of key/value pairs
  // TODO: replace it into MUTEX-lazy-list
  ConcurrentHashTable<int, int> kv_store;

  /// filename is the name of the file from which the Storage object was loaded,
  /// and to which we persist the Storage object every time it changes
  string filename = "";

  /// Construct the Storage::Internal object by setting the filename and bucket
  /// count
  ///
  /// @param fname       The name of the file that should be used to load/store
  ///                    the data
  /// @param num_buckets The number of buckets for the hash
  Internal(string fname, size_t num_buckets) : kv_store(num_buckets), filename(fname) {}
};

/// Construct an empty object and specify the file from which it should be
/// loaded.  To avoid exceptions and errors in the constructor, the act of
/// loading data is separate from construction.
///
/// @param fname       The name of the file that should be used to load/store
///                    the data
/// @param num_buckets The number of buckets for the hash
Storage::Storage(const string &fname, size_t num_buckets) : fields(new Internal(fname, num_buckets)) {}

/// Destructor for the storage object.
///
/// NB: The compiler doesn't know that it can create the default destructor in
///     the .h file, because PIMPL prevents it from knowing the size of
///     Storage::Internal.  Now that we have reified Storage::Internal, the
///     compiler can make a destructor for us.
Storage::~Storage() = default;

/// Populate the Storage object by loading this.filename.  Note that load()
/// begins by clearing the maps, so that when the call is complete,
/// exactly and only the contents of the file are in the Storage object.
///
/// @returns false if any error is encountered in the file, and true
///          otherwise.  Note that a non-existent file is not an error.
bool Storage::load() { return true; }



/// Write the entire Storage object to the file specified by this.filename.
/// To ensure durability, Storage must be persisted in two steps.  First, it
/// must be written to a temporary file (this.filename.tmp).  Then the
/// temporary file can be renamed to replace the older version of the Storage
/// object.
void Storage::persist() {}

/// Shut down the storage when the server stops.
///
/// NB: this is only called when all threads have stopped accessing the
///     Storage object.  As a result, there's nothing left to do, so it's a
///     no-op.
void Storage::shutdown() {}

/// Create a new key/value mapping in the table
///
/// @param key       The key whose mapping is being created
/// @param val       The value to copy into the map
///
/// @returns A vec with the result message
vec Storage::kv_insert(const int &key, const int &val) {
    if (fields->kv_store.insert(key, val)) return vec_from_string(RES_OK);
    return vec_from_string(RES_ERR_KEY);
};

/// Get a copy of the value to which a key is mapped
///
/// @param key       The key whose value is being fetched
///
/// @returns A pair with a bool to indicate error, and a vector indicating the
///          data (possibly an error message) that is the result of the
///          attempt.
pair<bool, vec> Storage::kv_get(const int &key) {
    int content;
    bool success = fields->kv_store.do_with_readonly(key, [&content](int V) {content = V;});
    string content_str = to_string(content);
    if (success) return {false, vec_from_string(content_str)};
    return {true, vec_from_string(RES_ERR_KEY)};
};

/// Delete a key/value mapping
///
/// @param key       The key whose value is being deleted
///
/// @returns A vec with the result message
vec Storage::kv_delete(const int &key) {
    fields->kv_store.remove(key);
    return vec_from_string(RES_OK);
};
