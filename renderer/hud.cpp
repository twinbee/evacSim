/*********************************************************
** hud.cpp
** Primary author: Mike J, Matthew B
** Description: Heads-up-display
** Used in sim: to draw an onscreen console window in simulation mode
*********************************************************/

#include "hud.h"

#include "camsutil.h"

#include "map.h"

#include "stringbuffer.h"

#include "textbuffer.h"

#include <GL/glu.h>

using namespace std;

void SDL_GL_RenderText(const char *text, TTF_Font *font, SDL_Color color, SDL_Rect *location)
{

    SDL_Surface *initial;
    SDL_Surface *intermediary;

    int w,h;
    unsigned int texture;

    /* Use SDL_TTF to render our text */
    if (!font) return;
    initial = TTF_RenderText_Solid(font, text, color);
    if (!initial) return;

    /* Convert the rendered text to a known format */
    w = nextpoweroftwo(initial->w);
    h = nextpoweroftwo(initial->h);

    intermediary = SDL_CreateRGBSurface(0, w, h, 32,
                                        0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

    SDL_BlitSurface(initial, 0, intermediary, 0);


    /* Tell GL about our new texture */
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, intermediary->pixels );

    /* GL_NEAREST looks horrible, if scaled... */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    /* prepare to render our texture */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glColor3f(1.0f, 1.0f, 1.0f);


    /* Draw a quad at location */
    glBegin(GL_QUADS);
    /* Recall that the origin is in the lower-left corner
       That is why the TexCoords specify different corners
       than the Vertex coors seem to. */
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(SCREEN_WIDTH_BIAS * location->x    , SCREEN_HEIGHT_BIAS *location->y);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(SCREEN_WIDTH_BIAS * (location->x + w), SCREEN_HEIGHT_BIAS *location->y);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(SCREEN_WIDTH_BIAS * (location->x + w), SCREEN_HEIGHT_BIAS * (location->y + h));
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(SCREEN_WIDTH_BIAS * location->x    , SCREEN_HEIGHT_BIAS * (location->y + h));
    glEnd();

    /* Bad things happen if we delete the texture before it finishes */
    glFinish();

    /* return the deltas in the unused w,h part of the rect */
    location->w = initial->w;
    location->h = initial->h;

    /* Clean up */
    SDL_FreeSurface(initial);
    SDL_FreeSurface(intermediary);
    glDeleteTextures(1, &texture);
}

