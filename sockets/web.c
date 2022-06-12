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
  int sockfd;
  unsigned int port = 8080;
  int yes = 1;
  struct sockaddr_in my_addr;
  if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }

  // opcja
  /*    if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
          perror("setsockopt");
          exit(1);
      } */

  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(port);
  my_addr.sin_addr.s_addr = INADDR_ANY;
  memset(&(my_addr.sin_zero), '\0', 8);

  if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) ==
      -1) {
    perror("bind");
    exit(1);
  }

  if (listen(sockfd, 10) == -1) {
    perror("listen");
    exit(1);
  }

  while (1) {
    char *in = (char *)malloc(5000000);   // dane przychodzace
    char *sent = (char *)malloc(500000);  // dane wychodzace
    int buffer_chunks;
    long filesize, range = 0;
    int clientSocket;
    struct sockaddr remoteAddr;

    socklen_t sin_size = sizeof(struct sockaddr_in);
    if ((clientSocket =
             accept(sockfd, (struct sockaddr *)&remoteAddr, &sin_size)) == -1) {
      perror("on error");
      return -1;
    }

    if (read(clientSocket, in, 5000000) == -1) {
      perror("recive");
      close(clientSocket);
      free(in);
      free(sent);
      return -1;
    } else {
      char method[1024];
      char queryString[1024];
      char proto[1024];
      sscanf(in, "%s %s %s", method, queryString, proto);
      char responseData[1024] = "Siemano\n";
      int responseSize = strlen(responseData);
      sprintf(
          sent,
          "HTTP/1.1 200 - ok\nServer: myawesomeserver\nContent-Length: "
          "%d\nConnection: close\nContent-Type: text/html; charset=utf8\n\n",
          responseSize);
      send(clientSocket, sent, strlen(sent), MSG_NOSIGNAL);
      send(clientSocket, responseData, responseSize, MSG_NOSIGNAL);
      close(clientSocket);
      free(in);
      free(sent);
    }
  }
  close(sockfd);
}
