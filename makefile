CC=gcc
CFLAGS=-std=c99 -Wall -O2 -g
CLIBS=-lc

install: ostudy

ostudy: main.c 
	$(CC) $(CFLAGS) $(CLIBS) $< -o $@
