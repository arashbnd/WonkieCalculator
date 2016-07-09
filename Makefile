
BINDIR ?= ${CURDIR}
DEBUG ?=
CFLAGS ?= -std=c11 -Wall

SRCS = WonkieCalculator.c

all: ${BINDIR}/WonkieCalculator

.PHONY: all

${BINDIR}/WonkieCalculator: ${SRCS}
	${CC} ${CFLAGS} ${DEBUG} -o $@ ${SRCS}
