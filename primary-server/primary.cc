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
    while ((opt = getopt(argc, argv, "s:p:h")) != -1) {
        switch (opt) {
            case 's': config.server_name = std::string(optarg); break;
            case 'p': config.port = atoi(optarg); break;  
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

    // If the data file exists, load the data into a Storage object.  Otherwise,
    // create an empty Storage object.
    string datafile = "saved_file.txt";
    int num_buckets = 10;
    Storage storage(datafile, num_buckets);

    accept_client(serverSd, [&](int sd) {return serve_client(sd, storage); });
    ///** Initialize a set of active sockets, and add the listening socket to it */
    //fd_set active_sds;
    //FD_ZERO(&active_sds);
    //FD_SET(serverSd, &active_sds);

    //while (true) {
    //    /** wait for input to come in on any of the active sockets */
    //    fd_set read_fd_set = active_sds;
    //    if (select(FD_SETSIZE, &read_fd_set, nullptr, nullptr, nullptr) < 0) {
    //        error_message_and_exit(0, errno, "Error calling select(): ");
    //    }

    //    /** Go throigh all sockets in active_sds that have pending input and process them.
    //     * FD_SETSIZE is the number of active connections
    //     */
    //    for (int i = 0; i < FD_SETSIZE; ++i) {
    //        if (FD_ISSET(i, &read_fd_set)) {
    //            /** If this socket is the server socket, it means we have a new incoming connection that we need to add to the set */
    //            if (i == serverSd) {
    //                sockaddr_in clientname;
    //                socklen_t size = sizeof(clientname);
    //                int connSd = accept(serverSd, (struct sockaddr *)&clientname, &size);
    //                if (connSd < 0) {
    //                    error_message_and_exit(0, errno, "Error accepting connection from client: ");
    //                }
    //                cout << "Connected to: " << inet_ntoa(clientname.sin_addr) << ":" << ntohs(clientname.sin_port) << endl;
    //                FD_SET(connSd, &active_sds);
    //            }
    //            /** Otherwise the socket is already in the set, which means that a client just sent data */
    //            else {
    //                if (!handle_client_input(i)) {
    //                    close(i);
    //                    FD_CLR(i, &active_sds);
    //                }
    //            }
    //        }
    //    }
    //}
}
