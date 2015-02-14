#include "global.h"

// read string from user
// XXX: sanitize
// XXX: broken
char *readinput(const char *prompt) {
    char line[BUFSIZ];
    printf("%s", prompt);
    fgets(line, sizeof(line), stdin);
    if (line != NULL) {
        printf("%s\n", line);
        char *string = malloc((sizeof(line) + 1) * sizeof(char *));
        strncpy(string, line, BUFSIZ);
        string[sizeof(string) + 1] = '\0';
        return string;
    } else {
        return NULL;
    }
}

// procedure to read lines from file descriptor
void ReadFD(int *fd) {
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

// grow array of one element of given size
void *growArray(void *ptr, size_t nelem, size_t size) {
    void *tempPtr;
    size_t totalSize = nelem * size;

    if (totalSize == 0) {
        printf("Trying to allocate 0\n");
        return NULL;
    }
    if (SIZE_MAX / nelem < size) {
        printf("Trying to allocate too much\n");
        return NULL;
    }
    if (ptr == NULL) {
        tempPtr = malloc(totalSize);
    } else {
        tempPtr = realloc(ptr, totalSize);
    }
    if (tempPtr == NULL) {
        printf("Failed to reallocate %zu bytes.\n", totalSize);
    }

    return tempPtr;
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
        array = growArray(array, i + 1, sizeof(char *));
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
