
#include "net.h"
#include "vec.h"

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



/// Print an error message that combines some application-specific text with the
/// standard unix error message that accompanies errno.
///
/// @param err    The error code that was generated by the program
/// @param prefix The text to display before the error message
void sys_error(int err, const char *prefix) {
  char buf[1024];
  cerr << prefix << " " << strerror_r(err, buf, sizeof(buf)) << endl;
}
/// Internal method to send a buffer of data over a socket.
///
/// @param sd    The socket on which to send
/// @param bytes A pointer to the first byte of the data to send
/// @param len   The number of bytes to send
///
/// @returns True if the whole buffer was sent, false otherwise
bool reliable_send(int sd, const unsigned char *bytes, int len) {
  // When we send, we need to be ready for the possibility that not all the
  // data will transmit at once
  const unsigned char *next_byte = bytes;
  int remain = len;
  while (remain) {
    int sent = send(sd, next_byte, remain, 0);
    // NB: Sending 0 bytes means the server closed the socket, and we should
    //     fail, so it's only EINTR that is recoverable.
    if (sent <= 0) {
      if (errno != EINTR) {
        sys_error(errno, "Error in send():");
        return false;
      }
    } else {
      next_byte += sent;
      remain -= sent;
    }
  }
  return true;
}

/// Send a vector of data over a socket.
///
/// @param sd  The socket on which to send
/// @param msg The message to send
///
/// @returns True if the whole vector was sent, false otherwise
bool send_reliably(int sd, const vec &msg) {
  return reliable_send(sd, msg.data(), msg.size());
}

/// Send a string over a socket.
///
/// @param sd  The socket on which to send
/// @param msg The message to send
///
/// @returns True if the whole string was sent, false otherwise
bool send_reliably(int sd, const string &msg) {
  return reliable_send(sd, (const unsigned char *)msg.c_str(), msg.length());
}

/// Perform a reliable read when we are not sure how many bytes we are going to
/// receive.
///
/// @param sd     The socket from which to read
/// @param buffer The buffer into which the data should go.  It is assumed to be
///               pre-allocated to max bytes.
/// @param max    The maximum number of bytes to get
///
/// @returns A vector with the data that was read, or an empty vector on error
vec reliable_get_to_eof(int sd) {
    // set up the initial buffer
    vec res(16);
    int recd = 0;
    // start reading.  Double the buffer any time we fill up
    while (true) {
        int remain = res.size() - recd;
        int justgot = recv(sd, (res.data() + recd), remain, 0);
        // EOF means we're done reading
        if (justgot == 0) {
            res.resize(recd);
            return res;
        }
            // On error, fail for non-EINTR, no-op on EINTR
        else if (justgot < 0) {
            if (errno != EINTR) {
                sys_error(errno, "Error in recv():");
                return {};
            }
        }
            // bytes received.  advance pointer, maybe double the buffer
        else {
            recd += justgot;
            if (recd == (int)res.size()) {
                res.resize(2 * res.size());
            }
        }
    }
}
