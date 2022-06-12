#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  FILE *f;
  char buf[1024];
  f = popen("echo witaj", "r");
  if (f == NULL) {
    return -1;
  }

  while (fread(buf, 1, 1024, f) > 0) {
    printf("%s", buf);
  }

  pclose(f);

  return 0;
}
