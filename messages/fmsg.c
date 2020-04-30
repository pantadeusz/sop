#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>

typedef struct msgbuf {
    long mtype;       // rodzaj wiadomosci > 0
    char mtext[1];    // dane do wiadomosci, tu 1 bajt, ale moglaby to byc jakas struktura
} msgbuf;

int main(int argc, char **argv) {
    key_t id = ftok(argv[0],123);
    msgctl( id, IPC_RMID, 0); // wyczyszczenie kolejki (jakby sie przypadkiem okazalo ze cos tam jeszcze czeka)
    int kolejka = msgget(id, IPC_CREAT | 0660 );
    printf("Kolejka (%d): %d\n", id, kolejka);
    int pid = fork();
    if (pid == 0) {
        msgbuf bufor;
        bufor.mtext[0] = 'Y';
        bufor.mtype = 1;
        int ret = msgsnd(kolejka, &bufor, 1, 0);
        switch(errno) {
        case EACCES:
            printf("EACCES=%d \n", ret);
            break;
        case EAGAIN:
            printf("EAGAIN=%d \n", ret);
            break;
        case EFAULT:
            printf("EFAULT=%d \n", ret);
            break;
        case EIDRM:
            printf("EIDRM=%d \n", ret);
            break;
        default:
            printf("no error=%d \n", ret);
        }

    } else {
        msgbuf bufor;
        ssize_t ret = msgrcv(kolejka, &bufor, 1, 1,0);
        printf("Otrzymalem (%d) %c\n", ret, bufor.mtext[0]);
    }

    return 0;
}
