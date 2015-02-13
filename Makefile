SRCS+=	main.c strings.c
TARGET = ysh
CC =	cc
CFLAGS = -std=c99 -Wall
LDFLAGS = -lreadline	

all:
	${CC} ${CFLAGS} ${SRCS} -o ${TARGET} ${LDFLAGS}

clean:
	rm ${TARGET}
