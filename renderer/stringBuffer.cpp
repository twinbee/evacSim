/*********************************************************
** stringbuffer.cpp / stringbuffer.h
** Primary author: Matthew B
** Description: Simple buffer to get and place input
** Used in sim: to type the current line of input in the console
*********************************************************/

#include "stringBuffer.h"

#include <cstring>
#include <string>

CStringBuffer::CStringBuffer(int rhs)
{
    size = rhs;

    stringData = new char [size];
    stringData[curPos = 0] = '\0';
}

CStringBuffer::CStringBuffer(const char *  rhs)
{
    int in_size = strlen(rhs);
    size = 3*in_size;
    stringData = new char [size];

    strcpy(stringData, rhs);
    strcat(stringData, " ");
    curPos = in_size;
}

void CStringBuffer::setstr(const char *  rhs)
{
    int in_size = strlen(rhs);
    if (in_size > size)
    {
        delete[] stringData;
        size = 3*in_size;
        stringData = new char [size];
    }

    strcpy(stringData, rhs);
    strcat(stringData, " ");
    curPos = in_size;
}

void CStringBuffer::setstr(std::string rhs)
{
    int in_size = rhs.size();
    if (in_size > size)
    {
        delete[] stringData;
        size = 3*in_size;
        stringData = new char [size];
    }

    strcpy(stringData, rhs.c_str());
    strcat(stringData, " ");
    curPos = in_size;
}

CStringBuffer::~CStringBuffer()
{
    delete [] stringData;
}

char * CStringBuffer::getData()
{
    return stringData;
}

int CStringBuffer::getCursorPos()
{
    return curPos;
}


void CStringBuffer::clear()
{
    curPos = 0;
    stringData[curPos] = '\0';
}

void CStringBuffer::AddChar(const char & a)
{
    stringData[curPos] = a;
    if (curPos < size) curPos++;
    stringData[curPos] = '\0';
}

void CStringBuffer::RemoveChar()
{
    stringData[curPos-1] = '\0';
    if (curPos > 0)
    {
        curPos--;
    }
}
