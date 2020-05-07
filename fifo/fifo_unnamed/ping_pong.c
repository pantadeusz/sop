#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * ping-pong example using pipes
 * */

int main(){
int fd[2];
pipe(fd);
printf("%d - %d\n",fd[0], fd[1]);
char txt[1000];
if (fork()) {
	// parent sends "witaj" and waits for the child to finish
	strcpy(txt,"witaj");
	printf("1 wyslalem %s\n",txt);
	write(fd[1],txt,100);
	wait(NULL);
	// now read the result
	read(fd[0],txt,100);
	printf("1 odebralem %s\n",txt);
	close(fd[1]);
	close(fd[2]);
} else {
	read(fd[0],txt,100);
	printf("2 odebralem %s\n",txt);
	strcat(txt," swiecie");
	printf("2 wysylam %s\n",txt);
	write(fd[1],txt,100);
	printf("2 wyslalem %s\n",txt);	
	close(fd[1]);
	close(fd[2]);
}
}
