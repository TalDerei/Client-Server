
#include "net.h"

/**
 * Print an error message that combines some provded text (prefix) with the
 * standard unix error message that accompanies errno, and then exit the
 * program.  This routine makes it easier to see the logic in our program while
 * still correctly handling errors.
 *
 * @param code   The exit code to return from the program
 * @param err    The error code that was generated by the program
 * @param prefix The text to display before the error message
 */
void error_message_and_exit(std::size_t code, std::size_t err, const char *prefix) {
  char buf[1024];
  cout << strerror_r(err, buf, sizeof(buf)) << endl;
  exit(code);
}

/**
 * Connect to a server so that we can have bidirectional communication on the
 * socket (represented by a file descriptor) that this function returns
 *
 * @param hostname The name of the server (ip or DNS) to connect to
 * @param port     The server's port that we shoul duse
 */
int connect_to_server(std::string hostname, std::size_t port) {
  /** Figure out IP addresses and put it in a sockaddr_in */
  struct hostent *host = gethostbyname(hostname.c_str());
  
  if (host == nullptr) {
    cout << "connect_to_server():DNS error: " << hstrerror(h_errno) << endl;
    exit(0);
  }

  sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)*host->h_addr_list));
  addr.sin_port = htons(port);
  
  /** Create socket and try to connect to it */
  int sd = socket(AF_INET, SOCK_STREAM, 0);
  if (sd < 0) {
    error_message_and_exit(0, errno, "Error making client socket: ");
  }
  if (connect(sd, (sockaddr *)&addr, sizeof(addr)) < 0) {
    close(sd);
    error_message_and_exit(0, errno, "Error connecting socket to address: ");
  }

  return sd;
}

/**
 * Send a message over a socket
 *
 * @param sd  The file descriptor of the socket on which to write
 * @param msg The message to send
 */
void write_to_server(int sd, const char *msg) {
  const char *next_byte = msg;
  size_t remain = strlen(msg);
  while (remain) {
    size_t sent = write(sd, next_byte, remain);
    if (sent <= 0) {
      if (errno != EINTR) {
        error_message_and_exit(0, errno, "Error in write(): ");
      }
    } else {
      next_byte += sent;
      remain -= sent;
    }
  }
}
