#include "global.h"

bool stop;

int main(int argc, char **argv) {
	// no arguments allowed
	if (argc >= 2) {
		printf("%s : This program takes no arguments,"
		    "please type your command\n"
		    "in the prompt.\n", argv[0]);
		return EXIT_FAILURE;
	} else {
		printf("Welcome to ysh, run `exit` to quit.\n");
		// shell loop
		while (stop == false) {
			ysh();
		}
	}

	// exit
	// XXX: handle signals for ctrl+c
	// XXX: cleanup functions
	return EXIT_SUCCESS;
}

int ysh(void) {
	int pipefd[2];
	int status;

	if (pipe(pipefd) == -1) {
		printf("Failed to pipe");
		return -1;
	}

	//pid_t parent = getpid();
	//printf("parent process = %d\n", parent);

	int childpid = 0;
	if (stop == false) {
		childpid = fork();
	}
	if (childpid == 0) { // child
		// listen to the command from parent process
		int ret = 0;
		char buf[BUFSIZ];
		char **prog = NULL;

		// read command as a string from parent
		ssize_t bytes = read(pipefd[PIPE_READ], buf, BUFSIZ);
		if (bytes == -1) {
			printf("Failed to get command from parent process\n");
		} else {
			// convert string read to array usable by execvp
			// but first trim it to the number of bytes read.
			buf[bytes] = '\0';
			prog = stringToArray(buf, " ");
			if (prog == NULL) {
				printf("Failed to get command.\n");
			}
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
		ret = execvp(prog[0], prog);

		// no need to free anything, execvp wipes
		// the address space.

		// exit with status returned from execvp
		_Exit(ret);
	} else if (childpid == -1) {
		perror("Failed to fork()\n");
		return EXIT_FAILURE;
	} else { // parent
		// read command as a string from user
		char *prog = readinput(" > ");

		if (prog == NULL) {
			return EXIT_FAILURE;
		}

		// send command to child process as a string
		ssize_t bytes = write(pipefd[PIPE_WRITE], prog, strlen(prog));
		if (bytes == -1) {
			printf("Failed to send command to child process\n");
		}

		// close the writing pipe
		if (close(pipefd[PIPE_WRITE]) == -1) {
			printf("failed to close PIPE_WRITE\n");
		} else {
			// read command output from child process
			ReadFD(&pipefd[PIPE_READ]);
		}
		// wait for child process and get it's pid
		pid_t child_p = wait(&status);
		printf("child %d finished with %d (%s)\n", child_p, status,
		    (status > 0) ? "bad" : "good");
		// if the command is "exit" then leave
		if (strcmp(prog, EXIT) == 0) {
			printf("exiting...\n");
			stop = true;
			return EXIT_SUCCESS;
		}

		prog = cleanPtr((char **)prog, NULL);
	}
	return status;
}
