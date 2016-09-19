/**********************************************************
** script.cpp
** Primary Author: Matthew B
** Description: script support !!
*********************************************************/

#include "script.h"

#include "main.h"

#include "stringbuffer.h"

#include "textbuffer.h"

#include <fstream>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

void loadScript(std::string filename)
{
    const bool debug = false;

    ifstream s;
    s.open(filename.c_str() );

    while (s)
    {
        char * temp = new char[1000];
        s.getline(temp, 1000, '\n');
        scriptCommands.push_back(string(temp));
    }

    if (debug) textbuffer.InsertLine("> Script loaded <");

    s.close();

}

///////////////////////////////////////////////////////////////////////////////
void runScript()
{
    if (!g_HOLD && !scriptCommands.empty() )
    {
        stringbuffer.setstr( scriptCommands.at(0) );
        EvaluatePrompt();
        scriptCommands.erase( scriptCommands.begin() );
    }
}

///////////////////////////////////////////////////////////////////////////////
void startupScript()
{
    stringbuffer.setstr("script startup.txt");
    EvaluatePrompt();
    stringbuffer.setstr("script motd.txt");
    EvaluatePrompt();

    return;
}

///////////////////////////////////////

