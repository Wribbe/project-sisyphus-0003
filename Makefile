BINS := $(patsubst src/%.c,bin/%,$(wildcard src/*.c))

FLAGS := -g -Wall --pedantic -I. -lX11 -pthread -lpulse
CC := gcc

all: ${BINS}

bin/% : src/%.c Makefile | bin
	${CC} $(filter %.c,$^) -o $@ ${FLAGS}

bin :
	mkdir -p $@
