#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define EXIT "exit"
#define PIPE_READ 0
#define PIPE_WRITE 1

extern bool stop;

void *cleanPtr(char **, unsigned int *);
char **stringToArray(char *, char *);
char *readinput(const char *);
void ReadFD(int *);
int ysh(void);
void *growArray(void *, size_t, size_t);
void dumpArray(char **, size_t);
