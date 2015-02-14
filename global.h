#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <readline/readline.h>

#define EXIT "exit"
#define PIPE_READ 0
#define PIPE_WRITE 1
#define arraySize(x)    sizeof(x)/sizeof(*x)

extern bool stop;

void cleanPtr(char **);
char **stringToArray(char *, char *);
char *readinput(const char *);
void ReadFD(int *);
int ysh(void);
void *growArray(void *, size_t, size_t);
void dump(char **, size_t);
