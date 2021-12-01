/**
 * @file config_t.h 
 */

#ifndef CONFIG_SERVER_DEF
#define CONFIG_SERVER_DEF

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

    /** Is the user requesting a usage message? */
    bool usage = false;

    /**
     * Datafile persisting the lazy list
     */
    std::string datafile = "";
};

#endif