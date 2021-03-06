/**
 * @file config_t.h 
 */

#ifndef CONFIG_T_DEF
#define CONFIG_T_DEF

#include <iostream>
#include <string>

/**
 * Configuration information for command-line parameters of the program
 */
struct config_t {
  /** The name of the server to which the parent program will connect */
  std::string server_name = "";

  /** The port on which the program will connect to the above server */
  size_t port = 0;

  /** The time to wait between sending the first and second message */
  int wait = 0;

  /** Is the user requesting a usage message? */
  bool usage = false;

  /** API command */
  std::string command = "";

  /** Key */
  std::string key = "";

  /** Value */
  std::string value = "";
};

#endif