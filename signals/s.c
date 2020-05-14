#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

volatile int x = 0;

void increase_counter(int signum) {
	x++;
	printf("tekst %d %d\n", x, signum);
}

int main(int argc, char **argv) {
	signal(SIGINT, increase_counter);
	while (x < 3) {
		printf("czekam... %d\n", x);
		sleep(1);
	}
	printf("koniec!\n");
	return 0;
}
