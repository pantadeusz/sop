// zobacz jeszcze http://www.ibm.com/support/knowledgecenter/ssw_ibm_i_71/rzab6/poll.htm
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <grp.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>


int main() {
    signal(SIGPIPE, SIG_IGN);
    int lsocket;
    int s;
    unsigned int port = 9999;
    int yes=1;
    int no=0;
    struct sockaddr_in my_addr;
    if ((lsocket = socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1) {
        perror("socket");
        exit(1);
    }
    if (setsockopt(lsocket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(my_addr.sin_zero), 0, 8);
    if (bind(lsocket, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(1);
    }
    if (listen(lsocket, 10) == -1) {
        perror("listen");
        exit(1);
    }
    /*
    if (ioctl(lsocket, FIONBIO, (char *)&yes) < 0) {
        perror("ioctl");
        close(lsocket);
        exit(-1);
    } */

    while (1) {
        struct sockaddr_in remoteAddr;
        socklen_t sin_size = sizeof(struct sockaddr_in);

        if ((s = accept(lsocket, (struct sockaddr *)&remoteAddr, &sin_size)) == -1) {
            //perror("on error");
            printf("czekam\n");
            sleep(1); // raczej nalezy stosowac poll albo select
            //return -1;
        } else {
//            if (ioctl(s, FIONBIO, (char *)&no) < 0) {
//                perror("ioctl - s");
//                exit(-1);
//            }
            unsigned char *addr = (char*)&(remoteAddr.sin_addr.s_addr);
            printf("otrzymalem polaczenie z %d.%d.%d.%d\n",addr[0],addr[1],addr[2],addr[3]);
            {
                char buf[128];
                strcpy(buf,"Witaj :D");
                write(s, buf, 128);
            }
            close(s);
        }
    }
    close(lsocket);
    return 0;
}
