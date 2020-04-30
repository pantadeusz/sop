#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
	if (argc > 1){
		mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
		mkfifo(argv[1], mode);
		
		int cpid = fork();
		if (cpid) { // rodzic
			printf("fifo wysylam\n");
			char text[32] = "Witaj FIFO";
			FILE *f = fopen(argv[1],"w");
			fwrite(text, 32, 1, f);
			fclose(f);
			wait(NULL);
		} else { // potomek
			char text[32];
			FILE *f = fopen(argv[1],"r");
			fread(text, 32, 1, f);
			fclose(f);
			printf("odczytalem '%s'\n", text);
		}
	}else{
		return -1;
	}
}
