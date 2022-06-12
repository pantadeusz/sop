// Zobacz strone manuala getaddrinfo
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 500

int main(int argc, char *argv[]) {
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  int sfd, s, j;
  size_t len;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC; /* IPv4 lub IPv6 */
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = 0;
  hints.ai_protocol = 0;

  s = getaddrinfo("127.0.0.1", "9999", &hints, &result);
  if (s != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    exit(EXIT_FAILURE);
  }

  // getaddrinfo() zwraca liste jednostronnie wiazana
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (sfd != -1) {
      if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {
        break;
      } else {
        close(sfd);
      }
    }
  }

  if (rp == NULL) {  // nie udalo sie
    fprintf(stderr, "Blad polaczenia\n");
    exit(EXIT_FAILURE);
  }

  freeaddrinfo(result);
  {
    int x, y;

    x = 10;
    y = 20;
    if (argc == 3) {
      x = strtod(argv[1], NULL);
      y = strtod(argv[2], NULL);
    }
    write(sfd, &x, sizeof(x));
    write(sfd, &y, sizeof(y)); 
    int r;   
    int rsize = read(sfd, &r, sizeof(r));
    printf("Otrzymalem(%d):\n%d+%d = %d\n", rsize, x, y, r);
  }
  close(sfd);
  exit(EXIT_SUCCESS);
}
