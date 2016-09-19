#include "main.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include "draw.h"

#include "camsutil.h"

#include "snapshotj.h"

#include "hud.h"

#include "frustrum.h"

#include "draw.h"

#include "map.h"

#include "model.h"
using namespace std;

int DrawFlags = DRAW_SURFACE | DRAW_WALL | DRAW_PORTAL | DRAW_ENTITIES;

//////////////////////////////////////////////////////////////////////////////

void DrawInteractive(void)
{
    g_bdrawcursor = true;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity ();

    CVector Pos = g_camera.GetPos();
    CVector View = g_camera.GetView();
    CVector Up = g_camera.GetUp();

    gluLookAt( Pos.x,Pos.y,Pos.z, //position point
               Pos.x + View.x, Pos.y + View.y, Pos.z + View.z, //lookat point
               Up.x,Up.y,Up.z); //up vec

    CFrustrum Frustum;
    Frustum.CalculateFrustrum();
        


    // Full rendering is now faster than wireframe, so don't automatically
    // switch to wireframe on camera movements. [Mike E]

    if ( g_Model && g_Draw_Detail_Map )
    {
        g_Model->draw();
    }

    if ( g_Map and g_Draw_Pathing_Map )
    {
     g_Map->Draw(&g_camera,Frustum,FrameTime);
    }
    
    
    if(g_Omap && g_Draw_Occlusion_Map)
    {
      g_Omap->Draw(Frustum,false);
    }


    DrawHud();
    SDL_GL_SwapBuffers();
}

//////////////////////////////////////////////////////////////////////////////

void DrawOffscreen(void)
{
    g_bdrawcursor = false;

    if (framemod >= frameskip)
    {
        const int last_frame = 100000; //prevents disk from filling

        for (int c=0; c< g_Camera_list.size(); c++)
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glLoadIdentity ();

            CVector Pos = g_Camera_list[c].GetPos();
            CVector View = g_Camera_list[c].GetView();
            CVector Up = g_Camera_list[c].GetUp();

            gluLookAt(Pos.x,Pos.y,Pos.z, Pos.x + View.x,Pos.y + View.y, Pos.z + View.z,Up.x,Up.y,Up.z);//controls your camera! posx, posy, posz, lookx, looky, lookz, upx, upy, upz

            CFrustrum Frustum;
            Frustum.CalculateFrustrum();


            if (g_Model && g_Draw_Detail_Map)
            {
                g_Model->draw();
            }

            if (g_Map) g_Map->Draw(&g_Camera_list[c],Frustum,FrameTime);

            DrawHud();
            char temp[100];
            sprintf ( temp, "cam%d/",c+1);
            if (frame < 10) strcat(temp, "0");
            if (frame < 100) strcat(temp, "0");
            if (frame < 1000) strcat(temp, "0");
            if (frame < 10000) strcat(temp, "0");
            char num[15];
            strcat(temp,itoa(frame,num,10));
            strcat(temp,".JPG");

            if (frame > last_frame) g_bImageCapture = false;

            screenshot(SCREEN_WIDTH,  SCREEN_HEIGHT, temp, 80);
            SDL_Delay(200);
            
            glFlush();
            SDL_GL_SwapBuffers();


        }//end for each camera

        frame++;
        framemod = 0;
    }

    framemod++;

  //  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 //   glLoadIdentity ();
 //   DrawHud();
 //   SDL_GL_SwapBuffers();

}//end DrawOffscreen()
