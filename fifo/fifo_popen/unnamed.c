#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
	char s[1024];
	FILE *stream = popen("ls", "r");
	if (stream == NULL) printf("ERR\n");
	while (fgets(s,1024,stream))
	printf("PLIK: %s", s);
	pclose(stream);
	
	return 0;
}
