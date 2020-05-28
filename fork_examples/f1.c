#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void p0();
void p1();
void p2();
void p3();
void p4();

void p0() {
  printf("jestem P0[%d] parent:[%d]\n", getpid(), getppid());
  int childpid = fork();
  if (childpid == 0) {
    p2();
    return;
  }
  if (fork() == 0)
    p3();
}
void p1() { printf("jestem P1[%d] parent:[%d]\n", getpid(), getppid()); }
void p2() {
  printf("jestem P2[%d] parent:[%d]\n", getpid(), getppid());
  if (fork() == 0)
    p4();
}
void p3() { printf("jestem P3[%d] parent:[%d]\n", getpid(), getppid()); }
void p4() { printf("jestem P4[%d] parent:[%d]\n", getpid(), getppid()); }

int main() {
  if (fork()) {
    p0();
  } else {
    p1();
  }
  sleep(15);
  return 0;
}