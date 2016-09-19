/*********************************************************
** main.h
** Primary author: Matthew B, Mike J
** Description: contains externs to
   all global variables and program state incuding UI
*********************************************************/

#ifndef MAIN_H
#define MAIN_H

#include <GL/gl.h>
#include <GL/glu.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include <vector>
#include <utility>
#include "BSPtree.h"

class Model;
class CMap;
class CCamera;
class CFrustrum;

class CTextBuffer;
class CStringBuffer;

//class CBSPtree;


extern CCamera g_camera;

//PROGRAM STATES
extern TTF_Font *font;
extern TTF_Font *monospace; //0
extern TTF_Font *serif;     //1
extern TTF_Font *sans;      //2
extern int fontsize;

extern GLuint ccylDL, cylDL, sphereDL; //display lists

enum physmode {PHYSICSON, PHYSICSOFF};
extern physmode g_PHYS_MODE;

enum {MENU, SIMULATION, CAMERAMODE};
extern int g_MAIN_STATE;

//near and far clipping planes
extern float g_Near_Clip;
extern float g_Far_Clip;

//drawing states
extern bool g_Draw_Detail_Map;
extern bool g_Draw_Pathing_Map;
extern bool g_Draw_Occlusion_Map;

//prompt colors
extern const SDL_Color SDL_WHITE, SDL_GREEN, SDL_BLUE;

//timers
extern float my_time;
extern float lasttime;
extern float FrameEnd, FrameTime, FrameStart, FPS;

extern float SimStepSize;

extern double WallStart;

extern float g_SimTime, g_WalTime;
extern int   g_RunNum;

extern int frame,framemod,frameskip;

extern bool g_HOLD;

extern std::vector <std::string> scriptCommands;

//vector <pair <int, char *> > AtSysTimeCommands;
extern std::vector <std::pair <int, std::string> >
    AtWallTimeCommands, AtSimTimeCommands, AtPercentCommands;

//SCREEN DIMENSIONS
extern int SCREEN_DEPTH,SCREEN_WIDTH,SCREEN_HEIGHT;
extern float SCREEN_WIDTH_BIAS,SCREEN_HEIGHT_BIAS;

//DRAWING FLAGS
extern bool drawFPS,textprompt,g_bImageCapture,g_bdrawcursor;

//menu globals
extern unsigned int g_menutex,g_crosstex;
extern float crossrot,g_menutexX,g_menutexY;

//CLASSES
extern CTextBuffer textbuffer;
extern CStringBuffer stringbuffer;

extern CFrustrum g_Frustum;

extern CMap * g_Map;
extern Model* g_Model;
extern CBSPtree * g_Omap;

extern SDL_Surface *MainWindow;
extern int VideoFlags;

extern bool Done;

extern GLenum rgb, doubleBuffer;
extern int windW, windH;

extern int g_RenderFlags;

void MainLoop();
void CreateMyWindow(const char * strWindowName, int width, int height, int VideoFlags);
void SizeOpenGLScreen(int width, int height);
void SetupPixelFormat(void);

void Quit(int ret_val);
extern bool done;

void RenderMenu();
void EvaluatePrompt();

void Reshape(int w, int h);


void Quit(int);

#endif