void Begin2d()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    int vPort[4];
    glGetIntegerv(GL_VIEWPORT, vPort);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, vPort[2], 0, vPort[3], -1, 1);
    //glOrtho(left,right,bottom, top,near,far)

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void End2d()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, ((float)SCREEN_WIDTH)/((float)SCREEN_HEIGHT), g_Near_Clip, g_Far_Clip);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void DrawHud()
{
    int ht = 15;
    SDL_Rect position = {(Sint16) 10.0, (Sint16) ht};

//heads up display is drawin in front of the scene
// includes information at bottom plus the text console
    Begin2d();

    //simple transparency method
    glDisable(GL_BLEND);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);

    static char statusline_text[80];

    if (!g_bImageCapture)
    {
        //draw the sim step size
        sprintf( statusline_text,"Sim Step Size : %f ",SimStepSize);
        position.y += ht;
        SDL_GL_RenderText(statusline_text, font, SDL_BLUE, &position);
    }
    //draw the fps
    if (drawFPS && (!g_bImageCapture))
    {
        sprintf ( statusline_text, "FPS : %f ",FPS );

        position.y += ht;

        SDL_GL_RenderText(statusline_text, font, SDL_BLUE, &position);
    }

    //draw simtime
    int simhours   =  int(g_SimTime / 3600);
    int simminutes =  int((g_SimTime - 3600*simhours) / 60 );
    int simseconds =  int((g_SimTime - 3600*simhours - 60*simminutes));

    sprintf ( statusline_text, "Sim Time(H/M/S) : %d %d %d ", simhours, simminutes, simseconds );

    position.y += ht;

    SDL_GL_RenderText(statusline_text, font, SDL_BLUE, &position);

    if (!g_bImageCapture)
    {
        //draw wall time
        int walhours   =  int(g_WalTime / 3600);
        int walminutes =  int((g_WalTime - 3600*walhours) / 60 );
        int walseconds =  int((g_WalTime - 3600*walhours - 60*walminutes));

        sprintf ( statusline_text, "Wall Time(H/M/S) : %d %d %d ", walhours, walminutes, walseconds );

        position.y += ht;

        SDL_GL_RenderText(statusline_text, font, SDL_BLUE, &position);
    }


    if (g_Map)
    {
        //draw the # of evacuated entities
        sprintf ( statusline_text, "Egress: %d ",g_Map->GetNumRemaining());

        position.y += ht;

        SDL_GL_RenderText(statusline_text, font, SDL_BLUE, &position);

        //draw # still in building

        sprintf ( statusline_text, "Safe:   %d ",(g_Map->GetNumEvacuated() - g_Map->GetNumFloaters()) );

        position.y += ht;

        SDL_GL_RenderText(statusline_text, font, SDL_BLUE, &position);

        //draw the # of dead entities
        sprintf ( statusline_text, "Dead: %d ",g_Map->NumDead());
        position.y += ht;
        SDL_GL_RenderText(statusline_text, font, SDL_BLUE, &position);
        
        //draw the # of injured entities
        sprintf ( statusline_text, "Injured: %d ",g_Map->NumInjured());
        position.y += ht;
        SDL_GL_RenderText(statusline_text, font, SDL_BLUE, &position);
        
        //draw # erroneous entities
        sprintf ( statusline_text, "Error:   %d ",g_Map->GetNumFloaters() );
        position.y += ht;
        SDL_GL_RenderText(statusline_text, font, SDL_BLUE, &position);
        
    }//end if g_Map

    //draw program mode
    if (!g_bImageCapture)
    {
        if ((g_MAIN_STATE == CAMERAMODE))
        {
            sprintf ( statusline_text, "Program Mode : CAMERAMODE [%d cameras]", g_Camera_list.size() );
        }

        else if (g_MAIN_STATE == SIMULATION)
            sprintf ( statusline_text, "Program Mode : SIMULATION" );

        position.y += ht;

        SDL_GL_RenderText(statusline_text, font, SDL_BLUE, &position);
    }
    if (g_bdrawcursor)
    {
        //draw crosshairs
        glPushMatrix();
        glTranslatef(SCREEN_WIDTH/2,SCREEN_HEIGHT/2,0);
        glRotatef(crossrot,0,0,1);
        crossrot += 120 * FrameTime;
        glBindTexture(GL_TEXTURE_2D, g_crosstex);
        glBegin(GL_QUADS);
        glTexCoord2f(0,1);
        //glVertex2i((SCREEN_WIDTH/2) - 16,(SCREEN_HEIGHT/2) + 16);
        glVertex2i( - 16, + 16);
        glTexCoord2f(1,1);
        //glVertex2i((SCREEN_WIDTH/2) + 16,(SCREEN_HEIGHT/2) + 16);
        glVertex2i( + 16,+ 16);
        glTexCoord2f(1,0);
        //glVertex2i((SCREEN_WIDTH/2) + 16,(SCREEN_HEIGHT/2) - 16);
        glVertex2i( + 16,- 16);
        glTexCoord2f(0,0);
        //glVertex2i((SCREEN_WIDTH/2) - 16,(SCREEN_HEIGHT/2) - 16);
        glVertex2i( - 16,- 16);
        glEnd();
        glPopMatrix();
    }

    //draw textprompt
    if (textprompt)
    {

        glDisable(GL_TEXTURE_2D);
        glColor3f(0,0,0);
        glBegin(GL_QUADS);

        //glTexCoord2f(0.0f, 1.0f);
        glVertex2f(0 ,SCREEN_HEIGHT - (16*15));
        //glTexCoord2f(1.0f, 1.0f);
        glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT - (16*15));
        //glTexCoord2f(1.0f, 0.0f);
        glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT);
        //glTexCoord2f(0.0f, 0.0f);
        glVertex2f(0  , SCREEN_HEIGHT);

        glColor3f(1,1,1);
        //glTexCoord2f(0.0f, 1.0f);
        glVertex2f(0 ,SCREEN_HEIGHT - (16*15));
        //glTexCoord2f(1.0f, 1.0f);
        glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT - (16*15));
        //glTexCoord2f(1.0f, 0.0f);
        glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT - (16*15) +2);
        //glTexCoord2f(0.0f, 0.0f);
        glVertex2f(0  , SCREEN_HEIGHT - (16*15)+2);

        glEnd();
        glEnable(GL_TEXTURE_2D);


        //draw whatever is in the textbuffer onto the black background
        SDL_Rect promptpos = {Sint16 (2), Sint16 (SCREEN_HEIGHT - SCREEN_HEIGHT_BIAS * 16*16 + SCREEN_HEIGHT_BIAS * 15)};
        SDL_Rect cursorpos = {Sint16 (2+stringbuffer.getCursorPos()*(fontsize - 6)), Sint16 (SCREEN_HEIGHT - SCREEN_HEIGHT_BIAS * 16*16 + SCREEN_HEIGHT_BIAS * 15)};

        SDL_GL_RenderText(stringbuffer.getData(), font, SDL_WHITE, &promptpos);
        SDL_GL_RenderText("_", font, SDL_WHITE, &cursorpos);

        for (int x=textbuffer.numOutputLines(); x > 0; x--)
        {
            promptpos.y += 15;
            char outline[100];
            strcpy(outline, textbuffer.getOutputLine(x).c_str());
            SDL_GL_RenderText(outline, font, SDL_WHITE, &promptpos);
        }

    }//end if textprompt


    glDisable(GL_ALPHA_TEST);

    End2d();
} //end DrawHUD
