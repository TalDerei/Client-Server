/**
 * @file client.cc
 * 
 * Client is half of a client/server pair that demonstrates how a server
 * can use select() so that a single-threaded server can manage multiple client
 * communications.  The client connects to the server, sends a message, waits,
 * and sends another message. By waiting, there is an opportunity for another
 * client to connect in the meantime.
 */

#include "config_t.h"
#include "net.h"
#include "protocols.h"
#include "client_commands.h"

using namespace std;

/**
 * Display a help message to explain how the command-line parameters for this
 * program work
 */
void usage() {
    cout << "  -s [string]  Name of the server (probably 'localhost')" << endl;
    cout << "  -p [int]     Port number of the server" << endl;
    cout << "  -w [int]     Time to wait between messages" << endl;
    cout << "  -C [string]  API Command" << endl;
    cout << "                   KVI (insert)" << endl;
    cout << "                   KVG (contains)" << endl;
    cout << "                   KVD (remove)" << endl;
    cout << "  -k [string]   Key" << endl;
    cout << "  -v [string]   Value" << endl;
    cout << "  -h            Print help (this message)" << endl;
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
    while ((opt = getopt(argc, argv, "s:p:w:C:k:v:h")) != -1) {
        switch (opt) {
            case 's': config.server_name = std::string(optarg); break;
            case 'p': config.port = atoi(optarg); break;  
            case 'w': config.wait = atoi(optarg); break;
            case 'h': usage(); break;
            case 'C': config.command = std::string(optarg); break;
            case 'k': config.key = std::string(optarg); break;
            case 'v': config.value = std::string(optarg); break;
        }
    }
}

/**
 * Main function
 */
int main(int argc, char **argv) {
    /** Parse command-line arguments */
    config_t args;
    parseargs(argc, argv, args);
    if (args.usage) {
        usage();
        exit(0);
    }

    /** Identify client by its unique PID */
    cout << "Starting client is: " << getpid() << endl;

    /** Client socket for communication */
    int sd = connect_to_server(args.server_name, args.port);

    /** Send message to server */
    if (args.command.length() > 0) {
        vector<string> cmds = {REQ_KVI, REQ_KVD, REQ_KVG};
        decltype(client_insert) *funcs[] = {client_insert, client_remove, client_contains};
        for (size_t i = 0; i < cmds.size(); ++i) {
            if (args.command == cmds[i]) {
                funcs[i](sd, args.key, args.value);
            }
        }
    } 
    else {
        usage();
    }

    cout << "Closing client: " << getpid() << endl;

    return 0;
}