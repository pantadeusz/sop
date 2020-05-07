#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>

// testujemy:
// for i in `seq 1 100`; do ./a.out | wc | grep -vE '^..11000.*'; done

int licznik = 0;
int we = 0, wy = 0;
#define ROZMIAR_BUFORA 1024
int bufor[ROZMIAR_BUFORA];


int lock = 0;

int testAndSet(int *x) {
    int xx;
    #pragma omp critical
    {
        xx = *x;
        *x = 1;
    }
    return xx;
}

int producer() {
    int x = 0;
    while(1) {
        x++; // produkujemy nowa wartosc
        while (licznik == ROZMIAR_BUFORA) ;
        bufor[we] = x; // wysylamy do bufora
        we = (we + 1) % ROZMIAR_BUFORA;

        // sekcja wejsciowa
        while(testAndSet(&lock)) ;

        // sekcja krytyczna

        licznik = licznik + 1;

        // sekcja wyjściowa
        #pragma omp critical
        lock = 0;

        if (x == 11000) break;
    }
}

int consumer() {
    int v;
    while(1) {
        while(licznik == 0) ;
        v = bufor[wy];
        wy = (wy + 1) % ROZMIAR_BUFORA;


        // sekcja wejsciowa
        while(testAndSet(&lock)) ;

        // sekcja krytyczna
        licznik = licznik - 1;

        // sekcja wyjściowa
        #pragma omp critical
        lock = 0;

        printf("%d\n",v);
        if (v == 11000) break;
    }

}

int main() {

    #pragma omp parallel num_threads(2) shared(licznik,we,wy,bufor)
    {
        int i = omp_get_thread_num();
        if (i == 0) {
            consumer();
        } else if (i == 1) {
            producer();
        }
    }
    return 0;
}
