SRCS+=	main.c strings.c
TARGET = ysh
CC ?=	cc
CFLAGS ?= -Weverything
LDFLAGS ?= -lreadline	

all:
	${CC} ${CFLAGS} ${SRCS} -o ${TARGET} ${LDFLAGS}

clean:
	rm ${TARGET}
