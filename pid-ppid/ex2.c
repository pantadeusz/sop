#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	printf("(START) PID: %d\n",  getpid());
	printf("(START) PPID: %d\n", getppid());
	int r = fork();
	if (r == 0) {
		printf("(%d) PID: %d\n",  r, getpid());
		printf("(%d) PPID: %d\n", r, getppid());
		sleep(1);
	} else {
		printf("(%d) PID: %d\n",  r, getpid());
		printf("(%d) PPID: %d -- czekam...\n", r, getppid());
		wait(NULL);
		printf("(%d) PPID: %d -- wyczekalem\n", r, getppid());
	}
	printf("(%d) PID: %d koniec\n", r, getpid());
	return 0;
}
