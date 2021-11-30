#include "vec.h"

using namespace std;

/// Create a vector from a string, by copying the string contents into the
/// vector
///
/// @param s The string to copy
///
/// @returns A vector of the string
vec vec_from_string(const string &s) { return vec(s.begin(), s.end()); }

/// Append the contents of a string to the end of a vector
///
/// @param v The vector to which we will append
/// @param s The string to append to the end of the vector
void vec_append(vec &v, const string &s) {
  v.insert(v.end(), s.begin(), s.end());
}

/// Append the contents of one vector to another vector
///
/// @param to   The vector into which we should copy
/// @param from The vector from which to copy
void vec_append(vec &to, const vec &from) {
  to.insert(to.end(), from.begin(), from.end());
}

/// Append a binary representation of an integer to the end of a vector
///
/// @param v The vector into which we should append
/// @param i The integer to put into the vector
void vec_append(vec &v, int i) {
  v.insert(v.end(), (char *)&i, ((char *)&i) + sizeof(int));
}
