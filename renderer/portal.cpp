/******************************************************************************
** portal.cpp, portal.h
** Primary author: Mike J
** Description: a portal is a one-way pass between two adjacent regions
** Used in sim: pathing, pathing map
******************************************************************************/

#include "portal.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "convert.h"
#include <SDL.h>
#include <SDL_ttf.h>


CPortal::CPortal(CVector  p1, CVector  p2, int DestinationIndex, CVector  up, CColor pcolor = GREEN)
{
    bClosed = false;
    vPoints[0] = p1;
    vPoints[1] = p2;
    Normal = ((p2 - p1)).UnitVector() ^ up;  //up is the normal of the region this portal belongs to
    Width = (p2 - p1).Length();
    DestIndex = DestinationIndex;
    MidPoint = (vPoints[0] + vPoints[1]) / 2;
    color = pcolor;

    label = 0;

    bottleneck = false;
    //remove this! only for testing purposes
//    SetLabel("fuck");
}


void CPortal::Close(int WallIndex2)
{
    WallIndex = WallIndex2;
    bClosed = true;
    color = PINK;
}

void CPortal::Open()
{
    WallIndex = -1;
    bClosed = false;
}

int CPortal::GetWallIndex()
{
    return WallIndex;
}

CVector CPortal::GetPoint1()
{
    return vPoints[0];
}

CVector CPortal::GetPoint2()
{
    return vPoints[1];
}

bool CPortal::IsClosed()
{
    return bClosed;
}

int CPortal::GetDest()
{
    return DestIndex;
}

float CPortal::GetWidth()
{
    return Width;
}

CVector CPortal::GetNormal()
{
    return Normal;
}

CColor CPortal::GetColor()
{
    return color;
}

void CPortal::SetColor(CColor pcolor)
{
    color = pcolor;
}

void CPortal::Draw(CCamera * Cam)
{
    bool bDrawNormal = false;
    glColor3f(color.r(),color.g(),color.b());
    //draw the portal edge
    glBegin(GL_LINES);
    glVertex3f(vPoints[0].x,vPoints[0].y,vPoints[0].z);
    glVertex3f(vPoints[1].x,vPoints[1].y,vPoints[1].z);
    glEnd();

    //draw the normal
    if (bDrawNormal)
    {
        glColor3f(0,0,1);
        CVector NormalStart = (vPoints[0] + vPoints[1]) / 2.0;
        CVector NormalEnd = NormalStart + Normal;

        glBegin(GL_LINES);
        glVertex3f(NormalStart.x,NormalStart.y,NormalStart.z);
        glVertex3f(NormalEnd.x,NormalEnd.y,NormalEnd.z);
        glEnd();
    }

    if (label && Cam) label->Draw(Cam);

    glColor3f(1,1,1);
}


CPortal::~CPortal()
{
    if (label) delete label;
    return;
}

CVector CPortal::GetMidPoint()
{
    return MidPoint;
}

void CPortal::SetLabel(char * text)
{
    if (label) delete label;

    SDL_Color sdlcolor;
    sdlcolor.r = 255;
    sdlcolor.g = 255;
    sdlcolor.b = 255;

    CVector labelpos(MidPoint.x,MidPoint.y,MidPoint.z + .5);
    label = new CBillboard(text,sdlcolor,2,1, labelpos);
}

void CPortal::ClearLabel()
{
    if (label) delete label;
    label = 0;
}
