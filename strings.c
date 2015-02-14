#include "global.h"

// read string from user
char *readinput(const char *prompt) {
    char line[BUFSIZ];

    printf("%s", prompt);

    fgets(line, sizeof(line), stdin);
    if (line != NULL) {
        char *string = malloc(BUFSIZ * sizeof(char *));
        strlcpy(string, line, strlen(line));
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
    if (SIZE_T_MAX / nelem < size) {
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

    // separate the string into tokens based on the delimiters
    // and store these tokens in an array.
    while ((token = strsep(&string, delims)) != NULL) {
        // grow array at index + 1
        array = growArray(array, i + 1, sizeof(char *));
        if (array == NULL) {
            printf("Failed to reallocate array at index %d\n", i);
            return NULL;
        } else {
            // allocate string in array
            array[i] = malloc(BUFSIZ * sizeof(char));
            if (array[i] == NULL) {
                printf("Failed to malloc\n");
                return NULL;
            } else {
                // don't copy an empty string
                if (strlen(token) > 0) {
                    strlcpy(array[i], token, sizeof(array[i]));
                    i++;
                }
            }
        }
    }

    // make sure the array of string end with NULL
    array[i] = NULL;

    // debug array content
    //dump(array, i);

    return array;
}

// dumps the content of a string array with given size
void dump(char **ptr, size_t size) {
    if (ptr == NULL) {
        printf("Can't dump NULL pointer\n");
        return;
    }

    if (size == 0) {
        printf("Can't dump array of no elements\n");
        return;
    }

    printf("args[%zu] = {\n", size + 1);

    unsigned int i = 0;
    for (i = 0; i <= size; i++) {
        printf("\t");
        if (ptr[i] == NULL) {
            printf("NULL,\n");
        } else {
            printf("%s, // (%zu)\n", ptr[i], strlen(ptr[i]));
        }
    }
    printf("};\n");
}
