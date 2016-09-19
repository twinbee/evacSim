#include "billboard.h"
#include <GL/gl.h>
#include "convert.h"
#include "textures.h"

CBillboard::CBillboard(char * label, SDL_Color sdlcolor, float width, float height, CVector ppos)
{
    pos = ppos;
    hw = width / 2.0;
    hh = height / 2.0;

    CreateLabel(label,sdlcolor);
}

CBillboard::CBillboard(char * filename, float width, float height, CVector ppos)
{
    pos = ppos;
    hw = width / 2.0;
    hh = height / 2.0;
    
    if(!CreateTexture((unsigned char *)filename,&texture,GL_REPEAT, false))
    {
      SDL_Color sdlcolor;
      sdlcolor.r = 255;
      sdlcolor.g = 255;
      sdlcolor.b = 255;
      hw = (height *  14) / 2.0;
      CreateLabel("File Not Found", sdlcolor);
    }
}

bool CBillboard::CreateLabel(char * label, SDL_Color sdlcolor)
{
    int fontsize = 14;
    //aquire the font
    if (TTF_Init()==-1) return false;
    TTF_Font * lfont = TTF_OpenFont( "fonts/sans.ttf", fontsize );
    if (!lfont) return false;

    //create the initial surface
    SDL_Surface *initial = TTF_RenderText_Solid(lfont, label, sdlcolor);
    if (!initial)
    {
        TTF_CloseFont(lfont);
        return false;
    }

    /* Convert the rendered text to a known format */
    int w = nextpoweroftwo(initial->w);
    int h = nextpoweroftwo(initial->h);

    SDL_Surface *intermediary = SDL_CreateRGBSurface(SDL_HWSURFACE, w, h, 32,
                                0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    if (!intermediary)
    {
        SDL_FreeSurface(initial);
        return false;
    }

    SDL_BlitSurface(initial, 0, intermediary, 0);


    /* Tell GL about our new texture */
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, intermediary->pixels );

    /* GL_NEAREST looks horrible, if scaled... */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFinish();

    //clean up our temporary surfaces and temp font
    SDL_FreeSurface(initial);
    SDL_FreeSurface(intermediary);
    TTF_CloseFont(lfont);

    return true;
}

CBillboard::~CBillboard()
{
    glDeleteTextures(1, &texture);
}

void CBillboard::Draw(CCamera * Cam)
{
     Cam->MouseFollow(0.00001,0.00001,0.00001);
    //get orientation
    CVector view = pos - Cam->GetPos();
    view.Normalize();
    CVector left = Cam->GetSide();
    CVector up = view ^ left;

    //create billboard quad bounds
    left |= hw;
    up |= hh;

    //create the four corners of the billboard quad
    CVector P1 = left + -up;
    CVector P2 = -left + -up;
    CVector P3 = -left + up;
    CVector P4 = left + up;

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);

    /* prepare to render our texture */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
    glTranslatef(pos.x,pos.y,pos.z);
    /* Draw a quad at location */
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(P1.x,P1.y,P1.z ); //left bottom
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(P2.x,P2.y,P2.z); //right bottom
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(P3.x,P3.y,P3.z);  //right top
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(P4.x,P4.y,P4.z);   //left top
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
}

void CBillboard::SetPos(CVector & ppos)
{
    pos = ppos;
}

void CBillboard::SetPos(float x, float y, float z)
{
    pos.Set(x,y,z);
}
