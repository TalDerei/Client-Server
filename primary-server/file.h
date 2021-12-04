#pragma once

#include <string>

#include "vec.h"

/// Determine if a file exists.  Note that using this is not a good way to avoid
/// TOCTOU bugs, but it is acceptable for this class project.
/// @param filename The name of the file whose existence is being checked
/// @returns true if the file exists, false otherwise
bool file_exists(const std::string &filename);

/// Load a file and return its contents
/// @param filename The name of the file to open
/// @returns A vector with the file contents.  On error, returns an empty vector
vec load_entire_file(const std::string &filename);

/// Create or truncate a file and populate it with the provided data
/// @param filename The name of the file to create/truncate
/// @param data     The data to write
/// @param bytes    The number of bytes of data to write
/// @returns false on error, true if the file was written in full
bool write_file(const std::string &filename, const char *data, size_t bytes);
