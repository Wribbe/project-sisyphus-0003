BINS := $(patsubst src/%.c,bin/%,$(wildcard src/*.c))

FLAGS := -g -Wall --pedantic -I.
CC := gcc

all: ${BINS}

bin/% : src/%.c | bin
	${CC} $(filter %.c,$^) -o $@

bin :
	mkdir -p $@
