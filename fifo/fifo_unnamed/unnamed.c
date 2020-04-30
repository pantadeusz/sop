#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int suma(int a, int b) {
	int s = 0;
	for (int i = 0; i < b; i++) s += i;
	return s;
}

int main() {
	
	int fd[2]; // 1 - do zapisu, 0 - do odczytu
	pipe(fd);
	
	if (fork()) {
		int partsum = suma(0,100);
		printf("rodzic - suma: %d\n", partsum);
		int part2sum;
		read(fd[0],&part2sum,sizeof(part2sum));
		close(fd[0]); // zamykamy do odczytu
		int pidenidng, status;
		pidenidng = wait(&status);
		printf("rodzic dla (%d -> %x)- suma całości: %d\n",pidenidng,status, partsum+part2sum);
	} else {
		
		int partsum = suma(100,200);
		printf("dziecko(%d) - suma: %d\n", getpid(), partsum);
		write(fd[1],&partsum,sizeof(partsum));
		close(fd[1]); // zamykamy do zapisu
		return 0x0ab;
	}
	
	return 0;
}
