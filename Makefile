SRCS+=	main.c strings.c
TARGET = ysh
CC =	cc
CFLAGS = -std=c99 -Wall

all:
	${CC} ${CFLAGS} ${SRCS} -o ${TARGET}

clean:
	rm ${TARGET}
