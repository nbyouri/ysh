#include "global.h"

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
