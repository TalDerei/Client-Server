/**
 * @file protocol.h 
 */

#ifndef PROTOCOL_DEF
#define PROTOCOL_DEF

#pragma once

#include <string>

using namespace std;

const int LEN_RKBLOCK = 7;

/** API Commands */
const string REQ_KVI = "KVI";
const string REQ_KVG = "KVG";
const string REQ_KVD = "KVD";
const string REQ_ROR = "ROR";

/** Response code to indicate that the command was successful */
const string RES_OK = "TRUE";

/* Response code to indicate that there was an error when searching for the given key */
const string RES_ERR_KEY = "FALSE";
const string RES_ERR_INVALID = "INVALID";

#endif
