// mousekeys.cpp

#include "main.h"

#include "camsutil.h"

#include "mousekeys.h"

#include "stringbuffer.h"

#include "textbuffer.h"

#include "map.h"

#include "model.h"


//keydown flags
bool g_uppressed =   false;
bool g_downpressed = false;
bool g_leftpressed = false;
bool g_rightpressed = false;
bool g_Camera_Moved = false;

// keyboard state flags
bool K_SHIFTMODE = false;
bool K_CTRLMODE = false;
bool K_ALTMODE = false;

mousemode g_MOUSE_MODE = LOCKM;

//////////////////////////////////////////////////////////////////////////////

void HandleKeyPressEvent(SDL_keysym * keysym)
{
    if (keysym->sym == SDLK_TAB)
    {
        textprompt = !textprompt;
        return;
    }

    if (keysym->sym == SDLK_LSHIFT || keysym->sym == SDLK_RSHIFT)
    {
        K_SHIFTMODE = true;
        return;
    }

    if (keysym->sym == SDLK_LALT|| keysym->sym == SDLK_RALT)
    {
        K_ALTMODE = true;
        return;
    }

    if (keysym->sym == SDLK_LCTRL || keysym->sym == SDLK_RCTRL)
    {
        K_CTRLMODE = true;
        return;
    }

    switch (keysym->sym)
    {
    case SDLK_F1 :
        textprompt = true;
        stringbuffer.setstr("help");
        EvaluatePrompt();
        return;

    case SDLK_F2 :

        stringbuffer.setstr("fullscr");
        EvaluatePrompt();
        return;

    case SDLK_F3 :
    {
        toggleMouseMode();
        return;
    }

    case SDLK_F4 :
        stringbuffer.setstr("script text.txt");
        EvaluatePrompt();
        return;

    case SDLK_F5 :
    case SDLK_F6 :
    case SDLK_F7 :
    case SDLK_F8 :
    case SDLK_F9 :
    case SDLK_F10:
    case SDLK_F11:
    case SDLK_F12:

        return;
    }

    if (textprompt)
    {
        if (keysym->sym == SDLK_ESCAPE)
        {
            textprompt = !textprompt;
            return;
        }

        if (keysym -> sym == SDLK_BACKSPACE) stringbuffer.RemoveChar();

        else if (keysym->sym == SDLK_RETURN)
        {
            EvaluatePrompt();
        }
        else if (keysym->sym == SDLK_UP)
        {
            textbuffer.PrevCommand();
            stringbuffer.setstr(textbuffer.getCommand());
        }
        else if (keysym->sym == SDLK_DOWN)
        {
            textbuffer.NextCommand();
            stringbuffer.setstr(textbuffer.getCommand());
        }
        else
        {
            if (K_SHIFTMODE == true)
            {
                if (isalpha(keysym -> sym)) stringbuffer.AddChar(toupper(keysym -> sym));
                else if (keysym -> sym == '0') stringbuffer.AddChar(')');
                else if (keysym -> sym == '1') stringbuffer.AddChar('!');
                else if (keysym -> sym == '2') stringbuffer.AddChar('@');
                else if (keysym -> sym == '3') stringbuffer.AddChar('#');
                else if (keysym -> sym == '4') stringbuffer.AddChar('$');
                else if (keysym -> sym == '5') stringbuffer.AddChar('%');
                else if (keysym -> sym == '6') stringbuffer.AddChar('^');
                else if (keysym -> sym == '7') stringbuffer.AddChar('&');
                else if (keysym -> sym == '8') stringbuffer.AddChar('*');
                else if (keysym -> sym == '9') stringbuffer.AddChar('(');
                else if (keysym -> sym == '-') stringbuffer.AddChar('_');
                else if (keysym -> sym == '`') stringbuffer.AddChar('~');
                else if (keysym -> sym == '=') stringbuffer.AddChar('+');
                else if (keysym -> sym == '\\') stringbuffer.AddChar('|');
                else if (keysym -> sym == '[') stringbuffer.AddChar('{');
                else if (keysym -> sym == ']') stringbuffer.AddChar('}');
                else if (keysym -> sym == ';') stringbuffer.AddChar(':');
                else if (keysym -> sym == '\'') stringbuffer.AddChar('\"');
                else if (keysym -> sym == ',') stringbuffer.AddChar('<');
                else if (keysym -> sym == '.') stringbuffer.AddChar('>');
                else if (keysym -> sym == '/') stringbuffer.AddChar('?');
            }
            else stringbuffer.AddChar(keysym -> sym);
        }

    }

    else
        switch (keysym -> sym)                                 // which key have we got
        {

        case SDLK_ESCAPE:
            if ((g_MAIN_STATE == SIMULATION) || (g_MAIN_STATE == CAMERAMODE))
            {
                if (g_Map)
                {
                    textbuffer.InsertLine("> Deleting Pathing Map ... ");

                    delete g_Map;
                    g_Map = 0;


                    textbuffer.tag("done <");
                    RenderMenu();
                }

                if (g_Model)
                {
                    textbuffer.InsertLine("> Deleting Detail Map ... ");

                    delete g_Model;
                    g_Model = 0;

                    textbuffer.tag("done <");
                    RenderMenu();
                }
                
                if(g_Omap)
                {
                    textbuffer.InsertLine("> Deleting Occlusion Map ... ");

                    delete g_Omap;
                    g_Omap = 0;

                    textbuffer.tag("done <");
                    RenderMenu();                  
                }

                g_MAIN_STATE = MENU;
                g_Camera_list.clear();

            }
            else if (g_MAIN_STATE == MENU)
                Quit(0);
            break;

        case SDLK_UP:
            g_uppressed = true;
            break;
        case SDLK_DOWN:
            g_downpressed = true;
            break;
        case SDLK_RIGHT:
            g_rightpressed = true;
            break;
        case SDLK_LEFT:
            g_leftpressed = true;
            break;

        case 'P':
        case 'p':
				stringbuffer.setstr("pause");
            EvaluatePrompt();
				break;
        case 'C':
        case 'c':
            if (g_Camera_list.size() < maxcams-1 )
                g_Camera_list.push_back(g_camera);
                dirtyCams[g_Camera_list.size()]++;
            break;

        case '1':
        case '!':
            stringbuffer.setstr("cam 1");
            EvaluatePrompt();
            break;
        case '2':
        case '@':
            stringbuffer.setstr("cam 2");
            EvaluatePrompt();
            break;
        case '3':
        case '#':
            stringbuffer.setstr("cam 3");
            EvaluatePrompt();
            break;
        case '4':
        case '$':
            stringbuffer.setstr("cam 4");
            EvaluatePrompt();
            break;
        case '5':
        case '%':
            stringbuffer.setstr("cam 5");
            EvaluatePrompt();
            break;
        case '6':
        case '^':
            stringbuffer.setstr("cam 6");
            EvaluatePrompt();
            break;
        case '7':
        case '&':
            stringbuffer.setstr("cam 7");
            EvaluatePrompt();
            break;
        case '8':
        case '*':
            stringbuffer.setstr("cam 8");
            EvaluatePrompt();
            break;
        case '9':
        case '(':
            stringbuffer.setstr("cam 9");
            EvaluatePrompt();
            break;

        default:
            break;
        }
}

