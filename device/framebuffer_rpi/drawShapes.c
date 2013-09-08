/**
 * @file drawShapes.c
 *
 * Allows shapes to be rendered onscreen.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <stddef.h>
#include <string.h>
#include <framebuffer.h>
#include <bcm2835.h>

/* Draws a colored pixel at given (x, y) coordinates. */

void drawPixel(int x, int y, ulong color)
{

	//check if somebody tried to draw something out of range.
	if ( (y < DEFAULT_HEIGHT) && (x < DEFAULT_WIDTH) && (y >= 0) && (x >= 0) ) {
	    pre_peripheral_write_mb();

	    // compute address of pixel to write.
	    // framebuffer address + (y*pitch)+(x*(depth/8))
	    volatile ulong *address = (volatile ulong *)(framebufferAddress +
                                                     (y * pitch) +
                                                     (x * (BIT_DEPTH/8)));
        *address = color;

	    post_peripheral_write_mb();
    }
}

/* Line drawing based on Bresenham's Algorithm so that we don't need
 * to mess with division or decimals. */
void drawLine(int x1, int y1, int x2, int y2, ulong color)
{
	int deltax, stepx, deltay, stepy, error;
	if (x2 > x1) {
		deltax = x2 - x1;
		stepx = 1;
	} else {
		deltax = x1 - x2;
		stepx = -1;
	}
	if (y2 > y1) {
		deltay = y2 - y1;
		stepy = 1;
	} else {
		deltay = y1 - y2;
		stepy = -1;
	}
	error = deltax - deltay;
	while ( (x1 != x2 + stepx) && (y1 != y2 + stepy) ) {
		drawPixel(x1, y1, color);
		if (error * 2 >= -1 * deltay) {
			x1 = x1 + stepx;
			error = error - deltay;
		}
		if (error * 2 <= deltax) {
			y1 = y1 + stepy;
			error = error + deltax;
		}
	}
}

/* Modified drawLine specifically for turtle graphics */
void drawSegment(int x1, int y1, int x2, int y2, ulong color)
{
    int deltax, stepx, deltay, stepy, error, i;
    int maxy = DEFAULT_HEIGHT - (CHAR_HEIGHT * MINISHELLMINROW) - 1;
	if (x2 > x1) {
		deltax = x2 - x1;
		stepx = 1;
	} else {
		deltax = x1 - x2;
		stepx = -1;
	}
	if (y2 > y1) {
		deltay = y2 - y1;
		stepy = 1;
	} else {
		deltay = y1 - y2;
		stepy = -1;
	}
	error = deltax - deltay;
	while ( (x1 != x2 + stepx) && (y1 != y2 + stepy) ) {
        //error checking
        if ( (y1 < maxy) && (y1 >= 0) && (x1 < DEFAULT_WIDTH) && (x1 >= 0) ) {
		    drawPixel(x1, y1, color);
            i = y1 * DEFAULT_WIDTH + x1;
            linemap[i] = color; //add it to the linemap.
        }
		if (error * 2 >= -1 * deltay) {
			x1 = x1 + stepx;
			error = error - deltay;
		}
		if (error * 2 <= deltax) {
			y1 = y1 + stepy;
			error = error + deltax;
		}
	}
}

void drawRect(int x1, int y1, int x2, int y2, ulong color) {
	drawLine(x1, y1, x2, y1, color);
	drawLine(x1, y2, x2, y2, color);
	drawLine(x2, y1, x2, y2, color);
	drawLine(x1, y1, x1, y2, color);
}

void fillRect(int x1, int y1, int x2, int y2, ulong color, bool gradient) {
	int iterations;
	if ( (x2 - x1) > (y2 - y1) )
		iterations = (x2 - x1) / 2;
	else iterations = (y2 - y1) / 2;

	while (iterations != 0) {
		drawRect(x1 + iterations, y1 + iterations, x2 - iterations, y2 - iterations, color);
		iterations--;
		if (gradient) color += 2;
	}
	drawRect(x1, y1, x2, y2, color);
}

