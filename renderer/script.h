/**********************************************************
** script.h
** Primary Author: Matthew B
** Description: script support for command prompt
*********************************************************/
#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>

///////////////////////////////////////////////

void loadScript(std::string filename);
void runScript();
void startupScript();

void HandleCommandLine(int argc, char ** argv);

#endif
