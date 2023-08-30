#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <util.h>

void uga(int fd, const char *msg) {
  puts(msg);
  write(fd, msg, strlen(msg));
}

int main() {
  char *args[2];
  args[0] = strdup("/usr/local/bin/vim");
  args[1] = 0;

  char *env[1];
  env[0] = 0;

  int out = open("test.out", O_WRONLY | O_CREAT | O_TRUNC);
  if (!out) {
    perror("nope");
    return 1;
  }

  winsize sz{
      .ws_row = 120,
      .ws_col = 60,
  };

  int pri;
  switch (forkpty(&pri, nullptr, nullptr, &sz)) {
  case -1:
    return 1;
  case 0:
    dup2(out, STDOUT_FILENO);
    execve("/usr/local/bin/vim", args, env);
    break;
  default:
    uga(pri, ":echo &columns\n");
    uga(pri, ":echo &lines\n");
    uga(pri, ":echo &ttytype\n");
    uga(pri, ":echo &term\n");
    uga(pri, ":echo has_key(environ(), 'HOME')\n");
    uga(pri, ":set! all\n");
    sleep(5);
    uga(pri, "G");
    sleep(1);
    uga(pri, "\n");
    sleep(1);
    uga(pri, ":q\n");
    sleep(1);
    uga(pri, ":q\n");
    sleep(1);
    uga(pri, ":q\n");
    sleep(1);
    uga(pri, ":q\n");
    wait(0);
    break;
  }
}
