#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <netdb.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>

int listen_server(char *server_name, char *port_name, int max_queue) {
  int listening_socket;
  if (server_name == NULL)
    server_name = "0.0.0.0";
  if (port_name == NULL)
    port_name = "9921";
  if (max_queue <= 0)
    max_queue = 32;
  struct addrinfo hints;
  bzero((char *)&hints, sizeof(struct addrinfo));

  hints.ai_family = AF_UNSPEC;     ///< IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; ///< Stream socket
  hints.ai_flags = AI_PASSIVE;     ///< For wildcard IP address
  hints.ai_protocol = 0;           ///< Any protocol
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;

  struct addrinfo *result, *rp;
  int s = getaddrinfo(server_name, port_name, &hints, &result);
  if (s != 0) {
    fprintf(stderr, gai_strerror(s));
    return -1;
  }

  // try to create socket and bind it to address
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    listening_socket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (listening_socket != -1) {
      int yes = 1;
      if (setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &yes,
                     sizeof(yes)) == -1) {
        fprintf(stderr, "setsockopt( ... ) error\n");
        return -1;
      }
      if (bind(listening_socket, rp->ai_addr, rp->ai_addrlen) == 0) {
        freeaddrinfo(result);
        if (listen(listening_socket, max_queue) == -1) {
          fprintf(stderr, "listen error\n");
          return -1;
        }
        return listening_socket;
      }
      close(listening_socket); // didn't work, let's close socket
    }
  }
  freeaddrinfo(result);
  fprintf(stderr, "error binding adress\n");
  return -1;
}

volatile int listening_socket;
volatile int serving = 1;
void on_ctrlc() {
  serving = 0;
  printf("Koncze...\n");
  close(listening_socket); // stop listening on socket
}

int main(int argc, char **argv) {
  signal(SIGINT, on_ctrlc);
  listening_socket = listen_server("*", (argc > 1) ? argv[1] : "9921", 2);
  printf("Waiting for connection...\n");
  char host[NI_MAXHOST], service[NI_MAXSERV];

  struct sockaddr_storage peer_addr;
  socklen_t peer_addr_len = sizeof(struct sockaddr_storage);
  int requ_number = 0;
  while (serving) {
    int s =
        accept(listening_socket, (struct sockaddr *)&peer_addr, &peer_addr_len);
    requ_number++;
    if (fork() == 0) {
      char msg[5];
      // while (read(s, msg, 1) > 0) {
      //  printf("%c",msg[0]);
      //}
      printf("odsylamy\n");
      char response[10000];

      sprintf(response,
              "HTTP/1.1 200 OK\r\n"
              "Date: Mon, 23 May 2005 22:38:34 GMT\r\n"
              "Content-Type: text/html; charset=UTF-8\r\n"
              "Content-Length: 138\r\n"
              "Last-Modified: Wed, 08 Jan 2003 23:11:55 GMT\r\n"
              "Server: Apache/1.3.3.7 (Unix) (Red-Hat/Linux)\r\n"
              "Accept-Ranges: bytes\r\n"
              "Connection: close\r\n"
              "\r\n"
              "<html>\r\n"
              "  <head>\r\n"
              "    <title>An Example Page</title>\r\n"
              "  </head>\r\n"
              "  <body>\r\n"
              "    <p>Hello World, this is a very simple %4d document.</p>\r\n"
              "  </body>\r\n"
              "</html>\r\n\r\n",
              requ_number);
      sleep(5);
      write(s, response, strlen(response));
      close(s); // close connected socket
      return 0;
    }
  }
  return 0;
}
