#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <string.h>
#include "strings.c"

// don't bother with arguments
#define PATH "/Users/youri/Downloads"

// entry
static struct entry {
	unsigned int id;
	char *name;
	char diff;
	TAILQ_ENTRY(entry) entries;
} *el0, *np;

static TAILQ_HEAD(tailhead, entry) head = TAILQ_HEAD_INITIALIZER(head);

char *toString(struct entry *);
struct entry *get(unsigned int);
struct entry *getPrev(struct entry *);
struct entry *getNext(struct entry *);
char *getName(struct entry *);
ssize_t setName(char *, struct entry *);
size_t getSize(void);
int cmpName(struct entry *, struct entry *);
void swapNext(struct entry *);
unsigned int toArray(char ***);
void cleanDaPtr(char **, unsigned int *);

unsigned int toArray(char ***array) {
	*array = NULL;
	unsigned int i = 0;
	*array = growArray(*array, getSize(), sizeof(char *));
	if (*array == NULL) {
		perror("*array == NULL");
	}
	TAILQ_FOREACH(np, &head, entries) {
		(*array)[i] = malloc(BUFSIZ);
		(*array)[i] = np->name;
		i++;
	}
	return i;
}

char *toString(struct entry *en) {
	char *string;
	string = malloc(BUFSIZ);
	snprintf(string, BUFSIZ, "%u -> %s", en->id, en->name);
	return string;
}

struct entry *get(unsigned int i) {
	TAILQ_FOREACH(np, &head, entries) {
		if (np->id == i) {
			return np;
		}
	}
	return NULL;
}

struct entry *getPrev(struct entry *en) {
	struct entry *prev;
	prev =  TAILQ_PREV(en, tailhead, entries);
	return prev;
}

struct entry *getNext(struct entry *en) {
	struct entry *next;
	next = TAILQ_NEXT(en, entries);
	return next;
}

char *getName(struct entry *en) {
	return (en->name);
}

ssize_t setName(char *name, struct entry *en) {
	if (en->name == NULL) {
		en->name = malloc(BUFSIZ);
	}
	if (strnlen(name, BUFSIZ) > 0) {
		return (ssize_t)strlcpy(en->name, name, BUFSIZ);
	} else {
		return -1;
	}
}

size_t getSize(void) {
	size_t size = 0;
	TAILQ_FOREACH(np, &head, entries) {
		size++;
	}
	return size;
}

int cmpName(struct entry *base, struct entry *next) {
	return (strcmp(base->name, next->name));
}

void swapNext(struct entry *base) {
	struct entry *next = TAILQ_NEXT(base, entries);
	// place the first after the next
	TAILQ_INSERT_AFTER(&head, next, base, entries);
	// remove the first
	TAILQ_REMOVE(&head, base, entries);
}

int main(void) {
	DIR                 *dp;
	struct dirent       *ep;

	// get the size of one file name, platform dependant
	size_t namelen = sizeof(ep->d_name);
	unsigned int id = 0;

	// initialize list
	TAILQ_INIT(&head);

	// get a file listing from PATH
	if ((dp = opendir(PATH)) == NULL) {
		perror("Failed to opendir");
		return -1;
	} else {
		while ((ep = readdir(dp)) != NULL) {
			if ((ep->d_name[0] != '.') && (strnlen(ep->d_name,
					    namelen) > 0)) {
				// we've got data to store in our linked list,
				// malloc a pointer for an item.
				el0 = malloc(sizeof(struct entry));
				if (el0 == NULL) {
					perror("Failed to malloc\n");
					return -1;
				} else {
					// malloc a string for a file listing,
					// think `ls`.
					el0->name = malloc(namelen);
					if (el0->name == NULL) {
						perror("failed to malloc\n");
						return -1;
					} else {
						// copy the string in our item
						strlcpy(el0->name, ep->d_name,
						    namelen);
						// insert the entry in the list
						TAILQ_INSERT_TAIL(&head, el0,
						    entries);
						el0->id = id++;
					}
				}
			}
		}
		if (closedir(dp) == -1) {
			perror("Failed to close dir pointer");
			return -1;
		}
	}

	// swap last and last - 1 elements
	// print traversal
	TAILQ_FOREACH(np, &head, entries) {
		struct entry *next = TAILQ_NEXT(np, entries);
		size_t nextsize = 0;
		size_t size = strlen(np->name);
		if (next != NULL) {
			nextsize = strlen(next->name);
			np->diff = size > nextsize ? '>' : '<';
		} else {
			np->diff = '-';
		}
		printf("%02u : %s (%c : %zu|%zu)\n", np->id, np->name,
		    np->diff, size, nextsize);
	}

	// get an entry like an array
	struct entry *item = get(5);
	printf("5 > %s\n", toString(item));
	printf("%s\n", "renaming entry 5 name to \"remy\"");
	if (setName("remy", item) == -1) {
		perror("Failed to set name\n");
	} else {
		printf("prev > %s\n", toString(getPrev(item)));
		printf("%s\n", toString(item));
		printf("next > %s\n", toString(getNext(item)));
	}


	// convert linkedlist to static array
	//char **array = NULL;
	//for (int i = 0; i < 61; i++) {
	//	memset(&array[i], 0, sizeof(array));
	//	printf("%u > nulling array[%u]\n", i, i);
	//}
	char **array;
	unsigned count = toArray(&array);
	dumpArray(array, count);
	cleanPtr(array, &count);

	// bubble sort
#if 0
	size_t s = getSize();
	for (int i = 0; i < s; i++) {
		for (int j = 0; j < (s - 1); j++) {
			if (cmpName(get(j), get(j + 1)) > 0) {
				puts("bigger");
			} else {
				puts("smaller");
			}
		}
	}
#endif

	// free allocated memory for each entry
	// and remove them.
	TAILQ_FOREACH(np, &head, entries) {
		if (np->name != NULL) {
			free(np->name);
			np->name = NULL;
		}
		TAILQ_REMOVE(&head, np, entries);
	}

	// should be empty by now, still check
	if (!TAILQ_EMPTY(&head)) {
		perror("empty\n");
	}

	// free entry pointers
	if (el0 != NULL) {
		free(el0);
		el0 = NULL;
	}

	if (np != NULL) {
		free(np);
		np = NULL;
	}

	return EXIT_SUCCESS;
}
