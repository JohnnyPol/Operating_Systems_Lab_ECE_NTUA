#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int main() {
    pid_t pid;
    
    // Loop indefinitely until we find the desired PID
    while (1) {
        // Fork a new process
        pid = fork();

        if (pid < 0) {
            // Error handling for fork failure
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // In child process, execute the riddle program
            execlp("./riddle", "./riddle", NULL);

            // If execlp fails
            perror("execlp failed");
            exit(EXIT_FAILURE);
        } else {
            // In parent process, wait for the child to finish
            int status;
            waitpid(pid, &status, 0);

            // Check if the child's PID is 32767
            if (pid == 32767) {
                printf("Achieved PID 32767 for ./riddle!\n");
                break; // Exit the loop if we found the desired PID
            } else {
                printf("Current PID: %d (not 32767)\n", pid);
            }
        }
    }

    return 0;
}

