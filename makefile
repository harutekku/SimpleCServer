vpath %.o bin
vpath %.c src
vpath %.h include

COMPILE.c = clang
CFLAGS = -O3 -std=c2x -std=gnu2x -Wall -Wextra -Wpedantic

program: main.o diagnostics.o
	make --directory=bin/

main.o: main.c diagnostics.h
	$(COMPILE.c) -c $< -o bin/$@ $(CFLAGS)

diagnostics.o: diagnostics.c diagnostics.h
	$(COMPILE.c) -c $< -o bin/$@ $(CFLAGS)

