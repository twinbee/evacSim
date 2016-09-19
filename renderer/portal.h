/******************************************************************************
** portal.cpp, portal.h
** Primary author: Mike J
** Description: a portal is a one-way pass between two adjacent regions
** Used in sim: pathing, pathing map
******************************************************************************/


#ifndef _PORTAL_H
#define _PORTAL_H

#include "vector.h"
#include "color.h"
#include "billboard.h"

struct PortalInfo
{
    CVector P1,P2;
    int DestinationIndex;
    CVector Up;
    CColor color;
};

class CPortal
{
public:
    CPortal(CVector  p1, CVector  p2, int DestinationIndex, CVector up, CColor pcolor);
    ~CPortal();

    //actions

    void Draw(CCamera * Cam = 0); //draws with a label if one exists
    void Close(int WallIndex2);
    void Open();
    void SetColor(CColor pcolor);
    void SetLabel(char * text);
    void ClearLabel();

    //accessor functions, so the portal can't be altered
    bool IsClosed();
    int GetDest();
    float GetWidth();
    CVector GetPoint1();
    CVector GetPoint2();
    CVector GetNormal();
    CVector GetMidPoint();
    int GetWallIndex();
    CColor GetColor();
    bool bottleneck;

private:
    bool bClosed;                 //is it closed or open?
    CVector vPoints[2];           //verticies of the portal
    CVector Normal;
    CVector MidPoint;
    float Width;
    int DestIndex;
    int WallIndex;                //if it's closed, which wall corresponds to this?
    CColor color;
    CBillboard * label;

};

#endif

