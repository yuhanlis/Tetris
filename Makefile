CC  = arm-linux-gcc

all:Teris
Teris:Teris.c game.h game.c
	$(CC) -o $@ Teris.c game.h game.c