#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "vector.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include "camera.h"

class CBillboard
{
public:
    CBillboard(char * label, SDL_Color sdlcolor, float width, float height, CVector ppos);
    CBillboard(char * filename, float width, float height, CVector ppos);
    ~CBillboard();
    void Draw(CCamera * Cam);
    void SetPos(CVector & ppos);
    void SetPos(float x, float y, float z);

private:
    bool CreateLabel(char * label, SDL_Color sdlcolor);
    unsigned int texture;
    float hw;
    float hh;
    CVector pos;
};


#endif
