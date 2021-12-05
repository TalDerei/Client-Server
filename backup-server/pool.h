#pragma once

#include <functional>
#include <memory>

/// thread_pool encapsulates a pool of threads that are all waiting for data to
/// appear in a queue.  Whenever data arrives in the queue, a thread will pull
/// the data off and process it, using the handler function provided at
/// construction time.
class thread_pool {
  /// Internal is the class that stores all the members of a thread_pool object.
  /// To avoid pulling too much into the .h file, we are using the PIMPL pattern
  /// (https://www.geeksforgeeks.org/pimpl-idiom-in-c-with-examples/)
  struct Internal;

  /// A reference to the internal fields of the thread_pool object
  std::unique_ptr<Internal> fields;

public:
  /// construct a thread pool by providing a size and the function to run on
  /// each element that arrives in the queue
  ///
  /// @param size    The number of threads in the pool
  /// @param handler The code to run whenever something arrives in the pool
  thread_pool(int size, std::function<bool(int)> handler);

  /// destruct a thread pool
  ~thread_pool();

  /// Allow a user of the pool to provide some code to run when the pool decides
  /// it needs to shut down.
  ///
  /// @param func The code that should be run when the pool shuts down
  void set_shutdown_handler(std::function<void()> func);

  /// Allow a user of the pool to see if the pool has been shut down
  bool check_active();

  /// Shutting down the pool can take some time.  await_shutdown() lets a user
  /// of the pool wait until the threads are all done servicing clients.
  void await_shutdown();

  /// When a new connection arrives at the server, it calls this to pass the
  /// connection to the pool for processing.
  ///
  /// @param sd The socket descriptor for the new connection
  void service_connection(int sd);
};