#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	int r = fork();
	if (r == 0) {
		while (1) {
			FILE *f = fopen("exit","r");
			if (f) {
				fclose(f);
				unlink("exit");
				break;
			}
			sleep(1);
			printf(".");
			fflush(stdout);
		}
	} else {
		while (1) {
			char c;
			scanf("%c",&c);
			if (c == 'q') {
				FILE *f = fopen("exit","w");
				fclose(f);
				break;
			}
		}
		wait(NULL);
	}
	printf("(%d) PID: %d koniec\n", r, getpid());
	return 0;
}

