/**********************************************************
** draw.h
** Primary Author: Mike J, Matthew B
** Description: some int-based flags we use for keeping track of graphics state
** Used in sim: draw
*********************************************************/

#ifndef DRAWFLAGS_H
#define DRAWFLAGS_H

//drawing flags
enum {
      DRAW_SURFACE = 1, 
      DRAW_NORMAL = 2, 
      DRAW_BB = 4, 
      DRAW_OUTLINE = 8,
      DRAW_LIT = 16, 
      DRAW_WALL = 32, 
      DRAW_PORTAL = 64, 
      DRAW_ENTITIES = 128,
      DRAW_FLOOR = 256      

     };

const int DRAW_REGION_ALL = DRAW_SURFACE | 
                            DRAW_WALL | 
                            DRAW_PORTAL | 
                            DRAW_ENTITIES;

extern int DrawFlags;

void DrawInteractive(void);
void DrawOffscreen(void);

//no more drawing flags than 16 please (4 lines of 4)

#endif
