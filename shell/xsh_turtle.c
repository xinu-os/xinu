/*
 * @file     xsh_turtle.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread.h>
#include <tty.h>
#include <string.h>
#include <shell.h>
#include <framebuffer.h>
#include <device.h>


/* A turtle has three important features:
 * location, orientation, and pen.
 * The pen itself has color, width, and up/down */

/**
 * Shell command (turtle). Runs a mini-shell of its own until 
 * ended (via 'end' command)
 * @param nargs  number of arguments in args array
 * @param args   array of arguments
 * @return OK at end
 */

#if FRAMEBUF // don't include if no framebuffer exists

/* Global variables used by turtle */
int x, y; //location simulates quadrant 1 in that (0,0) is the bottom left corner instead of the top left corner.
int orientation; //angle in degrees; limited from -359 to +359 for simplicity.
int width; //width of pen line
ulong color; //current pen color
bool up; //controls whether 'pen' is up or down
struct newcommand newcommandtab[MAXNEWCOMMANDS];

//Start turtle in middle of the screen.
//Keep track of where the minishell starts.
#define MAXY (DEFAULT_HEIGHT - MINISHELLMINROW * CHAR_HEIGHT - 2)
#define STARTX (DEFAULT_WIDTH / 2)
#define STARTY (MAXY / 2)

/* Turtle function prototypes */
void reset(void);
void status(void);
void printHelp(void);
void eraseTurtle(int, int, int);
void drawTurtle(int, int, int);
char* colorfinder(ulong);
void move(int);
void turn(int);
void test(void);
void colorset(char*);
void pendrop(char*);
void widthset(char*);
void right(char*);
void left(char*);
void forward(char*);
void backward(char*);
void setx(char*);
void sety(char*);
void home(void);
void drawLineMap(void);
int commandfinder(char*, char*);
int newcommandfinder(int, char*[]);
int repeatcommand(int, int, char*[]);
int def(char[], char[]);
void test(void);

/* built in default colors */
struct defaultcolor colortab[] = {
    {"white", WHITE},
    {"red", RED},
    {"green", GREEN},
    {"blue", BLUE},
    {"black", BLACK},
    {"gray", GRAY},
    {"silver", SILVER},
    {"yellow", YELLOW},
    {"cyan", CYAN},
    {"magenta", MAGENTA},
    {"purple", PURPLE},
    {"darkblue", DARKBLUE},
    {"orange", ORANGE},
    {"brown", BROWN},
    {"raspberry", RASPBERRY},
    {"leafgreen", LEAFGREEN},
    {"darkgreen", DARKGREEN},
    {"indigo", INDIGO},
    {"blueivy", BLUEIVY},
    {"pink", PINK}
};

/* The built-in turtle commands. Can be used in loops and new commands.
 * Not included: 1 arg commands -- end, reset, status, help, home */
struct defaultcommand basicCommands[] = {
    {"forward", forward},
    {"backward", backward},
    {"left", left},
    {"right", right},
    {"width", widthset},
    {"pen", pendrop},
    {"color", colorset},
    {"setx", setx},
    {"sety", sety}
};

int ncolor = sizeof(colortab) / sizeof(struct defaultcolor);
int nbasics = sizeof(basicCommands) / sizeof(struct defaultcommand);

/**
 * @ingroup shell
 */
