#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h> // Required for pid_t
#include <sys/stat.h>  // Required for open flags
#include <linux/fcntl.h>

int main(int argc, char* argv[]) 
{

  char buffer[4096];

  int fd = openat(AT_FDCWD,"secret_number", O_RDWR|O_CREAT,0600);
  pid_t pid = fork();
  if (pid == 0){
    char *args[] = {"./riddle", NULL};
    execv(args[0], args);
  }
  else {
    sleep(5);
    int bytes_read = read(fd,buffer,4096);
    printf("%s\n",buffer);
    sleep(15);
  }
}
