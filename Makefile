
BINDIR ?= ${CURDIR}
DEBUG ?=

SRCS = WonkieCalculator.c expression.c printer.c parser.c
HDRS = expression.h printer.h parser.h

all: ${BINDIR}/WonkieCalculator

.PHONY: all

${BINDIR}/WonkieCalculator: ${SRCS} ${HDRS}
	${CC} ${DEBUG} -o $@ ${SRCS}