shellcmd xsh_turtle(int nargs, char *args[])
{
    char buf[SHELL_BUFLEN];     /* line input buffer        */
    short buflen;               /* length of line input     */
    char tokbuf[SHELL_BUFLEN + SHELL_MAXTOK];   /* token value buffer       */
    short ntok;                 /* number of tokens         */
    char *tok[SHELL_MAXTOK];    /* pointers to token values */

    /* Run a minimized version of the shell so that it isn't taking up the whole screen */
    minishell = TRUE;
    reset();
   
    // constantly receive and process input. 
    while(TRUE) {
        printf("\nxturtle$ ");
        /* Receive and parse input. */
        control(stdin, TTY_CTRL_CLR_IFLAG, TTY_IRAW, NULL);
        control(stdin, TTY_CTRL_SET_IFLAG, TTY_ECHO, NULL);
        buflen = read(stdin, buf, SHELL_BUFLEN - 1);
        if ( (SYSERR == (ntok = lexan(buf, buflen, &tokbuf[0], &tok[0]))) || ntok <= 0)
        {
            printf("Sorry, command not recognized. Type 'help' for a list of available commands.");
            continue;
        }
        //type end to break.
        if ( (0 == strncmp(tok[0], "end", 3)) && (ntok == 1) ) break;

        //display a help menu that explains available colors and commands
        else if ((0 == strncmp(tok[0], "help", 4)) && (ntok == 1)) {
            printHelp();
            continue;
        } 

        //reset back to beginning state
        else if ((0 == strncmp(tok[0], "reset", 5)) && (ntok == 1)) {
            reset();
            continue;
        }

        //go back to (0, 0)
        else if ((0 == strncmp(tok[0], "home", 4)) && (ntok == 1)) {
            home();
            continue;
        }

        //display all current turtle variables
        else if ((0 == strncmp(tok[0], "status", 6)) && (ntok == 1)) {
            status();
            continue;
        }

        //built-in tests
        else if ((0 == strncmp(tok[0], "test", 4)) && (ntok == 1)) {
            test();
            continue;
        }

        //define a new command. eg, 'define line forward 100'
        else if (0 == strncmp(tok[0], "define", 6)) {
            char *text = "";
            bzero(text, COMMANDLENGTH);
            int i;
            for (i = 2; i < ntok; i++) {
                text = strncat(strncat(text, tok[i], COMMANDNAMELENGTH), " ", strlen(text));
            }
            if (strlen(text) > COMMANDLENGTH) {
                printf("Error defining command: Command too long.");
                continue;
            }
            if (def(tok[1], text) == SYSERR) { 
                printf("Error defining command. Does it already exist?");
            }
            continue;
        }

	    /* compare to default commands. */
	    else if ((ntok == 2) && (commandfinder(tok[0], tok[1]) == OK)) continue;

        /* compare to new commands */
	    else if ((ntok == 1) && (newcommandfinder(ntok, tok) == OK)) continue;

	    /* Handle repeat commands: repeat n [command n*] */
	    else if (0 == strncmp(tok[0], "repeat", 6) ) {
            repeatcommand(atoi(tok[1]), ntok, tok);
            continue;
    } 

    /* If we made it here, we have no idea what this command is. */
    printf("Sorry, I don't know how to %s. Check syntax and help menu.", tok[0]);

    }

    minishell = FALSE;
    initlinemap();
    cursor_col = 1;
    cursor_row = 0;
    screenClear(background);
    return OK;

}

/* Define a new command */
int def(char name[COMMANDNAMELENGTH], char text[COMMANDLENGTH]) {
    int i;
    for (i = 0; i < MAXNEWCOMMANDS; i++) {
        //check that the name doesn't already exist
        if (0 == strncmp(newcommandtab[i].name, name, COMMANDNAMELENGTH)) {
            return SYSERR;
        }
        if (0 == strncmp(newcommandtab[i].name, "NULL", COMMANDNAMELENGTH)) {
            strncpy(newcommandtab[i].name, name, COMMANDNAMELENGTH);
            strncpy(newcommandtab[i].text, text, COMMANDLENGTH);
            printf("New command, %s, defined.", newcommandtab[i].name);
            return OK;
        }
    }
    return SYSERR; //no space in table
}

/* Iterate through default command table */
int commandfinder(char *arg0, char *arg1) {
	int i;
    for (i = 0; i < nbasics; i++)
    {
        if (0 == strncmp(basicCommands[i].commandname, arg0, COMMANDNAMELENGTH))
        {
            break;
        }
    }

    if (i >= nbasics)
    {
        return SYSERR;
    }
    /* Run command! */
    (*basicCommands[i].command) (arg1);
	return OK;
}

/* handle repeats */
/* args: iterations, number of args, list of args */
int repeatcommand(int num, int nargs, char *args[]) {
    int i;
    while (num != 0) {
        i = 2;
        while (i < nargs) {
		    if (commandfinder(args[i], args[i + 1]) == OK) {
                i += 2;
            } else if (newcommandfinder(nargs - i, args + i) == OK) {
                i++;
            } else {
                printf("Sorry, I don't know how to %s", args[i]);
                return SYSERR;
            }
        }
        num--;
    }
    return OK;
}

