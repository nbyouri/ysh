#include <assert.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "queue.h"
#include <string.h>
#include "strings.c"

/*
 * TOOD: - add concat
 *	 - simplify head/init with macros
 *	 - cleanup
 *	 - man page
 *	 - warn about not using get() twice in parameters
 *	 - ...
 */
// don't bother with arguments
#define PATH "/Users/youri/Downloads"
#define RED "\033[22;32m"
#define NOR "\033[00;0m"


// entry
static struct entry {
	unsigned int id;
	char *name;
	/* pointer to previous and next entries in the tail queue */
	TAILQ_ENTRY(entry) entries;
} *np;

// tail queue head
static TAILQ_HEAD(tailhead, entry) head = TAILQ_HEAD_INITIALIZER(head);

// Traverse the tail queue in forward direction
#define foreach(x)	TAILQ_FOREACH(x, &head, entries)
//#define foreach_rev(x)	TAILQ_FOREACH_REVERSE(x, &head, tailhead, entries)

char *toString(struct entry *);
void add(struct entry *);
void addBefore(struct entry *, struct entry *);
void addAfter(struct entry *, struct entry *);
void addOnTop(struct entry *);
void rm(struct entry *);
struct entry *new(unsigned int id, char *name);
struct entry *get(unsigned int);
struct entry *getFirst(void);
struct entry *getLast(void);
struct entry *getPrev(struct entry *);
struct entry *getNext(struct entry *);
void set(struct entry *, struct entry *);
void setPrev(struct entry *, struct entry *);
void setNext(struct entry *, struct entry *);
char *getName(struct entry *);
ssize_t setName(char *, struct entry *);
unsigned int getId(struct entry *);
size_t getSize(void);
int cmpName(struct entry *, struct entry *);
int cmpId(struct entry *, struct entry *);
void swap(struct entry *, struct entry *);
void swapPrev(struct entry *);
void swapNext(struct entry *);
void *toArray(char **);
bool isEmpty(void);
void freeList(void);

void *toArray(char **array) {
	unsigned int i = 0;
	array = growArray(array, getSize(), sizeof(char *));
	if (array == NULL) {
		perror("*array == NULL");
	}
	foreach (np) {
		array[i] = growArray(array[i], BUFSIZ, sizeof(char));
		strlcpy(array[i], np->name, BUFSIZ);
		i++;
	}
	return array;
}

char *toString(struct entry *en) {
	char *string = NULL;
	string = growArray(string, BUFSIZ, sizeof(char));
	snprintf(string, BUFSIZ, "%02u -> %s", en->id, en->name);
	return string;
}

void freeList() {
	foreach (np) {
		free(np->name);
		np->name = NULL;
		rm(np);
	}
}

void add(struct entry *en) {
	assert(en != NULL);
	TAILQ_INSERT_TAIL(&head, en, entries);
}

void addOnTop(struct entry *en) {
	assert(en != NULL);
	TAILQ_INSERT_HEAD(&head, en, entries);
}

void addBefore(struct entry *base, struct entry *new) {
	assert(base != NULL && new != NULL);
	TAILQ_INSERT_BEFORE(base, new, entries);
}

void addAfter(struct entry *base, struct entry *new) {
	assert(base != NULL && new != NULL);
	TAILQ_INSERT_AFTER(&head, base, new, entries);
}

void rm(struct entry *en) {
	assert(en != NULL);
	TAILQ_REMOVE(&head, en, entries);
}

struct entry *new(unsigned int id, char *name) {
	struct entry *en = NULL;
	en = growArray(en, 1, sizeof(*en));
	en->id = id;
	en->name = growArray(en->name, BUFSIZ, sizeof(*(en->name)));
	strlcpy(en->name, name, BUFSIZ);
	return en;
}

struct entry *get(unsigned int i) {
	foreach (np) {
		if (np->id == i) {
			return np;
		}
	}
	return NULL;
}

struct entry *getFirst(void) {
	return TAILQ_FIRST(&head);
}

struct entry *getLast(void) {
	return TAILQ_LAST(&head, tailhead);
}

struct entry *getPrev(struct entry *en) {
	assert(en != NULL);
	struct entry *prev;
	prev =  TAILQ_PREV(en, tailhead, entries);
	return prev;
}

struct entry *getNext(struct entry *en) {
	assert(en != NULL);
	struct entry *next;
	next = TAILQ_NEXT(en, entries);
	if (next == NULL) {
		printf("end of list\n");
	}
	return next;
}

void set(struct entry *base, struct entry *en) {
	assert(base != NULL && en != NULL);
	if (en->name != NULL) {
		strlcpy(base->name, en->name, BUFSIZ);
	}
	base->id = en->id;
}

void setPrev(struct entry *base, struct entry *en) {
	assert(base != NULL && en != NULL);
	struct entry *prev = getPrev(base);
	if (prev != NULL) {
		set(prev, en);
	}
}

void setNext(struct entry *base, struct entry *en) {
	assert(base != NULL && en != NULL);
	struct entry *next = getNext(base);
	if (next != NULL) {
		set(next, en);
	}
}