/* Midpoint Circle Algorithm calculation of a circle.
 * Based on "x^2 + y^2 = r^2" */

void drawCircle (int x0, int y0, int radius, ulong color)
{
    int x = radius, y = 0;
  	int radiusError = 1 - x;

  	while(x >= y)
  	{
    	drawPixel(x + x0, y + y0, color);
    	drawPixel(y + x0, x + y0, color);
    	drawPixel(-x + x0, y + y0, color);
    	drawPixel(-y + x0, x + y0, color);
    	drawPixel(-x + x0, -y + y0, color);
    	drawPixel(-y + x0, -x + y0, color);
    	drawPixel(x + x0, -y + y0, color);
    	drawPixel(y + x0, -x + y0, color);
    	y++;
        if(radiusError < 0)
           	radiusError += 2 * y + 1;
       	else
       	{
           	x--;
           	radiusError += 2 * (y - x + 1);
       	}
  	}
}

void fillCircle (int x0, int y0, int radius, ulong color, bool gradient) {
	int rad = radius;
	while (rad != 0) {
		drawCircle(x0, y0, rad, color);
		rad--;
		if (gradient) color++;
	}
}

/* Turtle graphics specific circle functions */
void drawBody (int x0, int y0, int radius, ulong color)
{
    int x = radius, y = 0;
  	int radiusError = 1 - x;

    //the maximum y-value we can have so that we don't draw any part of turtle in the minishell area
    int maxy = DEFAULT_HEIGHT - (MINISHELLMINROW * CHAR_HEIGHT) - BODY_RADIUS - (HEAD_RADIUS * 2);
    //don't even try to draw turtle if y is out of bounds
    if ((y0 > maxy) || (y0 < 0) || (x0 > DEFAULT_WIDTH) || (x0 < 0)) {
        return;
    }

  	while(x >= y)
  	{
            if (color == background) {
            //if we are drawing in background color, we're erasing
            //an existing circle. in that case, replace with
            //saved linemap data.
                drawPixel(x + x0, y + y0, linemap[(y+y0) * DEFAULT_WIDTH + (x+x0)] );
        	    drawPixel(y + x0, x + y0, linemap[(x+y0) * DEFAULT_WIDTH + (y+x0)] );
        		drawPixel(-x + x0, y + y0, linemap[(y+y0) * DEFAULT_WIDTH + (-x+x0)] );
        		drawPixel(-y + x0, x + y0, linemap[(x+y0) * DEFAULT_WIDTH + (-y+x0)] );
        		drawPixel(-x + x0, -y + y0, linemap[(-y+y0) * DEFAULT_WIDTH + (-x+x0)] );
        		drawPixel(-y + x0, -x + y0, linemap[(-x+y0) * DEFAULT_WIDTH + (-y+x0)] );
        		drawPixel(x + x0, -y + y0, linemap[(-y+y0) * DEFAULT_WIDTH + (x+x0)] );
        		drawPixel(y + x0, -x + y0, linemap[(-x+y0) * DEFAULT_WIDTH + (y+x0)] );
            } else { //draw the circle normally.
        	    drawPixel(x + x0, y + y0, color);
        	    drawPixel(y + x0, x + y0, color);
        	    drawPixel(-x + x0, y + y0, color);
        	    drawPixel(-y + x0, x + y0, color);
        	    drawPixel(-x + x0, -y + y0, color);
        	    drawPixel(-y + x0, -x + y0, color);
        	    drawPixel(x + x0, -y + y0, color);
        	    drawPixel(y + x0, -x + y0, color);
            }
    		y++;
            if(radiusError < 0)
               	radiusError += 2 * y + 1;
        	else
        	{
              	x--;
               	radiusError += 2 * (y - x + 1);
        	}
  	}
}

void fillBody (int x0, int y0, int radius, ulong color, bool gradient) {
	int rad = radius;
	while (rad != 0) {
		drawBody(x0, y0, rad, color);
		rad--;
		if (gradient) color++;
	}
}


