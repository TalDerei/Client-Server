#pragma once

#include <memory>
#include <string>
#include <utility>

#include "vec.h"

/// Storage is the main data type managed by the server.
/// The public interface of Storage provides functions that correspond 1:1 with
/// the data requests that a client can make.  In that manner, the server
/// command handlers need only parse a request, send its parts to the Storage
/// object, and then format and return the result.
///
/// Storage is a persistent object.  For the time being, persistence is achieved
/// by writing the entire object to disk in response to SAV messages.  We use a
/// relatively simple binary wire format to write every Auth table entry and
/// every K/V pair to disk:

class Storage {
  /// Internal is the class that stores all the members of a Storage object.  To
  /// avoid pulling too much into the .h file, we are using the PIMPL pattern
  /// (https://www.geeksforgeeks.org/pimpl-idiom-in-c-with-examples/)
  struct Internal;

  /// A reference to the internal fields of the Storage object
  std::unique_ptr<Internal> fields;

public:
  /// Construct an empty object and specify the file from which it should be
  /// loaded.  To avoid exceptions and errors in the constructor, the act of
  /// loading data is separate from construction.
  Storage(const std::string &fname, size_t num_buckets);

  /// Destructor for the storage object.
  ~Storage();

  /// Populate the Storage object by loading this.filename.  Note that load()
  /// begins by clearing the maps, so that when the call is complete, exactly
  /// and only the contents of the file are in the Storage object.
  ///
  /// @returns false if any error is encountered in the file, and true
  ///          otherwise.  Note that a non-existent file is not an error.
  bool load();


  /// Write the entire Storage object to the file specified by this.filename.
  /// To ensure durability, Storage must be persisted in two steps.  First, it
  /// must be written to a temporary file (this.filename.tmp).  Then the
  /// temporary file can be renamed to replace the older version of the Storage
  /// object.
  void persist();

  /// Shut down the storage when the server stops.
  ///
  /// NB: this is only called when all threads have stopped accessing the
  ///     Storage object.  As a result, there's nothing left to do, so it's a
  ///     no-op.
  void shutdown();

  /// Create a new key/value mapping in the table
  ///
  /// @param user_name The name of the user who made the request
  /// @param pass      The password for the user, used to authenticate
  /// @param key       The key whose mapping is being created
  /// @param val       The value to copy into the map
  ///
  /// @returns A vec with the result message
  vec kv_insert(const int &key, const int &val);

  /// Get a copy of the value to which a key is mapped
  ///
  /// @param user_name The name of the user who made the request
  /// @param pass      The password for the user, used to authenticate
  /// @param key       The key whose value is being fetched
  ///
  /// @returns A pair with a bool to indicate error, and a vector indicating the
  ///          data (possibly an error message) that is the result of the
  ///          attempt.
  std::pair<bool, vec> kv_get(const int &key);

  /// Delete a key/value mapping
  ///
  /// @param user_name The name of the user who made the request
  /// @param pass      The password for the user, used to authenticate
  /// @param key       The key whose value is being deleted
  ///
  /// @returns A vec with the result message
  vec kv_delete(const int &key);
};