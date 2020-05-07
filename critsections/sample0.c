#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>



typedef struct msgbuf {
   long mtype;       /* rodzaj wiadomości > 0 */
   char mtext[1];    /* wartość (może być więcej niż 1 bajt) */
} msgbuf;

int main(int argc, char **argv) {
	int op[2];
	pipe(op);	
	// key_t id = 12354; // mozna tez tak, ale nie jest to najlepsze rozwiazanie
	key_t id = ftok(argv[0],123); //
    int kolejka = msgget(id, IPC_CREAT | 0660 );
	if (kolejka >= 0) printf("Powstala kolejka %X\n", id);
	else {
		printf ("wynik: %s\n", strerror(errno));
		printf("Nie udalo sie stworzyc kolejki %X\n", id);
		return -1;
	}
	int pid = fork();
	if (pid == 0) {
		msgbuf bufor;
		bufor.mtext[0] = 'Y';
		bufor.mtype = 1;
		msgsnd(kolejka, &bufor, 1, 0);
		printf ("wynik: %s\n", strerror(errno));
		sleep(3);
	} else {
		msgbuf bufor;
		ssize_t ret = msgrcv(kolejka, &bufor, 1, 1,0);
		printf("Otrzymalem (%d) %c\n", (int)ret, bufor.mtext[0]);
		wait(NULL);
		msgctl( kolejka, IPC_RMID, 0); // skasowanie kolejki 
		printf ("wynik: %s\n", strerror(errno));
	}
	
	return 0;
}
