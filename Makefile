CC     = gcc
CFLAGS = -O3 -Wall
SDL    = $(shell sdl2-config --cflags --libs)

ant: ant.c
	$(CC) $(CFLAGS) ant.c -o ant $(SDL)

run: ant
	./ant

clean:
	rm -f ant

.PHONY: run clean
