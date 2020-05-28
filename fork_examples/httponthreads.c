#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void *handle_request(void *arguments) {
  int *arr = (int *)arguments;
  int s = arr[0];
  int requ_number = arr[1];

  char *request_string = malloc(1);
  int request_size = 0;
  while (read(s, request_string + request_size, 1) > 0) {
    request_size++;
    request_string = realloc(request_string, request_size);
    request_string[request_size] = 0;
    if (strcmp(request_string + request_size - 4, "\r\n\r\n") == 0)
      break;
  }
  printf("DOSTALISMY ZAPYTANIE:\n%s\n--------KONIEC ZAPYTANIA\n",
         request_string);
  printf("odsylamy\n");
  char response[10000];

  char response_headers[1000];
  char *response_body = malloc(1000);
  char request_method[31], request_url[1001], request_version[101];
  sscanf(request_string, "%30s %1000s %100s", request_method, request_url,
         request_version);
  FILE *f = fopen(request_url + 1, "r");
  int filesize = 0;
  if (f) {
    while (1) {
      int c = fgetc(f);
      if (c == EOF)
        break;
      response_body[filesize] = c;
      filesize++;
      if (filesize > 1000)
        response_body = realloc(response_body, filesize);
    }
    fclose(f);
  } else {
    sprintf(response_body,
            "<html>\r\n"
            "  <head>\r\n"
            "    <title>Strona informacyjna</title>\r\n"
            "  </head>\r\n"
            "  <body>\r\n"
            "    <p>Nie udało się otworzyć pliku %s.</p>\r\n"
            "  </body>\r\n"
            "</html>\r\n\r\n",
            request_url);
    filesize = strlen(response_body);
  }
  sprintf(response_headers,
          "Content-Type: text/html; charset=UTF-8\r\n"
          "Content-Length: %d\r\n"
          "Server: SOP Masters\r\n"
          "Accept-Ranges: bytes\r\n"
          "Connection: close\r\n",
          filesize);
  sprintf(response, "HTTP/1.1 200 OK\r\n"
                    "%s"
                    "\r\n",
          response_headers);
  write(s, response, strlen(response));
  write(s, response_body, filesize);
  close(s); // close connected socket
  free(request_string);
  free (response_body);
  return 0;
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
    pthread_t thread;
    int request_params[] = {s, requ_number};
    pthread_create(&thread, NULL, handle_request, &request_params);
  }
  return 0;
}
