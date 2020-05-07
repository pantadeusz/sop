
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


int czekanie[8] = {0,0,0,0,0,0,0,0};
int zamek = 0;

int testAndSet(int *x) {
    int xx;
    #pragma omp critical
    {
        xx = *x;
        *x = 1;
    }
    return xx;
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
        czekanie[i] = 1;
        klucz = 1;
        while (czekanie[i] && klucz)
            klucz = testAndSet(&zamek);
        czekanie[i] = 0;

        // sekcja krytyczna

        licznik = licznik + 1;

        // sekcja wyjściowa
        j = (i+1)%n;
        while(j!=i && !czekanie[j])
            j = (j+1)%n;
        if (j == i)
            #pragma omp critical
            zamek = 0;
        else czekanie[j] = 0;


        if (x == 11000) break;
    }
}

int consumer(int i,int n) {
    int v;
    int klucz=0,j=0;
    while(1) {
        while(licznik == 0) ;
        v = bufor[wy];
        wy = (wy + 1) % ROZMIAR_BUFORA;


        // sekcja wejsciowa
        czekanie[i] = 1;
        klucz = 1;
        while (czekanie[i] && klucz) klucz = testAndSet(&zamek);
        czekanie[i] = 0;

        // sekcja krytyczna
        licznik = licznik - 1;

        // sekcja wyjściowa
        j = (i+1)%n;
        while(j!=i && !czekanie[j])
            j = (j+1)%n;
        if (j == i)
            #pragma omp critical
            zamek = 0;
        else czekanie[j] = 0;

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
