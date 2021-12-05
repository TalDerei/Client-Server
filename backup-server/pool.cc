#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <unistd.h>
#include <vector>

#include "pool.h"

using namespace std;

/// thread_pool::Internal is the class that stores all the members of a
/// thread_pool object. To avoid pulling too much into the .h file, we are using
/// the PIMPL pattern
/// (https://www.geeksforgeeks.org/pimpl-idiom-in-c-with-examples/)
struct thread_pool::Internal {
  /// construct the Internal object by setting the fields that are
  /// user-specified
  ///
  std::vector<std::thread> worker_threads;
  std::queue<int> jobs;
  std::condition_variable cv;
  std::mutex m;
  std::function<bool(int)> handler;
  std::function<void()> shutdown_handler;
  bool done = false;
  bool stop_all = false;

  /// @param handler The code to run whenever something arrives in the pool
  Internal(function<bool(int)> handler) {}
};

/// construct a thread pool by providing a size and the function to run on
/// each element that arrives in the queue
///
/// @param size    The number of threads in the pool
/// @param handler The code to run whenever something arrives in the pool
thread_pool::thread_pool(int size, function<bool(int)> handler)
    : fields(new Internal(handler)) {

    fields->handler = handler;
    auto working = [&]() {
        while (true) {
            std::unique_lock<std::mutex> lock(fields->m);
            fields->cv.wait(lock, [&] { return !fields->jobs.empty() || fields->stop_all; });
            if (fields->stop_all && fields->jobs.empty()) {
                return;
            }

            int job = std::move(fields->jobs.front());
            fields->jobs.pop();
            lock.unlock();

            fields->done = fields->handler(job);
            if (fields->done) {
                fields->shutdown_handler();
                fields->stop_all = true;
            }
            close(job);
        }
    };

    fields->worker_threads.reserve(size);
    for (int i=0; i < size; i++) {
        fields->worker_threads.emplace_back(working);
    }
}

/// destruct a thread pool
thread_pool::~thread_pool() = default;


/// Allow a user of the pool to provide some code to run when the pool decides
/// it needs to shut down.
///
/// @param func The code that should be run when the pool shuts down
void thread_pool::set_shutdown_handler(function<void()> func) {
    fields->shutdown_handler = func;
}

/// Allow a user of the pool to see if the pool has been shut down
bool thread_pool::check_active() {
    if (fields->stop_all) return false;
    return true;
}

/// Shutting down the pool can take some time.  await_shutdown() lets a user
/// of the pool wait until the threads are all done servicing clients.
void thread_pool::await_shutdown() {
    fields->stop_all = true;
    fields->cv.notify_all();
    for (auto& thread : fields->worker_threads) {
        thread.join();
    }
}

/// When a new connection arrives at the server, it calls this to pass the
/// connection to the pool for processing.
///
/// @param sd The socket descriptor for the new connection
void thread_pool::service_connection(int sd) {
    std::lock_guard<std::mutex> lock(fields->m);
    fields->jobs.push(sd);
    fields->cv.notify_one();
}
