CFLAGS = -Wall
DEBUG_FLAGS = -O0 -Wall -Wextra -g -fsanitize=address,undefined

go_mtcs: main.c term.c
	gcc $(CFLAGS) main.c term.c -o go_mtcs

debug:
	gcc $(CFLAGS) $(DEBUG_FLAGS) main.c term.c -o go_mtcs
    

run: go_mtcs
	./go_mtcs

all: go_mtcs
