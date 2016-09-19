//mousekeys.h

#ifndef MOUSEKEYS_H
#define MOUSEKEYS_H

#include <SDL.h>

//keydown flags
extern bool g_uppressed, g_downpressed, g_leftpressed, g_rightpressed,g_Camera_Moved;

// keyboard state flags
extern bool K_SHIFTMODE, K_CTRLMODE,K_ALTMODE;

enum mousemode {FREEM, LOCKM};
extern mousemode g_MOUSE_MODE;

void toggleMouseMode();
void MouseOutWindow();
void MouseInWindow();

void HandleKeyPressEvent(SDL_keysym * keysym);
void HandleKeyReleaseEvent(SDL_keysym * keysym);

void HandleMouse();

void HandleMouseClickEvent(SDL_MouseButtonEvent);

#endif