char *getName(struct entry *en) {
	return (en->name);
}

unsigned int getId(struct entry *en) {
	assert(en != NULL);
	return en->id;
}

#if 0
int cmpId(struct entry *en1, struct entry *en2) {
	assert(en1 != NULL && en2 != NULL);
	return (en2->id - en1->id);
}
#endif

ssize_t setName(char *name, struct entry *en) {
	assert(en != NULL && name != NULL);
	if (en->name == NULL) {
		en->name = growArray(en->name, BUFSIZ, sizeof(char));
	}
	if (strlen(name) > 0) {
		return (ssize_t)strlcpy(en->name, name, BUFSIZ);
	}
	return -1;
}

size_t getSize(void) {
	size_t size = 0;
	foreach (np) {
		size++;
	}
	return size;
}

bool isEmpty(void) {
	return TAILQ_EMPTY(&head);
}

int cmpName(struct entry *base, struct entry *next) {
	assert(base != NULL && next != NULL);
	return (strcmp(base->name, next->name));
}

void swap(struct entry *first, struct entry *second) {
	assert(first != NULL && second != NULL);
	if (first->id != second->id) {
		struct entry *temp = first;
		printf("first: %s, second: %s, temp: %s\n",
		    toString(first), toString(second), toString(temp));
		set(first, second);
		//set(second, temp);
		printf("first: %s, second: %s, temp: %s\n",
		    toString(first), toString(second), toString(temp));
	}
}

void swapNext(struct entry *base) {
	assert(base != NULL);
	struct entry *next = getNext(base);
	struct entry *temp = base;
	// place the first after the next
	if (next == NULL) {
		printf("trying to swap last element.\n");
		return;
	}
	// remove the base entry and insert
	rm(base);
	addAfter(next, temp);
}

void swapPrev(struct entry *base) {
	assert(base != NULL);
	struct entry *prev = getPrev(base);
	struct entry *temp = base;
	if (prev == NULL) {
		printf("trying to swap first element.\n");
		return;
	}
	rm(base);
	addBefore(prev, temp);
}

int main(void) {
	DIR                 *dp;
	struct dirent       *ep;

	// get the size of one file name, platform dependant
	unsigned int id = 0;

	// initialize list
	TAILQ_INIT(&head);

	// get a file listing from PATH
	if ((dp = opendir(PATH)) == NULL) {
		perror("Failed to opendir");
		return -1;
	} else {
		while ((ep = readdir(dp)) != NULL) {
			if ((ep->d_name[0] != '.')) {
				add(new(id++, ep->d_name));
			}
		}
		if (closedir(dp) == -1) {
			perror("Failed to close dir pointer");
			return -1;
		}
	}

	// swap last - 1 and last - 2 elements
#if 0
	swapNext(get(59));
	swapNext(get(59));
	swapPrev(get(59));
#endif
	struct entry *first = get(40);
	struct entry *second = get(38);
	swap(first, second);

#if 0
	// replace next/prev entries
	struct entry e;
	e.name = growArray(e.name, BUFSIZ, sizeof(char));
	strlcpy(e.name, "youri", BUFSIZ);
	e.id = 99;
	setPrev(get(61), &e);
	//setNext(get(50), &e);
	//set(get(23), &e);
#endif


	// insert before / after
#if 0
	struct entry b, d;
	b.name = growArray(b.name, BUFSIZ, sizeof(char));
	d.name = growArray(d.name, BUFSIZ, sizeof(char));
	d.id = 11;
	b.id = 99;
	strlcpy(b.name, "youri", BUFSIZ);
	strlcpy(d.name, "kekek", BUFSIZ);
	addBefore(getLast(), &b);
	addAfter(getLast(), &d);
#endif

	// print contents
	foreach (np) {
		printf("%s\n", toString(np));
	}

	// get first / last entry
#if 0
	printf("%s\n", toString(getFirst()));
	printf("%s\n", toString(getLast()));
#endif

	// get an entry like an array
#if 0
	struct entry *item = get(5);
	printf("5 > %s\n", toString(item));
	printf("%s\n", "renaming entry 5 name to \"arst\"");
	if (setName("arst", item) == -1) {
		perror("Failed to set name\n");
	} else {
		printf("%s\n", toString(item));
	}
#endif


	// convert linkedlist to array
#if 0
	char **array = NULL;
	array = toArray(array);
	unsigned int count = (unsigned int)getSize();
	dumpArray(array, count);
	cleanPtr(array, &count);
#endif

	// bubble sort
	//struct entry *en = NULL;
	//for (int i = 0; i < getSize(); i++) {
	//	en = get(i);
	//	if (cmpId(en, getPrev(en)) > 0) {
	//		puts("bigger");
	//	} else {
	//		puts("smaller");
	//	}
	//}
	//en = NULL;

	// free allocated memory for each entry
	// and remove them.
	freeList();

	// should be empty by now, still check
	if (!isEmpty()) {
		perror("ll not empty\n");
	}

	// free entry pointer
	free(np);
	np = NULL;

	return EXIT_SUCCESS;
}
