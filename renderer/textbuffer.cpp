/*********************************************************
** textbuffer.cpp / textbuffer.h
** Primary author: Matthew B
** Description: Simple buffer to hold multi-strings
** Used in sim: to keep a command history in the console
*********************************************************/


#include "textbuffer.h"

CTextBuffer::CTextBuffer()
{
    scrollrow = 0;
}//end CTextBuffer()

void CTextBuffer::Clear()
{
    commands.clear();
    outputs.clear();
    scrollrow = 0;
}//end Clear()

void CTextBuffer::NextCommand()
{
    scrollrow = (scrollrow + 1) % commands.size();
}

void CTextBuffer::PrevCommand()
{
    scrollrow = (scrollrow - 1);
    if (scrollrow < 0) scrollrow = commands.size()-1;
}


std::string CTextBuffer::getCommand()
{
    return (commands[scrollrow]);
}


void CTextBuffer::InsertLine(std::string line, int mode)
{
    switch (mode)
    {
    case COMMAND:
        commands.push_back(line);

    case COMMENT:
        outputs.push_back(line);
        break;
    }
}

std::string CTextBuffer::getOutputLine(int n)
{
    if (n <= outputs.size() && n > 0)
        return (outputs[n-1]);
    else return std::string("");
}

char * CTextBuffer::getOutputLineCast(int n)
{
    if (n <= outputs.size() && n > 0)
        return (const_cast<char *> (outputs[n-1].c_str()));
    else return "\0";
}


int CTextBuffer::numOutputLines()
{
    return outputs.size();
}

void CTextBuffer::tag(char * tag)
{
    int n = numOutputLines();
    std::string newlie = getOutputLine(n);
    outputs.pop_back();
    newlie += tag;
    outputs.push_back(newlie);
}
