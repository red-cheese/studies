#include <signal.h>
#include <stdlib.h>

int main(int arc, char* argv[]) {
  int signo = (int) strtol(argv[1], NULL, 10);
  int pid = (int) strtol(argv[2], NULL, 10);
  return kill(pid, signo);
}
