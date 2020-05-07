#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>

// testujemy:
// for i in `seq 1 100`; do ./a.out | wc | grep -vE '^..11000.*'; done


/// UWAGA: nie dziala, ze wzgledu na to, ze nie da sie przejzyscie opragmowac wszystkich niepodzielnych operacji

int licznik = 0;
int we = 0, wy = 0;
#define ROZMIAR_BUFORA 1024
int bufor[ROZMIAR_BUFORA];


// rozwiazanie sekcji krytycznej:
int wybieranie[3] = {0,0,0};
int numer[3] = {0,0,0};
#define max(a,b,c) ((((a>b)?(a):(b))>(c))?(((a>b)?(a):(b))):(c))

int producer() {
	int i = 0, j;
	int x = 0;
	while(1) {
	  x++; // produkujemy nowa wartosc
	  while (licznik == ROZMIAR_BUFORA) ;
	  bufor[we] = x; // wysylamy do bufora
	  we = (we + 1) % ROZMIAR_BUFORA;
	  
	  // sekcja wejsciowa
	  wybieranie[i] = 1;
	  numer[i] = max(numer[0],numer[1],numer[2])+1;
	  wybieranie[i] = 0;
	  for(j = 0; j < 3; j++) {
		  while (wybieranie[j]);
		  int cond = 1;
		  while (cond) 
		  cond = (numer[j] != 0) && ( (numer[j] < numer[i]) || ((numer[j] == numer[i]) && (j < i)));
	  }
	  
	  // sekcja krytyczna

	  licznik = licznik + 1;

	  // sekcja wyjściowa
	  #pragma omp critical
	  numer[i] = 0;

	  if (x == 11000) break;
	}
}

int consumer1() {
	int i = 1, j;
	int v;
	while(1) {
	  while(licznik == 0) ;
	  v = bufor[wy];
	  wy = (wy + 1) % ROZMIAR_BUFORA;
	  
	  // sekcja wejsciowa
	  wybieranie[i] = 1;
	  numer[i] = max(numer[0],numer[1],numer[2])+1;
	  wybieranie[i] = 0;
	  for(j = 0; j < 3; j++) {
		  while (wybieranie[j]);
		  int cond = 1;
		  while (cond) 
		  cond = (numer[j] != 0) && ( (numer[j] < numer[i]) || ((numer[j] == numer[i]) && (j < i)));
	  }
     
	  // sekcja krytyczna
	  licznik = licznik - 1;

	  // sekcja wyjściowa
	  #pragma omp critical
	  numer[i] = 0;

	  printf("%d\n",v);
	  if ((v == 11000) || (v == 10999)) break;
	}
	
}

int consumer2() {
	int i = 2, j;
	int v;
	while(1) {
	  while(licznik == 0) ;
	  v = bufor[wy];
	  wy = (wy + 1) % ROZMIAR_BUFORA;
	  
	  // sekcja wejsciowa
	  wybieranie[i] = 1;
	  numer[i] = max(numer[0],numer[1],numer[2])+1;
	  wybieranie[i] = 0;
	  for(j = 0; j < 3; j++) {
		  while (wybieranie[j]);
		  int cond = 1;
		  while (cond) 
		  cond = (numer[j] != 0) && ( (numer[j] < numer[i]) || ((numer[j] == numer[i]) && (j < i)));
	  }
     
	  // sekcja krytyczna
	  licznik = licznik - 1;

	  // sekcja wyjściowa
	  #pragma omp critical
	  numer[i] = 0;

	  printf("%d\n",v);
	  if ((v == 11000) || (v == 10999)) break;
	}
	
}

int main() {

	#pragma omp parallel num_threads(3) shared(licznik,we,wy,bufor)
		{
			int i = omp_get_thread_num();
			if (i == 0){
				consumer1();
			} else if (i == 1) {
				producer();
			} else if (i == 2) {
				consumer2();
			}
		}
	return 0;
}
