/**
 * @file vec.h 
 */

#ifndef VEC_DEF
#define VEC_DEF

#pragma once

#include <string>
#include <vector>

/** vec -- vector of unsigned chars */
typedef std::vector<unsigned char> vec;

/**
 * @brief Create a vector from a string, by copying the string contents into the vector
 * 
 * @param s The string to copy
 * @return  vector of the string 
 */
vec vec_from_string(const std::string &s);

/**
 * @brief Append the contents of a string to the end of a vector
 * 
 * @param v The vector to which we will append
 * @param s The string to append to the end of the vector
 */
void vec_append(vec &v, const std::string &s);

/**
 * @brief Append the contents of one vector to another vector
 * 
 * @param to   The vector into which we should copy
 * @param from The vector from which to copy
 */
void vec_append(vec &to, const vec &from);

/**
 * @brief Append a binary representation of an integer to the end of a vector
 * 
 * @param v The vector into which we should append
 * @param i The integer to put into the vector
 */
void vec_append(vec &v, int i);

#endif
