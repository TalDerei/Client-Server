#include "net.h"
#include "config_t.h"
#include "server_parsing.h"
#include "server_storage.h"

using namespace std;

/**
 * Display a help message to explain how the command-line parameters for this program work
 */
void usage() {
    cout << "  -s [string] Name of the server (probably 'localhost')" << endl;
    cout << "  -p [int]    Port number of the server" << endl;
    cout << "  -f [int]    Persistant file name" << endl;
    cout << "  -h          Print help (this message)" << endl;
}

/**
 * Parse the command-line arguments, and use them to populate the provided args
 * object.
 *
 * @param argc The number of command-line arguments passed to the program
 * @param argv The list of command-line arguments
 * @param args The struct into which the parsed args should go
 */
void parseargs(int argc, char** argv, config_t& config) {
    long opt;
    while ((opt = getopt(argc, argv, "s:p:f:t:h")) != -1) {
        switch (opt) {
            case 's': config.server_name = std::string(optarg); break;
            case 'p': config.port = atoi(optarg); break;  
            case 'f': config.datafile = std::string(optarg); break;
            case 'h': usage(); break;
        }
    }
}

int main(int argc, char **argv) {
    /** Parse command-line arguments */
    config_t args;
    parseargs(argc, argv, args);
    if (args.usage) {
        usage();
        exit(0);
    }

    /** Set up server socket for listening */
    int serverSd = create_server_socket(args.port);

    /** If the data file exists, load the data into a Storage object. Otherwise, create an empty Storage object */
    Storage storage(args.datafile);

    /** Initialize lazy list data structure */
    storage.init_lazylist();

    /** Accept client connections */
    accept_client(serverSd, [&](int sd) {
        return serve_client(sd, storage); 
    });
}
