#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <readline/readline.h>


#define PIPE_READ 0
#define PIPE_WRITE 1
#define MAX_TOKEN_CHAR 40
#define BUF 4000
#define arraySize(x)    sizeof(x)/sizeof(*x)

void cleanPtr(char **);
char **stringToArray(char *, char *);
char **readinput(void);
void xread(int *);
