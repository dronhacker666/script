CC=gcc
SOURCES=*.c
LIBS=
PARAMS= -s
OUT=run

all:
	${CC} ${SOURCES} ${LIBS} ${PARAMS} -o ${OUT}

	${OUT}