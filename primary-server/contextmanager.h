#pragma once

#include <functional>

/// ContextManager is a simple RAII object for ensuring that cleanup code runs
/// when a function exits.
class ContextManager {
  /// action is the cleanup code to run.  It's a std::function, so probably a
  /// lambda.
  std::function<void(void)> action;

  /// runit lets us disable the reclamation in cases where we change our mind
  bool runit = true;

public:
  /// Construct a ContextManager by providing a function (lambda) to run on
  /// destruct
  ContextManager(std::function<void(void)> a) : action(a) {}

  /// When the ContextManager goes out of scope, run its action
  ~ContextManager() {
    if (runit)
      action();
  }

  /// Cancel the action, to keep it from running when this object goes out of
  /// scope
  void cancel() { runit = false; }
};
