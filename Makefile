root = /home/srl/Programming/Vulkapetl/
LDFLAGS = -lglfw -lvulkan

default: all

all: graphics include
	gcc $(root)src/main.c $(root)tests/graphics.o $(root)tests/log.o $(root)tests/dynint.o $(root)tests/dynstring.o -o $(root)tests/test $(LDFLAGS)


graphics:
	gcc -c $(root)src/graphics/graphics.c -o $(root)tests/graphics.o

include: dynint dynstr log
dyndir = $(root)src/include/dynarr/

log:
	gcc -c $(root)src/include/logger/log.c -o $(root)tests/log.o
dynint:
	gcc -c $(dyndir)dynInt/dynint.c -o $(root)tests/dynint.o
dynstr:
	gcc -c $(dyndir)dynString/dynstring.c -o $(root)tests/dynstring.o

