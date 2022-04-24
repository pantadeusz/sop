/*


(./a.out 1000000000 A&);(nice -n 19 ./a.out 1000000000 B&); sleep 1; ps aux | grep a.out | grep -v grep ;sleep 9; ps aux | grep a.out | grep -v grep


*/
#include <stdio.h>
int main (int argc, char **argv) {
	unsigned long long int N;
	if (argc < 3) return -1;
	sscanf(argv[1],"%lld",&N);
	
	unsigned long long int results[4] = {0,0,0,0};
	#pragma omp parallel for
	for (int times = 0; times < 4; times++) {
		unsigned long long int counter[3] = {(unsigned long long int)times,1,1};
		for (int i = 2; i < N; i++) {
				counter[2] = counter[1] + counter[0];
				counter[0] = counter[1];
				counter[1] = counter[2];
		}
		results[times] = counter[2];
	}
	printf("%s %lld %lld %lld %lld\n",argv[2], results[0], results[1], results[2], results[3]);
	return 0;
}
