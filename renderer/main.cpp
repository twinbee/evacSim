/*********************************************************
** main.cpp
** Primary author: Matthew B, Mike J
** Description: Entry point into execution
*********************************************************/

#include "sound.h"

#include "map.h"

#include "textbuffer.h"

#include "displaylists.h"

#include "draw.h"

#include "textures.h"

#include "camsutil.h"

#include "draw.h"

#include "hud.h"

#include "convert.h"

#include "parse.h"

#include "stringBuffer.h"

#include "stringutil.h"

#include "mousekeys.h"

#include "script.h"

#include "BSPtree.h"

bool debug = true; // Mike E -- everything goes in main!

int NumCollisions;
float starttimer;

//ASSORTED globals


physmode g_PHYS_MODE = PHYSICSON;

int g_MAIN_STATE = MENU;

CCamera g_camera;

float g_Near_Clip = 1.0;
float g_Far_Clip = 10000;

bool g_Draw_Detail_Map = false;
bool g_Draw_Pathing_Map = true;
bool g_Draw_Occlusion_Map = false;

const SDL_Color SDL_WHITE = {255, 255, 255};
const SDL_Color SDL_GREEN = {0, 255, 0};
const SDL_Color SDL_BLUE  = {0, 0, 255};

float my_time;
float lasttime = 0;
float FrameEnd, FrameTime, FrameStart, FPS;

float SimStepSize = 1.0/10.0;

//choose a font
TTF_Font *font;
TTF_Font *monospace; //0
TTF_Font *serif;     //1
TTF_Font *sans;      //2
int fontsize = 14;

double WallStart;

float g_SimTime = 0;
float g_WalTime = 0;

int frame = 0;
int framemod = 0;
int frameskip = 2;

bool g_HOLD = false;

vector <std::string> scriptCommands;

//vector <pair <int, char *> > AtSysTimeCommands;
vector <pair <int, std::string> > AtWallTimeCommands;
vector <pair <int, std::string> > AtSimTimeCommands;
vector <pair <int, std::string> > AtPercentCommands;

//SCREEN DIMENSIONS
int SCREEN_DEPTH = 32;
int SCREEN_WIDTH =  800;
int SCREEN_HEIGHT = 600;
float SCREEN_WIDTH_BIAS = 1.0;
float SCREEN_HEIGHT_BIAS = 1.0;

//DRAWING FLAGS
bool drawFPS = true;
bool textprompt = true;

bool g_bImageCapture = false;
bool g_bdrawcursor = true;

//PICKING STATE
int g_PICKING_MODE = 1;
vector<int> PortalPick;

//menu globals
unsigned int g_menutex;
unsigned int g_crosstex;

float crossrot;
float g_menutexX;
float g_menutexY;

//CLASSES
CTextBuffer textbuffer;
CStringBuffer stringbuffer;

CFrustrum g_Frustum;

CMap * g_Map = 0;
Model* g_Model = 0;
CBSPtree * g_Omap = 0;

SDL_Surface *MainWindow;
int VideoFlags;

bool Done = false;

GLenum rgb, doubleBuffer;
int windW, windH;

int g_RenderFlags;

