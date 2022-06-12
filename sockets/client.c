// Zobacz strone manuala getaddrinfo
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 500

int main(int argc, char *argv[]) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s, j;
    size_t len;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* IPv4 lub IPv6 */
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
        sfd = socket(rp->ai_family, rp->ai_socktype,
                     rp->ai_protocol);
        if (sfd != -1) {
            if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {
                break;
            } else {
                close(sfd);
            }
        }
    }

    if (rp == NULL) { // nie udalo sie
        fprintf(stderr, "Blad polaczenia\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);
    {
        char buf[128];
        read(sfd, buf, 128);
        printf("Otrzymalem:\n%s\n",buf);
    }
    close(sfd);
    exit(EXIT_SUCCESS);
}
