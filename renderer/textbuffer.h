/*********************************************************
** textbuffer.cpp / textbuffer.h
** Primary author: Matthew B
** Description: Simple buffer to hold multi-strings
** Used in sim: to keep a command history in the console
*********************************************************/


#ifndef TEXTBUFFER_H
#define TEXTBUFFER_H

enum {COMMAND, COMMENT};

#include <vector>
#include <string>

class CTextBuffer
{
private:
    int scrollrow;

    std::vector<std::string> commands;
    std::vector<std::string> outputs;

public:
    CTextBuffer();

    void Clear();

    void NextCommand();
    void PrevCommand();

    char * getOutputLineCast(int n);

    std::string getCommand();
    std::string getOutputLine(int n);

    int numOutputLines();

    void InsertLine(std::string, int mode = COMMENT);

    void tag(char *);

};

#endif
