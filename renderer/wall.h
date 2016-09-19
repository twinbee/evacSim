/******************************************************************************
** wall.cpp, wall.h
** Primary author: Mike J, Matthew B
** Description: a wall is the edge of a region where no region is adjacent
**  may or may not have some physical contraint
** Used in sim: pathing, pathing map
******************************************************************************/

#ifndef _WALL_H
#define _WALL_H

#include "vector.h"
#include "color.h"
#include "material.h"

#include "physworld.h"

#include "physical.h"

class Wall : public Physical
{
public:
    Wall(CVector P1, CVector P2, CVector Up, Material in_mat);

    ~Wall();
    CVector GetNormal();
    void Draw();
    CVector GetPoint1();
    CVector GetPoint2();

    inline CVector GetCentroid() {
        return Center;
    }

    void SetMaterial(Material in_mat);
    float GetLength();

    int getClass () {return 3;}

private:
    CVector vPoints[2];
    CVector Normal;
    CVector Center;

    Material mat;

    float length;
    float height;
    float width;
};

#endif
