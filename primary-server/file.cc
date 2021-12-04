#include <iostream>
#include <sys/stat.h>

#include "contextmanager.h"
#include "vec.h"

using namespace std;

/// Determine if a file exists.  Note that using this is not a good way to avoid
/// TOCTOU bugs, but it is acceptable for this class project.
/// @param filename The name of the file whose existence is being checked
/// @returns true if the file exists, false otherwise
bool file_exists(const string &filename) {
  struct stat stat_buf;
  return (stat(filename.c_str(), &stat_buf) == 0);
}

/// Load a file and return its contents
/// @warning This function has a TOCTOU bug.
/// @param filename The name of the file to open
/// @returns A vector with the file contents.  On error, returns an empty vector
vec load_entire_file(const string &filename) {
  // make sure file exists, then open it.  stat also lets us get its size later
  struct stat stat_buf;
  if (stat(filename.c_str(), &stat_buf) != 0) {
    cerr << "File " << filename << " not found\n";
    return {};
  }
  FILE *f = fopen(filename.c_str(), "rb");
  if (!f) {
    cerr << "File " << filename << " not found\n";
    return {};
  }
  ContextManager closer([&]() { fclose(f); }); // close file when we return

  // NB: Reading one extra byte should mean we get an EOF and only size bytes.
  //     Also, since we know it's a true file, we don't need to worry about
  //     short counts and EINTR.
  vec res(stat_buf.st_size); // reserve space in vec, based on stat() from
                             // before open() (TOCTOU risk)
  unsigned recd = fread(res.data(), sizeof(char), stat_buf.st_size + 1, f);
  if (recd != res.size() || !feof(f)) {
    cerr << "Wrong # bytes reading " << filename << endl;
    return {};
  }
  return res;
}

/// Create or truncate a file and populate it with the provided data
/// @param filename The name of the file to create/truncate
/// @param data     The data to write
/// @param bytes    The number of bytes of data to write
/// @returns false on error, true if the file was written in full
bool write_file(const string &filename, const char *data, size_t bytes) {
  FILE *f = fopen(filename.c_str(), "wb");
  if (f == nullptr) {
    cerr << "Unable to open '" << filename << "' for writing\n";
    return false;
  }
  ContextManager closer([&]() { fclose(f); }); // close file when we return

  // NB: since we know it's a true file, we don't need to worry about short
  //     counts and EINTR.
  if (fwrite(data, sizeof(char), bytes, f) != bytes) {
    cerr << "Incorrect number of bytes written to " << filename << endl;
    return false;
  }
  return true;
}
