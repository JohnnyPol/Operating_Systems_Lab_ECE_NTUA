#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    // Create two pipes
    int pipe1[2], pipe2[2];
    
    // Error handling for pipe creation
    if (pipe(pipe1) == -1) {
        perror("pipe1 failed");
        exit(EXIT_FAILURE);
    }
    
    if (pipe(pipe2) == -1) {
        perror("pipe2 failed");
        exit(EXIT_FAILURE);
    }
    // Duplicate the pipe's write and read ends to different FDs
    dup2(pipe1[0], 33);  // Replace FD 33 with the read end of pipe1
    dup2(pipe1[1], 34);  // Replace FD 34 with the write end of pipe1

    dup2(pipe2[0], 53);  // Replace FD 53 with the read end of pipe2
    dup2(pipe2[1], 54);  // Replace FD 54 with the write end of pipe2

    // Close the original pipe file descriptors since they are now duplicated
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    // Now we can execute the `./riddle` program
    char *args[] = {"./riddle", NULL};
    execv(args[0], args);

    // If execv fails, exit with error
    perror("execv failed");
    exit(EXIT_FAILURE);
}