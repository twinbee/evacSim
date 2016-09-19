/*********************************************************
** line.cpp, line.h
** Primary author: Mike J
** Description: line
** Used in sim: represents portals geometrically
*********************************************************/

#include "line.h"


CLine::CLine(CVector p1, CVector p2, CColor lcolor)
{
    P1 = p1;
    P2 = p2;
    color = lcolor;
}

CLine::~CLine(){}

void CLine::Draw()
{
    glColor3f(color.r(),color.g(),color.b());
    glBegin(GL_LINES);
    glVertex3f(P1.x,P1.x,P1.z);
    glVertex3f(P2.x,P2.y,P2.z);
    glEnd();
    glColor3f(1,1,1);
}
