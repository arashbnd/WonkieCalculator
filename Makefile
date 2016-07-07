
BINDIR ?= ${CURDIR}
DEBUG ?=

SRCS = WonkieCalculator.c expression.c printer.c parser.c simplify.c evaluate.c
HDRS = expression.h printer.h parser.h simplify.h evaluate.h

all: ${BINDIR}/WonkieCalculator

.PHONY: all

${BINDIR}/WonkieCalculator: ${SRCS} ${HDRS}
	${CC} ${DEBUG} -o $@ ${SRCS}