void InitGL(void)
{
    //initialize our display lists
    cylDL = createCylDL();
    ccylDL = createCCylDL();
    sphereDL = createSphereDL();

    glLineStipple(1, 0x00FF);

    //create the menu textures and set the menu scroll variables
    CreateTexture((unsigned char *)"textures/menu.tga",&g_menutex,GL_REPEAT,true);

    //load our textures
    CreateTexture((unsigned char *)"textures/crosshairs.tga",&g_crosstex,GL_REPEAT,false);

    //aquire the font
    if (TTF_Init()==-1) cerr << "TTF_Init: " << TTF_GetError() << endl;

    sans = TTF_OpenFont( "fonts/sans.ttf", fontsize );
    monospace = TTF_OpenFont( "fonts/mono.ttf", fontsize );
    serif = TTF_OpenFont( "fonts/serif.ttf", fontsize );

    font = serif;

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_DEPTH_TEST);

    SizeOpenGLScreen(SCREEN_WIDTH, SCREEN_HEIGHT);

    //set the intial line width
    glLineWidth(3.0);
    glClearColor(0.9,0.9,1.0,0);

    g_RenderFlags = 0;

    //lighting stuff
    glShadeModel(GL_FLAT);

    GLfloat LightAmbient[]=  { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat LightDiffuse[]=  { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat LightSpecular[]=  { 1.0f, 1.0f, 1.0f, 1.0f };

    GLfloat global_ambient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    glLightfv(GL_LIGHT7, GL_AMBIENT, LightAmbient);    // Setup The Ambient Light
    glLightfv(GL_LIGHT7, GL_DIFFUSE, LightDiffuse);    // Setup The Diffuse Light
    glLightfv(GL_LIGHT7, GL_SPECULAR,LightSpecular);

    glEnable(GL_LIGHT7);
    glEnable(GL_LIGHTING);

    //set the winding of "front" facing polygons
    // also sets the "back" face to be opposite (options: GL_CW, GL_CCW)
    glFrontFace(GL_CCW);

    glFogf( GL_FOG_START, g_Near_Clip );
    glFogf( GL_FOG_END, g_Far_Clip );
    
    NumCollisions = 0;
    starttimer = g_SimTime;

}//end init()

void Reshape(int width, int height)
{
    windW = width;
    windH = height;

    int sw = SCREEN_WIDTH;
    int sh = SCREEN_HEIGHT;

    if (width != SCREEN_WIDTH)   SCREEN_WIDTH = width;
    if (height != SCREEN_HEIGHT) SCREEN_HEIGHT = height;

    if (sw != SCREEN_WIDTH || sh != SCREEN_HEIGHT)
    {
        string output = "> Resolution changed from " + stringify(sw) + "x" + stringify(sh) + " to " + stringify(width) + "x" + stringify(height) + "<";
        textbuffer.InsertLine(output);
    }

    InitGL();
}

void UpdateGame(void)
{
 if (debug) cerr << "UpdateGame()\n";
    //check to see which keys are held down and react
    {
        if (g_leftpressed) g_camera.StrafeLeft(SimStepSize);
        if (g_rightpressed) g_camera.StrafeRight(SimStepSize);
        if (g_downpressed) g_camera.MoveBackward(SimStepSize);
        if (g_uppressed) g_camera.MoveForward(SimStepSize);
    }

    if (!g_Map) return; //nothing else to be done

    //run at Percentage commands
    int out = g_Map->GetNumEvacuated();
    int in = g_Map->GetNumRemaining();

    float percent = 100.0*(out/float(out+in)) + 1.0; //+1 bc of int round-down

    for (int i = 0; i < AtPercentCommands.size(); i++)
    {
        if (percent >= AtPercentCommands[i].first )
        {
            stringbuffer.setstr(AtPercentCommands[i].second);
            EvaluatePrompt();
            AtPercentCommands.erase(AtPercentCommands.begin() + i);
        }
    }

    //run at Simtime commands
    //int simhours   =  int(g_SimTime / 3600);
    //int simminutes =  int(g_SimTime / 60 );
    int simseconds =  int(g_SimTime);

    for (int i = 0; i < AtSimTimeCommands.size(); i++)
    {
        if (simseconds >= AtSimTimeCommands[i].first )
        {
            stringbuffer.setstr(AtSimTimeCommands[i].second);
            EvaluatePrompt();
            AtSimTimeCommands.erase(AtSimTimeCommands.begin() + i);
        }
    }

    //run at walltime commands

    for (int i = 0; i < AtWallTimeCommands.size(); i++)
    {
        if (g_WalTime >= AtWallTimeCommands[i].first )
        {
            stringbuffer.setstr(AtWallTimeCommands[i].second);
            EvaluatePrompt();
            AtSimTimeCommands.erase(AtWallTimeCommands.begin() + i);
        }
    }

    cout<<"DEBUG : g_Map->Update() begin. "<<g_WalTime<<endl;
    //update the entities' behavior
    if (g_MAIN_STATE == SIMULATION)
    {
        
        if (g_Map) g_Map->Update(SimStepSize);

        //if 1/30 s has passed, output debug data
        if((g_SimTime - starttimer) > (1.0/30.0))
        {
              cerr<<g_SimTime<<" "<<NumCollisions<<endl;
              NumCollisions = 0;
              starttimer = g_SimTime;              
        }
    }
    
    cout<<"DEBUG : g_Map->Update() end. "<<g_WalTime<<endl;

}//end updateGame()

void RenderMenu()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Begin2d();

    glDisable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0);

    SDL_Rect position;
    position.x = 10;
    position.y = 10;

    //draw the first watery menu background texture
    glBindTexture(GL_TEXTURE_2D, g_menutex);
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f+g_menutexX, 1.0f);
    glVertex2f(0 ,0);
    glTexCoord2f(1.0f+g_menutexX, 1.0f);
    glVertex2f(SCREEN_WIDTH,0);
    glTexCoord2f(1.0f+g_menutexX, 0.0f);
    glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT);
    glTexCoord2f(0.0f+g_menutexX, 0.0f);
    glVertex2f(0  , SCREEN_HEIGHT);
    glEnd();

    glEnable(GL_BLEND);
    glBlendFunc(GL_ZERO, GL_SRC_COLOR);

    //draw the second watery background texture
    glBindTexture(GL_TEXTURE_2D, g_menutex);
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f-g_menutexX, 1.0f+.2);
    glVertex2f(0 ,0);
    glTexCoord2f(1.0f-g_menutexX, 1.0f+.2);
    glVertex2f(SCREEN_WIDTH,0);
    glTexCoord2f(1.0f-g_menutexX, 0.0f+.2);
    glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT);
    glTexCoord2f(0.0f-g_menutexX, 0.0f+.2);
    glVertex2f(0  , SCREEN_HEIGHT);
    glEnd();

    glDisable(GL_BLEND);

    g_menutexX+= 0.06 * FrameTime; //increment the menu background offset


    //draw the title in the bottom left of the menu
    SDL_GL_RenderText("USM EVACSIM. Release 0.5, build " __DATE__, serif, SDL_WHITE, &position);

    //draw the text prompt if it has been turned on
    if (textprompt)
    {

        //draw whatever is in the textbuffer onto the black background
        SDL_Rect promptpos = {Sint16 (2), Sint16 (SCREEN_HEIGHT - SCREEN_HEIGHT_BIAS * 16*16 + SCREEN_HEIGHT_BIAS * 15)};
        SDL_Rect cursorpos = {Sint16 (2+stringbuffer.getCursorPos()*(fontsize - 6)), Sint16 (SCREEN_HEIGHT - SCREEN_HEIGHT_BIAS * 16*16 + SCREEN_HEIGHT_BIAS * 15)};

        SDL_GL_RenderText(stringbuffer.getData(), font, SDL_GREEN, &promptpos);
        SDL_GL_RenderText("_", font, SDL_GREEN, &cursorpos);

        for (int x=textbuffer.numOutputLines(); x > 0; x--)
        {
            promptpos.y += 15;
            SDL_GL_RenderText(textbuffer.getOutputLine(x).c_str(), font, SDL_WHITE, &promptpos);
        }
    }//end if textprompt

    SDL_GL_SwapBuffers();
    glDisable(GL_ALPHA_TEST);

    End2d();

}//end RenderMenu