void test() {

}

/* Iterate through new command table */
int newcommandfinder(int nargs, char *args[]) {
    int i, ntok;
    char tokbuf[SHELL_BUFLEN + SHELL_MAXTOK];   
    char *tok[SHELL_MAXTOK];    
    int j = 0;
    for (i = 0; i < MAXNEWCOMMANDS; i++) {
        if (0 == strncmp(newcommandtab[i].name, "NULL", 4)) {
            return SYSERR; //command not in table
        }
        else if (0 == strncmp(newcommandtab[i].name, args[0], COMMANDNAMELENGTH)) {
            //parse and check for errors
            if ( (SYSERR == (ntok = lexan(newcommandtab[i].text, COMMANDLENGTH, &tokbuf[0], &tok[0]))) || ntok <= 0) {
                return SYSERR;
            }
            while (ntok != 0) {
                if (0 == strncmp(tok[j], "repeat", 6) ) {
                    repeatcommand(atoi(tok[j+1]), ntok, tok + j);
                    return OK;
                } else if (commandfinder(tok[j], tok[j + 1]) == OK) {
                    j += 2;
                    ntok -= 2;
                } else if (newcommandfinder(ntok, tok + j) == OK) {
                    j++;
                    ntok--;
                } else {
                    printf("Sorry, I don't know how to %s %s", tok[j], tok[j+1]);
                    return SYSERR;
                }
            }
            return OK;
        }
    }
    return OK;
}

/* turn right by x degrees */
void right(char *arg) {
    turn(-1 * atoi(arg));    
}

/* turn left by x degrees */
void left(char *arg) {
    turn(atoi(arg));
}

/* move forward by x pixels */
void forward(char *arg) {
    move(atoi(arg));
}

/* move backward by x pixels */
void backward(char *arg) {
    move(-1 * atoi(arg));
}

/* move to a specified x location */
void setx(char *arg) {
    int newx = atoi(arg);
    if ((newx > 5000) || (newx < -5000)) {
        printf("New x value out of bounds.");
    }
    else {
        newx += STARTX; //adjust for abstracted coordinates
        eraseTurtle(x, y, orientation);
        if (!up) drawSegment(x, y, newx, y, color);
        x = newx;
        drawTurtle(x, y, orientation);
    }
}

/* move to a specified y location */
void sety(char *arg) {
    int newy = atoi(arg);
    if ((newy > 5000) || (newy < -5000)) {
        printf("New y value out of bounds.");
    }
    else {
        newy = STARTY - newy; //adjust for abstracted coordinates 
        eraseTurtle(x, y, orientation);
        if (!up) drawSegment(x, y, x, newy, color);
        y = newy;
        drawTurtle(x, y, orientation);
    }
}

/* change to a new color */
void colorset(char *newcolor) {
    //for custom hex colors (eg 'color 0xffffff')
    if (0 == strncmp(newcolor, "0x", 2)) {
        ulong hexcolor = 0;
        //chop off the 0x
        sscanf(newcolor + 2, "%x", &hexcolor);
        color = colorconvert(hexcolor);
        //printf("New line color: 0x%x", hexcolor);
    } else {
        /* iterate through default colors to match string */
        ulong tempcolor = ERRORCOLOR;
        int i;
        for (i = 0; i < ncolor; i++) {
            if (0 == strncmp(newcolor, colortab[i].colorname, 
            strlen(colortab[i].colorname))) {
                tempcolor = colortab[i].colornum;
            }
            //if you pick an invalid color, complain.
        } if (tempcolor == ERRORCOLOR)
            printf("Invalid color provided. See help menu.");
        else {
            color = tempcolor;
            //printf("New line color: %s", newcolor);
        }
    } 
}

/* change pen width */
void widthset(char *newwidth) {
    int i = atoi(newwidth);
    if ((i <= 0) || (i > 50)) {
        printf("Invalid width provided!");
    } else {
        width = i;
    }
}

/* put pen down or lift pen up */
void pendrop(char *newpos) {
    if (0 == strncmp(newpos, "up", 2) ) {
        up = TRUE;
    } else if (0 == strncmp(newpos, "down", 2) ) {
        up = FALSE;
    } else printf("Invalid pen arguments provided.");
}

