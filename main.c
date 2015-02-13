#include "global.h"

int main(void) {
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        printf("Failed to pipe");
        return -1;
    }

    int childpid = fork();
    if (childpid == 0) { // child
        // listen to the command from parent process
        char buf[BUFSIZ];
        char **prog = NULL;

        // read command as a string from parent
        ssize_t bytes = read(pipefd[PIPE_READ], buf, BUFSIZ);
        if (bytes == -1) {
            printf("Failed to get command from parent process\n");
        } else {
            // convert string read to array usable by execvp
            prog = stringToArray(buf, " \t\n");
        }

        // redirect stdout
        if (dup2(pipefd[PIPE_WRITE], STDOUT_FILENO) == -1) {
            printf("Failed to dup2 stdout\n");
        }

        // close reading part of pipe
        if (close(pipefd[PIPE_READ]) == -1) {
            printf("Failed to close pipefd[0]\n");
        }

        // execute the command
        int ret = execvp(prog[0], prog);

        // clean the array of string allocated
        cleanPtr(prog);

        // exit with status returned from execvp
        exit(ret);
    } else if (childpid == -1) {
        perror("Failed to fork()\n");
        return -1;
    } else { // parent
        // read command as a string from user
        char *prog = readline(" > ");

        // send command to child process as a string
        ssize_t bytes = write(pipefd[PIPE_WRITE], prog, strnlen(prog, BUFSIZ));
        if (bytes == -1) {
            printf("Failed to send command to child process\n");
        }

        // close the writing pipe
        if (close(pipefd[PIPE_WRITE]) == -1) {
            printf("failed to close PIPE_WRITE\n");
        } else {
            // read command output from child process
            xread(&pipefd[PIPE_READ]);
        }
    }
    return 0;
}