//////////////////////////////////////////////////////////////////////////////

void CreateMyWindow(const char * strWindowName, int width, int height, int VideoFlags)
{
    MainWindow = SDL_SetVideoMode(width, height, SCREEN_DEPTH, VideoFlags );     // SCREEN_DEPTH is macro for bits per pixel

    if ( MainWindow == NULL ) exit(1);
    SDL_WM_SetCaption(strWindowName, strWindowName);    // set the window caption (first argument) and icon caption (2nd arg)
}

//////////////////////////////////////////////////////////////////////////////

void SetupPixelFormat(void)
{
    //////// SURFACE IS THE DRAWABLE PORTION OF AN SDL WINDOW \\\\\\\\

    /////////////  we set the common flags here
    VideoFlags    = SDL_OPENGL;     // it's an openGL window
    VideoFlags    |= SDL_HWPALETTE;  // exclusive access to hardware colour palette
    VideoFlags    |= SDL_RESIZABLE;  // the window must be resizeable
    //VideoFlags    |= SDL_FULLSCREEN;  // the window must be resizeable

    const SDL_VideoInfo * VideoInfo = SDL_GetVideoInfo();     // query SDL for information about our video hardware

    if (VideoInfo == NULL)                                    // if this query fails
    {
        cerr<<"SDL could not retrieve video information, initialization failed"<<endl;
        exit(1);
    }

    ///////////// we set the system dependant flags here
    if (VideoInfo -> hw_available) VideoFlags |= SDL_HWSURFACE;
    else
    {
        VideoFlags |= SDL_SWSURFACE;
        cerr<<"SDL:Hardware Surfaces not available, check video card driver"<<endl;
    }
    // Blitting is fast copying / moving /swapping of contiguous sections of memory
    // for more about blitting check out : http://www.csc.liv.ac.uk/~fish/HTML/blitzman/bm_blitter.html
    if (VideoInfo -> blit_hw)                           // is hardware blitting available
        VideoFlags |= SDL_HWACCEL;
    else cerr<<"SDL:Hardware blitting not available, check video card driver"<<endl;

    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );      // tell SDL that the GL drawing is going to be double buffered
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE,   SCREEN_DEPTH);         // size of depth buffer
    SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 0);       // we aren't going to use the stencil buffer
    SDL_GL_SetAttribute( SDL_GL_ACCUM_RED_SIZE, 0);     // this and the next three lines set the bits allocated per pixel -
    SDL_GL_SetAttribute( SDL_GL_ACCUM_GREEN_SIZE, 0);   // - for the accumulation buffer to 0
    SDL_GL_SetAttribute( SDL_GL_ACCUM_BLUE_SIZE, 0);
    SDL_GL_SetAttribute( SDL_GL_ACCUM_ALPHA_SIZE, 0);
}