/* helper function for iterating through colortab */
char* colorfinder(ulong color) {
    int i; 
    for (i = 0; i < ncolor; i++) {
         if (color == colortab[i].colornum) 
            return colortab[i].colorname;
    }
    return "custom";
}

/* to reduce repeated code for right/left */
void turn(int num) {
    eraseTurtle(x, y, orientation);
    orientation = (orientation + num) % 360;
    drawTurtle(x, y, orientation);
}

/* to reduce repeated code for forward/backward */
void move(int num) {

    int i;

    /* Limit x/y to prevent overflow issues */
    if ( (num >= 5000) || (num <= -5000) ) {
        printf("Coordinates out of bounds.");
        return;
    }

    // calculate new x, y coordinates using Taylor series sine and cosine.
    int newx = x + num * cos(orientation);
    int newy = y + num * sin(orientation); 
    if (newx >= 5000 || newx <= -5000 || newy >= 5000 || newy <= -5000) {
        printf("Coordinates out of bounds.");
        return;
    }
    eraseTurtle(x, y, orientation); //erase the old turtle
    if (up == FALSE) { // if pen is down, draw a line from old point to new

        if (width == 1) drawSegment(x, y, newx, newy, color);
        else {
            for (i = 0; i < width/2; i+=2) {
                if ((orientation % 180) == 0) {//horizontal line
                    drawSegment(x, y + i, newx, newy + i, color);
                    drawSegment(x, y - i, newx, newy - i, color);
                } else if ((orientation % 90) == 0) { //vertical line; 0 & 180 already taken
                    drawSegment(x + i, y, newx + i, newy, color);
                    drawSegment(x - i, y, newx - i, newy, color);
                } else if ( //positive line
                    ((orientation < 0) && (orientation > -270)) ||
                    ((orientation < 90) && (orientation > 0)) ||
                    ((orientation < -90) && (orientation > -180)) ||
                    ((orientation < 270) && (orientation > 180)) ) {
                    drawSegment(x - i, y - i, newx - i, newy - i, color);
                    drawSegment(x + i, y + i, newx + i, newy + i, color);
                } else { //negative line
                    drawSegment(x + i, y - i, newx + i, newy - i, color);
                    drawSegment(x - i, y + i, newx - i, newy + i, color);
                }
            }
        }
    }
    x = newx;
    y = newy;
    drawTurtle(x, y, orientation); //draw the new turtle
}

/* reset back to beginning conditions */
void reset() {
    //clear out any added commands
    int i;
    for (i = 0; i < MAXNEWCOMMANDS; i++) {
        strncpy(newcommandtab[i].name, "NULL", COMMANDNAMELENGTH);
        strncpy(newcommandtab[i].text, "NULL", COMMANDLENGTH);
    }
    screenClear(background);
    initlinemap();
    color = LEAFGREEN;
    up = FALSE;
    width = 1;
    x = STARTX;
    y = STARTY;
    orientation = 0;
    //draw a rectangle around the entire screen so we can see limits
    drawSegment(0, 0, DEFAULT_WIDTH-1, 0, RASPBERRY);
    drawSegment(0, 0, 0, MAXY, RASPBERRY);
    drawSegment(0, MAXY, DEFAULT_WIDTH-1, MAXY, RASPBERRY);
    drawSegment(DEFAULT_WIDTH-1, 0, DEFAULT_WIDTH-1, MAXY, RASPBERRY);
    //draw initial turtle.    
    drawTurtle(x, y, orientation);
    cursor_row = rows - MINISHELLMINROW;
    cursor_col = 1;
    status();
}

/* from wherever Squirt is onscreen, return to (0,0) */
void home() {
    eraseTurtle(x, y, orientation);
    if (!up) drawSegment(x, y, STARTX, STARTY, color);
    x = STARTX;
    y = STARTY;
    orientation = 0;
    drawTurtle(x, y, orientation);
}

/* print current turtle variables */
void status() {

    char *str = "";
    if (up) str = "up";
    else str = "down";
    printf("STATUS: Current coordinates: (%d, %d). Current width: %d. Current angle: %d. Current color: %s. Pen is %s.", x - STARTX, STARTY - y, width, orientation, colorfinder(color), str);
}

