#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	int r = fork();
	if (r == 0) {
		int a;
		printf("r=0\n");
		scanf("%d",&a);
		printf("r=0:  %d\n",a);
		sleep(10);
	} else {
		int a;
		printf("r!=0\n");
		scanf("%d",&a);
		printf("r!=0:  %d\n",a);
		sleep(10);
	}
	return 0;
}