///////////////////////////////////////////////////////////////////////////////

void MainLoop(void)
{
    bool done = false;

    SDL_Event event;
    int tindex = 0;
    float FrameTimes[10];

    while (!done)
    {
        while ( SDL_PollEvent(& event) )                   // look for events (like keystrokes, resizing etc.)
        {
            switch ( event.type )                          // what kind of event have we got ?
            {
            case SDL_MOUSEBUTTONDOWN :
            {

                HandleMouseClickEvent( event.button );
            }
            break;

            case SDL_QUIT :                                         // if user wishes to quit    
                done = true;                                    // this implies our job is done
                cerr<<"SDL QUIT event matt is gay, really fucking gay"<<endl;
                break;

            case SDL_KEYDOWN :                                      // if the user has pressed a key
                HandleKeyPressEvent( & event. key.keysym );         // callback for handling keystrokes, arg is key pressed
                break;

            case SDL_KEYUP :
                HandleKeyReleaseEvent(& event.key.keysym) ;         // callback for handling keystrokes, arg is key released
                break;

            case SDL_VIDEORESIZE :                                  // if there is a resize event
                // request SDL to resize the window to the size and depth etc. that we specify
                if (event.resize.w != windW)
                {
                    MainWindow = SDL_SetVideoMode( event.resize.w, (3*event.resize.w/4), SCREEN_DEPTH, VideoFlags );
                    Reshape(event.resize.w, (3*event.resize.w/4));   // now resize the OpenGL viewport
                }
                else if (event.resize.h != windH)
                {
                    MainWindow = SDL_SetVideoMode( (4*event.resize.h/3), event.resize.h, SCREEN_DEPTH, VideoFlags );
                    Reshape((4*event.resize.h/3), event.resize.h);   // now resize the OpenGL viewport
                }


                //if (MainWindow == NULL)                             // if window resize has failed
                   // Quit(0);
                break;

            default:                                    // any other event
                break;                                  // nothing to do
            } // switch

        } // while( SDL_ ...


        //if (!g_bImageCapture)
        HandleMouse();

        if ((g_MAIN_STATE == SIMULATION) || (g_MAIN_STATE == CAMERAMODE))
        {
            UpdateGame();


            if (g_bImageCapture)
                DrawOffscreen();
            else
                DrawInteractive();
        }

        else if (g_MAIN_STATE == MENU)
        {
            RenderMenu();
        }
        long sdlticks = SDL_GetTicks();

        //FrameEnd=  (double)SDL_GetTicks() / 1000;
        FrameEnd=  (double)sdlticks / 1000;

        FrameTime=FrameEnd-FrameStart;

        FPS = 1.0/FrameTime;
        FrameStart=FrameEnd;

        if (g_MAIN_STATE == SIMULATION) g_SimTime+=SimStepSize;

        //g_WalTime = (SDL_GetTicks() - WallStart) / 1000.0;
        g_WalTime = (sdlticks - WallStart) / 1000.0;

        tindex++;
        if (tindex > 9) tindex = 0;

        if (!(sdlticks % 2)) runScript(); // run one script line every 6 window clock ticks

        //SDL_Delay(50); //hog / don't hog the CPU!
    } // while( ! done)

    cerr << "SDL Done = true\n";
	//Release the memory allocated to our sounds

    Quit(0);
}