/* helper function to erase turtle from screen */
void eraseTurtle(int x0, int y0, int orientation) {
    if (y0 > (MAXY - HEAD_RADIUS - BODY_RADIUS)) return;
    int body_x = x0 + BODY_RADIUS * cos(orientation);
    int body_y = y0 + BODY_RADIUS * sin(orientation);  
    int head_x = body_x + BODY_RADIUS * cos(orientation);
    int head_y = body_y + BODY_RADIUS * sin(orientation);
    fillBody(head_x, head_y, HEAD_RADIUS, background, FALSE);
    fillBody(body_x, body_y, BODY_RADIUS, background, FALSE);
}

/* draw turtle in new location */
void drawTurtle(int x0, int y0, int orientation) {
    if (y0 > (MAXY - HEAD_RADIUS - BODY_RADIUS)) return;
    int body_x = x0 + BODY_RADIUS * cos(orientation);
    int body_y = y0 + BODY_RADIUS * sin(orientation);  
    int head_x = body_x + BODY_RADIUS * cos(orientation);
    int head_y = body_y + BODY_RADIUS * sin(orientation);
    fillBody(head_x, head_y, HEAD_RADIUS, TURTLE_HEAD, FALSE);
    fillBody(body_x, body_y, BODY_RADIUS, TURTLE_BODY, TRUE);
} 


void printHelp() {
    int i;
    screenClear(background);
    minishell = FALSE;
    cursor_row = 0;
    cursor_col = 0;
    printf("Available Commands: \n");
    printf("\tforward [number of pixels]: moves turtle forward\n"); 
    printf("\tbackward [number of pixels]: moves turtle backward\n");
    printf("\tright [number of degrees]: turns turtle right\n");
    printf("\tleft [number of degrees]: turns turtle left\n");
    printf("\tpen ['up' or 'down']: lift or put down pen\n");
    printf("\twidth [number of pixels]: change pen width\n");
    printf("\treset: clear screen and start again\n");
    printf("\tcolor [color name]: change pen color\n");
    printf("\tcolor [0xXXXXXX]: change to custom pen color\n");
    printf("\trepeat [number] [command argument]*: repeat a list of commands with given arguments n times\n");
    printf("\tdefine [name] [string]: teach the turtle a new word\n");
    printf("\thome: return to (0,0)\n");
    printf("\tsetx [coordinate]: move to a specified x coordinate\n");
    printf("\tsety [coordinate]: move to a specified y coordinate\n");
    printf("\tstatus: show current turtle information\n");
    printf("\tend: quit Turtle Graphics.\n");
    printf("\thelp: display this screen.\n");
    printf("\nCustom Commands: \n");
    for (i = 0; i < MAXNEWCOMMANDS; i++) {
        if (0 == strncmp(newcommandtab[i].name, "NULL", 4)) break;
        printf("\t%s: %s\n", newcommandtab[i].name, newcommandtab[i].text);
    }
    printf("\nAvailable Colors: ");
    for (i = 0; i < ncolor; i++) {
        printf("\n\t%s", colortab[i].colorname);
    } 
    printf("\n\nExample Usage of define:\n");
    printf("\tdefine square width 10 color red repeat 4 forward 100 left 90\n");
    printf("\t This will teach the turtle that the word \"square\" means to draw a red width 10 square of size 100.\n");
    printf("\tdefine move pen up setx 100 sety 100 pen down\n");
    printf("\t This will teach the turtle that the word \"move\" means to pick his pen up and move to (100, 100), then put the pen down.\n");

    printf("\nHit 'Enter' to continue...");
    while (TRUE) {
        if (getchar()) break;
    }
    /* Redraw turtle as it was before. */
    screenClear(background);
    drawLineMap();
    minishell = TRUE;
    drawTurtle(x, y, orientation);
    cursor_row = rows - MINISHELLMINROW - 1;
    cursor_col = 1;
}


void drawLineMap() {
    //view current linemap
    ulong *address = (ulong *)(framebufferAddress);
    ulong *maxaddress = (ulong *)(framebufferAddress + (pitch * DEFAULT_HEIGHT) + (DEFAULT_WIDTH * BIT_DEPTH / 8));

    int i = 0;
    while (address != maxaddress) {
	    *address = linemap[i];
        i++;
        address++;
    }
}

#endif /* FRAMEBUF */
