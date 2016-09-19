/*********************************************************
** line.cpp, line.h
** Primary author: Mike J
** Description: line
** Used in sim: represents portals geometrically
*********************************************************/

#ifndef LINE_H
#define LINE_H

#include "vector.h"
#include "color.h"
#include <GL/gl.h>
#include <GL/glu.h>

class CLine
{
public:
    CLine(CVector p1, CVector p2, CColor lcolor);
    ~CLine();
    void Draw();

private:
    CColor color;
    CVector P1,P2;
};

#endif
