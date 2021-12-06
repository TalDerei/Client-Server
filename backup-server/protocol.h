/**
 * @file protocol.h 
 */

#pragma once

#include <string>

using namespace std;

const int LEN_RKBLOCK = 7;

/** API Commands */
const string REQ_KVI = "KVI";
const string REQ_KVG = "KVG";
const string REQ_KVD = "KVD";
const string REQ_PVI = "PVI";
const string REQ_PVD = "PVD";
const string REQ_DOR = "DOR";

/** Response code to indicate that the command was successful */
const string RES_OK = "OK";

/* Response code to indicate that there was an error when searching for the given key */
const string RES_ERR_KEY = "ERR_KEY";
const string RES_ERR_INVALID = "INVALID";
