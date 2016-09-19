/*********************************************************
** hud.h
** Primary author: Mike J, Matthew B
** Description: Heads-up-display
** Used in sim: to draw an onscreen console window in simulation mode
*********************************************************/

#ifndef HUD_H
#define HUD_H

#include "convert.h"

#include "main.h"

void SDL_GL_RenderText(const char *text, TTF_Font *font, SDL_Color color, SDL_Rect *location);
void Begin2d();
void End2d();
void DrawHud();

#endif
