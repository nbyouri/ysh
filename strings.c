#include "global.h"

// procedure to read lines from file descriptor
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

// clean array of strings
void cleanPtr(char ** array) {
    int count = arraySize(array);

    if (count > 1) {
        for (int i = 0; i < count; i++) {
            if (array[i] != NULL) {
                free(array[i]);
                array[i] = NULL;
            }
        }
    }

    if (array != NULL) {
        free(array);
        array = NULL;
    }
}

// convert string to array of strings separated by delimiters
char **stringToArray(char *string, char *delims) {
    char    *token;
    char    **array = NULL;
    unsigned int    i = 0;

    while ((token = strsep(&string, delims)) != NULL) {
        array = realloc(array, i + 1);
        if (array == NULL) {
            printf("Failed to reallocate array at index %d\n", i);
            return NULL;
        } else {
            array[i] = malloc(sizeof(char *));
            if (array[i] == NULL) {
                printf("Failed to malloc\n");
                return NULL;
            } else {
                array[i] = token;
                i++;
            }
        }
    }

    // make sure the array of string end with NULL
    array[i] = NULL;

    return array;
}
