#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <util.h>

int out;

void wait_idle(int fd) {
  while (true) {
    fd_set set;
    FD_ZERO(&set);
    FD_SET(fd, &set);

    timeval timeout{
        .tv_sec = 0,
        .tv_usec = 500000,
    };

    switch (select(fd + 1, &set, NULL, NULL, &timeout)) {
    case -1:
      perror("select");
      return;
    case 0:
      return;
    default: {
      char buf[1024];
      int rd = read(fd, buf, 1024);
      write(out, buf, rd);
      break;
    }
    }
  }
}
void uga(int fd, const char *msg) {
  wait_idle(fd);
  write(fd, msg, strlen(msg));
}

int main() {
  char *args[2];
  args[0] = strdup("/usr/local/bin/vim");
  args[1] = 0;

  char *env[1];
  env[0] = 0;

  out = open("test.out", O_WRONLY | O_CREAT | O_TRUNC);
  if (!out) {
    perror("nope");
    return 1;
  }

  winsize sz{
      .ws_row = 60,
      .ws_col = 120,
  };

  int pri;
  switch (forkpty(&pri, nullptr, nullptr, &sz)) {
  case -1:
    return 1;
  case 0:
    execve("/usr/local/bin/vim", args, env);
    _exit(1);
    break;
  default:
    wait_idle(pri);
    uga(pri, ":echo &columns\n");
    uga(pri, ":echo &lines\n");
    uga(pri, ":echo &ttytype\n");
    uga(pri, ":echo &term\n");
    uga(pri, ":echo has_key(environ(), 'HOME')\n");
    uga(pri, ":set! all\n");
    uga(pri, "G");
    uga(pri, "\n");
    uga(pri, ":qa!\n");
    close(pri); // surprisingly important - otherwise child don't exit
    wait(0);
    break;
  }
}
