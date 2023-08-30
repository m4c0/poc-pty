#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void uga(int fd, const char *msg) { write(fd, msg, strlen(msg)); }

int main() {
  char *args[2];
  args[0] = strdup("/usr/local/bin/vim");
  args[1] = 0;

  char *env[1];
  env[0] = 0;

  int err = open("/tmp/err", O_WRONLY | O_CREAT | O_TRUNC);
  if (!err) {
    perror("nope");
    return 1;
  }

  int out = open("/tmp/out", O_WRONLY | O_CREAT | O_TRUNC);
  if (!out) {
    perror("nope");
    return 1;
  }

  int fd[2];
  pipe(fd);

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  dup2(fd[0], STDIN_FILENO);
  dup2(out, STDOUT_FILENO);
  dup2(err, STDERR_FILENO);

  uga(fd[1], ":echo &columns\n");
  uga(fd[1], ":echo &lines\n");
  uga(fd[1], ":echo &ttytype\n");
  uga(fd[1], ":echo &term\n");
  uga(fd[1], ":echo has_key(environ(), 'HOME')\n");
  uga(fd[1], ":set! all\n");
  uga(fd[1], ":q\n");

  execve("/usr/local/bin/vim", args, env);
}
