root = /home/srl/Programming/Vulkapetl/
LDFLAGS = -lglfw -lvulkan

default: all

all: graphics include
	gcc $(root)src/main.c $(root)tests/graphics.o $(root)tests/dynarr.o $(root)tests/logger.o -o $(root)tests/test $(LDFLAGS)


graphics:
	gcc -c $(root)src/graphics/graphics.c -o $(root)tests/graphics.o

include: dynarr logger

dynarr:
	gcc -c $(root)src/include/dynarr/dynarr.c -o ${root}tests/dynarr.o
logger:
	gcc -c $(root)src/include/dynarr/dynarr.c -o ${root}tests/logger.o