//////////////////////////////////////////////////////////////////////////////

void SizeOpenGLScreen(int width, int height)
{
    glViewport(0,0,width,height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height, g_Near_Clip, g_Far_Clip);

    glMatrixMode(GL_MODELVIEW);
}


void Quit(int ret_val)
{
    SDL_Quit();                                            // shuts down SDL stuff

    //delete whatever needs deleting before the app ends

    TTF_CloseFont( serif );
    TTF_CloseFont( sans );
    TTF_CloseFont( monospace );

    glDeleteTextures(1,&g_menutex);
    glDeleteTextures(1,&g_crosstex);

    Sound::killSDLMixer();
    
    cerr << "Quit()\n";
    exit(ret_val);                                         // quit the program
}

///////////////////////////////////////////////////////////////////////////////

void HandleCommandLine(int argc, char ** argv)
{
    if (argc > 1)
    {
        for (int c = 1; c < argc; c++)
        {
            char loadstring[100] = "script ";
            strcat(loadstring,argv[c]);
            stringbuffer.setstr(loadstring);
            EvaluatePrompt();
        }
    }

}


int main (int argc, char * argv[])
{
    if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
    {
        fprintf( stderr, "Video initialization failed: %s\n", SDL_GetError( ) );
        SDL_Quit( );
    }

    // Set up the format for the pixels of the OpenGL drawing surface
    SetupPixelFormat();

    // Create our window, we pass caption for the window, the width, height and video flags required
    CreateMyWindow("Southern Miss EvacSim. School of Computing. Built " __DATE__,
                   SCREEN_WIDTH, SCREEN_HEIGHT, VideoFlags);

    MouseOutWindow();

    FrameStart=(double)SDL_GetTicks() / 1000;
    FrameTime = 0;

    //init the PROGRAM STATE to start at the menu
    g_MAIN_STATE = MENU;

    SDL_WarpMouse(SCREEN_WIDTH  >> 1, SCREEN_HEIGHT >> 1);

    InitGL();     // Initializes our OpenGL drawing surface

    if (argc > 0) HandleCommandLine(argc, argv);

    startupScript();

    MainLoop(); //"game" event loop

}//end main()

