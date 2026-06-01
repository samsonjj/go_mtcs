
go_mtcs: main.c
	gcc main.c -Wall -o go_mtcs

run: go_mtcs
	./go_mtcs

all: go_mtcs
