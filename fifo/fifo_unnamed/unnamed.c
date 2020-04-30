#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
	
	int fd[2];
	pipe(fd);
	if (fork()) {
		char data[20]="Witaj p	";
		write(fd[1],data,10);
		close(fd[1]); // zamykamy do zapisu
		wait(NULL);
	} else {
		char data[20];
		read(fd[0],data,10);
		printf("%d: %s\n", getpid(),data);
		close(fd[0]); // zamykamy do odczytu
	}
	
	return 0;
}
