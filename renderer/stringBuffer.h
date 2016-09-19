/*********************************************************
** stringbuffer.cpp / stringbuffer.h
** Primary author: Matthew B
** Description: Simple buffer to get and place input
** Used in sim: to type the current line of input in the console
*********************************************************/


#ifndef STRINGBUFFER_H
#define STRINGBUFFER_H

#include <string>

class CStringBuffer
{
public:
    CStringBuffer(int rhs_size = 100);
    CStringBuffer(const char *);
    ~CStringBuffer();

    void AddChar(const char & a);
    void RemoveChar();

    void clear();

    void setstr(const char * rhs);
    void setstr(std::string);

    char * getData();

    int getCursorPos();
    int getLastPos();

private:
    char * stringData;

    int size;

    int curPos;
};

#endif
