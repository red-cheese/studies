#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int call_cnt = 0;

void sigterm_handler(int signo) {
  if (++call_cnt > 2) {
    exit(EXIT_SUCCESS);
  }
  printf("handler is being called %d time\n", call_cnt);
}

void sigusr1_handler(int signo) {
  printf("Alexandra Mikhaylova\n");
}

int main() {
  signal(SIGTERM, sigterm_handler);
  signal(SIGUSR1, sigusr1_handler);

  while(1) {
    sleep(1);
  }

  return 0;
}
