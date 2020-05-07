// zobacz http://www.tldp.org/LDP/lpg/node70.html
// int shmdt ( char *shmaddr );




#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <sys/shm.h>

#define ROZMIAR_BUFORA 128
typedef struct {
    int licznik;
    int we;
    int wy;
    int bufor[ROZMIAR_BUFORA];
} mybuffer;

void pushBuffer(mybuffer *b, int x) {
    while (b->licznik == ROZMIAR_BUFORA) ;
    b->bufor[b->we] = x; // wysylamy do bufora
    b->we = (b->we + 1) % ROZMIAR_BUFORA;
    b->licznik++;
}

int popBuffer(mybuffer *b) {
    while(b->licznik == 0) ;
    int v = b->bufor[b->wy];
    b->wy = (b->wy + 1) % ROZMIAR_BUFORA;
    b->licznik--;
    return v;
}

int producer(mybuffer *b) {
    int x = 0;
    while(1) {
        x++; // produkujemy nowa wartosc
        pushBuffer(b,x);
        if (x == 11000) break;
    }
}

int consumer(mybuffer *b) {
    int v;
    int x = 0;
    while(1) {
        v = popBuffer(b);
        printf("%d\n",v);
        if (v == 11000) break;
    }

}

int main(int argc, char **argv) {
    mybuffer *b; // bufor w pamięci współdzielonej
    key_t keyval = ftok(argv[0],1234);
    printf("%d %x\n", (int)sizeof(mybuffer),(int)keyval);
    int shmid = shmget( keyval, sizeof(mybuffer), IPC_CREAT | 0660 ); // przygotowanie pamieci wspoldzielonej
    if (fork()) { // rodzic
        b = (mybuffer *)shmat(shmid, 0, 0);
        memset(b,0,sizeof(mybuffer));
        producer(b);
        wait();
    } else {
        b = (mybuffer *)shmat(shmid, 0, 0);
        consumer(b);
    }
    shmdt ( b );
    shmctl ( shmid, IPC_RMID, 0 );
    return 0;
}
