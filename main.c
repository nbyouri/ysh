#include "global.h"

void xread(int fd) {
    char buf[BUFSIZ];
    FILE *file;
    if ((file = fdopen(fd, "r")) == NULL) {
        perror("Failed to fdopen\n");
    } else {
        while (!feof(file) &&
                !ferror(file) &&
                fgets(buf, sizeof(buf), file) != NULL) {
            printf("%s", buf);
        }
        fclose(file);
    }
}


int main(void) {
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        printf("Failed to pipe");
        return -1;
    }

    char **prog = readinput();

    if (fork() == 0) { // child

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
        xread(pipefd[PIPE_READ]);
    }
    return 0;
}
