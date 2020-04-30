#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>


#define MSG_SIZE 1000

char **getlist(char *fname) {
    FILE *f = fopen(fname,"r");
    int items = 0;
    char **ret = (char **) malloc(sizeof(char**)*1);
    ret[items] = 0;

    char tmp[1000];
    while (fgets(tmp,1000,f)) {
        ret[items] = (char*)malloc(strlen(tmp));
        if (tmp[strlen(tmp)-1] == '\n') tmp[strlen(tmp)-1] = 0;
        strcpy(ret[items], tmp);
        items++;
        ret = realloc(ret, sizeof(char**)*items+1);
        ret[items] = 0;
    }
    fclose(f);
    return ret;
}

/**
 *  ./nazwa mojefifo plik_koordynujacy
 *
 * */
int main(int argc, char **argv) {

    if (argc > 1) {
        mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
        mkfifo(argv[1], mode);

        FILE *usersf =  fopen(argv[2], "a");
        fprintf(usersf, "%s\n",argv[1]);
        fclose(usersf);

        int cpid = fork();
        char text[MSG_SIZE];
        if (cpid) { // rodzic

            while (	fgets( text, MSG_SIZE, stdin) ) {

                char **list = getlist(argv[2]);
                for (int i = 0; list[i]; i++) {
                    FILE *f = fopen(list[i],"w");
                    fwrite(text, MSG_SIZE, 1, f);
                    fclose(f);
                    free(list[i]);
                }
                free(list);

            }
//			unlink(argv[1]);
            wait(NULL);
        } else { // potomek
            FILE *f;
            while (f = fopen(argv[1],"r") ) {
                fread(text, MSG_SIZE, 1, f);
                fclose(f);
                printf("%s: '%s'\n", argv[2], text);
            }
        }
    } else {
        return -1;
    }
}