//////////////////////////////////      HANDLE KEY RELEASE EVENT    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
//////
//////     This function handles the keyrelease events generated when the user release a key
//////
//////////////////////////////////      HANDLE KEY RELEASE EVENT    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void HandleKeyReleaseEvent(SDL_keysym * keysym)
{
    switch (keysym -> sym)                                 // which key have we got
    {
    case SDLK_LEFT:
        g_leftpressed = false;

        break;
    case SDLK_RIGHT:
        g_rightpressed = false;

        break;
    case SDLK_UP:
        g_uppressed = false;

        break;
    case SDLK_DOWN:
        g_downpressed = false;

        break;
    case SDLK_LSHIFT:
    case SDLK_RSHIFT:
        K_SHIFTMODE = false;

        break;


    default:                                           // any other key
        break;                                         // nothing to do
    }
}

///////////////////////////////////////////////////////////////////////////////

void HandleMouse()
{
    int mouseX,mouseY;
    int middleX = SCREEN_WIDTH  >> 1;              // This is a binary shift to get half the width
    int middleY = SCREEN_HEIGHT >> 1;              // This is a binary shift to get half the height

    int state = SDL_GetMouseState(&mouseX,&mouseY);

    if (g_MOUSE_MODE == LOCKM)
    {
        //**PUT CODE HERE USING THE MOUSEX,MOUSEY, MIDDLEX, MIDDLE Y VARS WHICH ARE UPDATED ABOVE
        if ( (mouseX == middleX) && (mouseY == middleY) )  return;
        else
        {
            g_Camera_Moved = true;
            g_camera.MouseFollow(mouseX-middleX,mouseY-middleY,SimStepSize);
            // Set the mouse position to the middle of our window
            SDL_WarpMouse(middleX, middleY);
        }
    }
}

void HandleMouseClickEvent(SDL_MouseButtonEvent e)
{
    if (e.button == SDL_BUTTON_MIDDLE) toggleMouseMode();

}

//////////////////////////////////////////////////////////////////////////////
void toggleMouseMode()
{
    if (g_MOUSE_MODE == LOCKM)
    {
        MouseOutWindow();
    }

    else if (g_MOUSE_MODE == FREEM)
    {
        MouseInWindow();
    }
}

void MouseOutWindow()
{
    SDL_WM_GrabInput(SDL_GRAB_OFF);
    SDL_ShowCursor( SDL_ENABLE );
    g_MOUSE_MODE = FREEM;
}

void MouseInWindow()
{

    SDL_WM_GrabInput(SDL_GRAB_ON);
    SDL_ShowCursor( SDL_DISABLE );
    g_MOUSE_MODE = LOCKM;
}
