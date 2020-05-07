
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>

// testujemy:
// for i in `seq 1 100`; do echo $i; ./a.out | wc | grep -vE '^..11000.*'; done

int licznik = 0;
int we = 0, wy = 0;
#define ROZMIAR_BUFORA 1024
int bufor[ROZMIAR_BUFORA];


int S = 1;

void wait(int *x) {
    while (*x <= 0) usleep(1);
    (*x)--;
}
void signal(int *x) {
    #pragma omp atomic
    (*x)++;
}

int producer(int i, int n) {
    int x = 0;
    int klucz=0,j=0;
    while(1) {
        x++; // produkujemy nowa wartosc
        while (licznik == ROZMIAR_BUFORA) ;
        bufor[we] = x; // wysylamy do bufora
        we = (we + 1) % ROZMIAR_BUFORA;

        // sekcja wejsciowa
        wait(&S);

        // sekcja krytyczna

        licznik = licznik + 1;

        // sekcja wyjściowa
        signal(&S);


        if (x == 11000) break;
    }
}

int consumer(int i,int n) {
    int v;
    while(1) {
        while(licznik == 0) ;
        v = bufor[wy];
        wy = (wy + 1) % ROZMIAR_BUFORA;


        // sekcja wejsciowa
        wait(&S);

        // sekcja krytyczna
        licznik = licznik - 1;

        // sekcja wyjściowa
        signal(&S);

        printf("%d\n",v);
        if (v == 11000) break;
    }

}

int main() {

    #pragma omp parallel num_threads(2) shared(licznik,we,wy,bufor)
    {
        int i = omp_get_thread_num();
        if (i == 0) {
            consumer(0,2);
        } else if (i == 1) {
            producer(1,2);
        }
    }
    return 0;
}
