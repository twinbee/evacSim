/**********************************************************
** frustrum.h, frustrum.cpp
** Primary Author: Mike J
** Description: holds a bounding volume for a view frustum
** Used in sim: frustum culling
*********************************************************/

#ifndef FRUSTRUM_H
#define FRUSTRUM_H

#include "vector.h"

enum {COMPLETE_OUT, INTERSECT, COMPLETE_IN, FIRST_OBJECT_ID};

// We create an enum of the sides so we don't have to call each side 0 or 1.
// This way it makes it more understandable and readable when dealing with frustum sides.
enum FrustumSide
{
    RIGHT	= 0,		// The RIGHT side of the frustum
    LEFT	= 1,		// The LEFT	 side of the frustum
    BOTTOM	= 2,		// The BOTTOM side of the frustum
    TOP		= 3,		// The TOP side of the frustum
    BACK	= 4,		// The BACK	side of the frustum
    FRONT	= 5			// The FRONT side of the frustum
};

// Like above, instead of saying a number for the ABC and D of the plane, we
// want to be more descriptive.
enum PlaneData
{
    A = 0,				// The X value of the plane's normal
    B = 1,				// The Y value of the plane's normal
    C = 2,				// The Z value of the plane's normal
    D = 3				// The distance the plane is from the origin
};

class CFrustrum
{

public:
    CFrustrum();
    ~CFrustrum();
    void CalculateFrustrum();
    bool PointInFrustrum(float,float,float);
    bool SphereInFrustrum(float,float,float,float);
    bool CubeInFrustrum(float,float,float,float);
    int BoxInFrustum(float,float,float,float,float,float);
    int BoxInFrustum(CVector *);

private:

    // This holds the A B C and D values for each side of our frustum.
    float m_Frustum[6][4];
    int MinMax[6][2];  //holds the dynamic min/max index for each plane
    //these are computed once when calculate frustrum is called
    //and are used on every AABB check for this frame



};

#endif
