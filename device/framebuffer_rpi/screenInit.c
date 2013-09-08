/**
 * @file screenInit.c
 *
 * Initializes communication channels between VC and ARM.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <stddef.h>
#include <framebuffer.h>
#include <stdlib.h>
#include <shell.h> /* for banner */
#include <kernel.h>
#include <bcm2835.h>

int rows;
int cols;
int cursor_row;
int cursor_col;
ulong background;
ulong foreground;
ulong linemap[MAPSIZE];
bool minishell;
ulong framebufferAddress;
int pitch;
bool screen_initialized;

/* screenInit(): Calls framebufferInit() several times to ensure we successfully initialize, just in case. */
void screenInit() {
	int i = 0;
    while (framebufferInit() == SYSERR) {
        if ( (i++) == MAXRETRIES) {
            screen_initialized = FALSE;
            return;
        }
    }
	// clear the screen to the background color.
	screenClear(background);
    initlinemap();
    screen_initialized = TRUE;
}

/* Initializes the framebuffer used by the GPU. Returns OK on success; SYSERR on failure. */
int framebufferInit() {
    //GPU expects this struct to be 16 byte aligned
    struct framebuffer frame __attribute__((aligned (16)));

	frame.width_p = DEFAULT_WIDTH; //must be less than 4096
	frame.height_p = DEFAULT_HEIGHT; //must be less than 4096
	frame.width_v = DEFAULT_WIDTH; //must be less than 4096
	frame.height_v = DEFAULT_HEIGHT; //must be less than 4096
	frame.pitch = 0; //no space between rows
	frame.depth = BIT_DEPTH; //must be equal to or less than 32
	frame.x = 0; //no x offset
	frame.y = 0; //no y offset
	frame.address = 0; //always initializes to 0x48006000
	frame.size = 0;

    mailboxWrite((ulong)&frame);

	ulong result = mailboxRead();

	/* Error checking */
	if (result) { //if anything but zero
		return SYSERR;
	}
    if (!frame.address) { //if address remains zero
		return SYSERR;
	}

    /* Initialize global variables */
    framebufferAddress = frame.address;
    rows = frame.height_p / CHAR_HEIGHT;
    cols = frame.width_p / CHAR_WIDTH;
    pitch = frame.pitch;
    cursor_row = 0;
    cursor_col = 0;
    background = BLACK;
    foreground = WHITE;
    minishell = FALSE;
	return OK;
}

/* Very heavy handed clearing of the screen to a single color. */
void screenClear(ulong color) {
	ulong *address = (ulong *)(framebufferAddress);
    ulong *maxaddress = (ulong *)(framebufferAddress + (DEFAULT_HEIGHT * pitch) + (DEFAULT_WIDTH * (BIT_DEPTH / 8)));
    while (address != maxaddress) {
	    *address = color;
        address++;
    }
}

/* Clear the minishell window */
void minishellClear(ulong color) {
    ulong *address = (ulong *)(framebufferAddress + (pitch * (DEFAULT_HEIGHT - (MINISHELLMINROW * CHAR_HEIGHT))) +  (DEFAULT_WIDTH * (BIT_DEPTH / 8)));
    ulong *maxaddress = (ulong *)(framebufferAddress + (DEFAULT_HEIGHT * pitch) + (DEFAULT_WIDTH * (BIT_DEPTH / 8)));
    while (address != maxaddress) {
	    *address = color;
        address++;
    }
}

/* Clear the "linemapping" array used to keep track of pixels we need to remember */
void initlinemap() {
    int i = MAPSIZE;
    while (i != 0) {
        i--;
        linemap[i] = background;
    }
}
