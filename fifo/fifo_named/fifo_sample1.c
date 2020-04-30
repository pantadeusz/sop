#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
	if (argc > 1){
		mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
		return mkfifo(argv[1], mode);
	}else{
		return -1;
	}
}
