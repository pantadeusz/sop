#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
	if (argc > 1){
        umask(0);
        return mknod(argv[1], S_IFIFO|0666, 0);
	}else{
		return -1;
	}
}
