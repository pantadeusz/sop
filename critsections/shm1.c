
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
       
#define ROZMIAR_BUFORA 128
#define EXPERIMENT_TESTS 11000
typedef struct {
    int licznik;
    int we;
    int wy;
    int bufor[ROZMIAR_BUFORA];
} mybuffer;


void dosignal(int id) { // operacja V
    struct sembuf operations[1];
    operations[0].sem_num = 0; // numer semafora
    operations[0].sem_op = 1; // dodanie 1 do semafora
    operations[0].sem_flg = 0; // oczekiwanie
    semop(id, operations, 1);
}

void dowait(int id) { // operacja P
    struct sembuf operations[1];
    operations[0].sem_num = 0; // numer semafora
    operations[0].sem_op = -1; // odejmujemy 1 od semafora
    operations[0].sem_flg = 0; // oczekiwanie
    semop(id, operations, 1);
}

void pushBuffer(volatile mybuffer *b,int semid, int x) {
    while (b->licznik == ROZMIAR_BUFORA) usleep(1);
    b->bufor[b->we] = x; // wysylamy do bufora
    b->we = (b->we + 1) % ROZMIAR_BUFORA;
    dowait(semid);
    b->licznik++;
    dosignal(semid);
}

int popBuffer(volatile mybuffer *b,int semid) {
    while(b->licznik == 0) ;
    int v = b->bufor[b->wy];
    b->wy = (b->wy + 1) % ROZMIAR_BUFORA;
    dowait(semid);
    b->licznik--;
    dosignal(semid);
    return v;
}

int producer(mybuffer *b,int semid) {
    int x = 0;
    while(1) {
        x++; // produkujemy nowa wartosc
        pushBuffer(b,semid,x);
        if (x == EXPERIMENT_TESTS) break;
    }
}

int consumer(mybuffer *b, int semid) {
    int v;
    while(1) {
        v = popBuffer(b, semid);
        printf("%d\n",v);
        if (v == EXPERIMENT_TESTS) break;
    }
}

int main(int argc, char **argv) {
    mybuffer *b; // bufor w pamięci współdzielonej

    key_t keyval = ftok(argv[0],997+sizeof(mybuffer));
    int shmid = shmget( keyval, sizeof(mybuffer), IPC_CREAT | 0660 ); // przygotowanie pamieci wspoldzielonej
    b = (mybuffer *)shmat(shmid, 0, 0);
    memset(b,0,sizeof(mybuffer));

    key_t semkey = ftok(argv[0],1997+sizeof(mybuffer)+2);
    int semid = semget(semkey, 1, 0666 | IPC_CREAT);
    semctl(semid, 0, SETVAL, 1); // ustawmy na poczatku 1

    if (fork()) { // rodzic
        producer(b,semid);
        wait(NULL);
    } else {
        consumer(b,semid);
        exit(0);
    }

    semctl (semid,0,IPC_RMID,0);
    shmdt ( b );
    shmctl ( shmid, IPC_RMID, 0 );
    return 0;
}
