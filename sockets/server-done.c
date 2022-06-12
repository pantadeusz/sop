// zobacz jeszcze
// http://www.ibm.com/support/knowledgecenter/ssw_ibm_i_71/rzab6/poll.htm
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <grp.h>
#include <netinet/in.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  signal(SIGPIPE, SIG_IGN);
  int lsocket;
  int s;
  unsigned int port = 9999;
  int yes = 1;
  struct sockaddr_in my_addr;
  if ((lsocket = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }
  if (setsockopt(lsocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    perror("setsockopt");
    exit(1);
  }

  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(port);
  my_addr.sin_addr.s_addr = INADDR_ANY;
  memset(&(my_addr.sin_zero), 0, 8);
  if (bind(lsocket, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) ==
      -1) {
    perror("bind");
    exit(1);
  }
  if (listen(lsocket, 1) == -1) {
    perror("listen");
    exit(1);
  }
  while (1) {
    struct sockaddr_in remoteAddr;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    if ((s = accept(lsocket, (struct sockaddr *)&remoteAddr, &sin_size)) ==
        -1) {
      perror("on error");
      return -1;
    }
    unsigned char *addr = (char *)&(remoteAddr.sin_addr.s_addr);
    printf("otrzymalem polaczenie z %d.%d.%d.%d\n", addr[0], addr[1], addr[2],
           addr[3]);
    char buf[1024];
    size_t n = read(s, buf, 1024);
    if (n > 0) {
      int i;
      char method[128], uri[1024], version[1024];
      char fname[1024];
      buf[n] = 0;
      sscanf(buf, "%s %s %s", method, uri, version);
      printf("Dostalem zadanie zasobu: %s\n", uri);
      for (i = 0; i < strlen(uri); i++) {
        if (uri[i] == '?') uri[i] = 0;
        if (i + 1 < strlen(uri))
          if ((uri[i] == '.') && (uri[i + 1] == '.')) uri[i] = 0;
      }
      sprintf(fname, ".%s", uri);
      if (strcmp(fname, "./") == 0) {
        strcpy(fname, "./index.html");
      }
      printf("Otwieram: %s\n", fname);
      FILE *f = fopen(fname, "rb");
      if (f) {
        char header[1024];
        char *response = malloc(1024 * 1024 * 1024);
        char responsetype[1024];
        char ext[1024];
        size_t nresponse;
        nresponse = fread(response, 1, 1024 * 1024 * 1024, f);
        printf("Wysylam %d bajtow\n", (int)nresponse);
        strcpy(responsetype, "text/html; charset=UTF-8");
        strcpy(ext, strchr(fname, '.') + 1);
        if (strcmp(ext, "html") == 0) {
          strcpy(responsetype, "text/html; charset=UTF-8");
        } else {  // if (strcmp(ext, "html") == 0) {
          strcpy(responsetype, "");
        }
        sprintf(header,
                "HTTP/1.1 200 OK\nContent-Length: %d\nContent-Type: %s\n\n",
                (int)nresponse, responsetype);
        write(s, header, strlen(header));
        write(s, response, nresponse);
        fclose(f);
      } else {
        char header[1024];
        char response[1024];
        strcpy(response, "Nie znaleziono zasobu");
        size_t nresponse = strlen(response);
        sprintf(header,
                "HTTP/1.1 404 Not found\nContent-Length: %d\nContent-Type: "
                "text/html; charset=UTF-8\n\n",
                (int)nresponse);
        write(s, header, strlen(header));
        write(s, response, nresponse);
      }
    }
    close(s);
  }
  close(lsocket);
  return 0;
}
