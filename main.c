#include "global.h"

void xread(int *fd) {
    char buf[BUFSIZ];
    FILE *file;
    file = fdopen(*fd, "r");
    if (file == NULL) {
        perror("Failed to fdopen\n");
        exit(EXIT_FAILURE);
    } else {
        while (!feof(file) &&
                !ferror(file) &&
                fgets(buf, sizeof(buf), file) != NULL) {
            printf("%s", buf);
        }
        if (fclose(file) == EOF) {
            perror("Failed to fclose\n");
        }
    }
}


int main(void) {
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        printf("Failed to pipe");
        return -1;
    }

    if (fork() == 0) { // child
        // listen to the command from parent process
        char buf[BUFSIZ];
        char **prog = NULL;
        if (read(pipefd[PIPE_READ], buf, sizeof(buf)) == -1) {
            printf("Failed to get command from parent process\n");
        } else {
            prog = stringToArray(buf, " \t\n");
        }

        // redirect stdout
        if (dup2(pipefd[PIPE_WRITE], STDOUT_FILENO) == -1) {
            printf("Failed to dup2 stdout\n");
        }

        if (close(pipefd[PIPE_READ]) == -1) {
            printf("Failed to close pipefd[0]\n");
        }

        int ret = execvp(prog[0], prog);

        cleanPtr(prog);
        exit(ret);
    } else { // parent
        char *prog = readline(" > ");

        if (write(pipefd[PIPE_WRITE], prog, sizeof(prog)) == -1) {
            printf("Failed to send command to child process\n");
        }

        if (close(pipefd[PIPE_WRITE]) == -1) {
            printf("failed to close PIPE_WRITE\n");
        } else {
            xread(&pipefd[PIPE_READ]);
        }
    }
    return 0;
}
