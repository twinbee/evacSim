/**********************************************************
** frustrum.h, frustrum.cpp
** Primary Author: Mike J
** Description: holds a bounding volume for a view frustum
** Used in sim: frustum culling
*********************************************************/


#include "frustrum.h"

#include <GL/gl.h>      // Header File For The OpenGL32 Library
#include <GL/glu.h>      // Header File For The GLu32 Library

#include <cmath>
#include <iostream>

void NormalizePlane(float frustum[6][4], int side)
{
    // Here we calculate the magnitude of the normal to the plane (point A B C)
    // Remember that (A, B, C) is that same thing as the normal's (X, Y, Z).
    // To calculate magnitude you use the equation:  magnitude = sqrt( x^2 + y^2 + z^2)
    float magnitude = (float)sqrt( frustum[side][A] * frustum[side][A] +
                                   frustum[side][B] * frustum[side][B] +
                                   frustum[side][C] * frustum[side][C] );

    // Then we divide the plane's values by it's magnitude.
    // This makes it easier to work with.
    frustum[side][A] /= magnitude;
    frustum[side][B] /= magnitude;
    frustum[side][C] /= magnitude;
    frustum[side][D] /= magnitude;
}



CFrustrum::CFrustrum()
{}

CFrustrum::~CFrustrum()
{}



void CFrustrum::CalculateFrustrum()
{
    float   proj[16];        // This will hold our projection matrix
    float   modl[16];        // This will hold our modelview matrix
    float   clip[16];        // This will hold the clipping planes

    // glGetFloatv() is used to extract information about our OpenGL world.
    // Below, we pass in GL_PROJECTION_MATRIX to abstract our projection matrix.
    // It then stores the matrix into an array of [16].
    glGetFloatv( GL_PROJECTION_MATRIX, proj );

    // By passing in GL_MODELVIEW_MATRIX, we can abstract our model view matrix.
    // This also stores it in an array of [16].
    glGetFloatv( GL_MODELVIEW_MATRIX, modl );

    // Now that we have our modelview and projection matrix, if we combine these 2 matrices,
    // it will give us our clipping planes.  To combine 2 matrices, we multiply them.

    clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
    clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
    clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
    clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

    clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
    clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
    clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
    clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

    clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
    clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
    clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
    clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

    clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
    clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
    clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
    clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

    // Now we actually want to get the sides of the frustum.  To do this we take
    // the clipping planes we received above and extract the sides from them.

    // This will extract the RIGHT side of the frustum
    m_Frustum[RIGHT][A] = clip[ 3] - clip[ 0];
    m_Frustum[RIGHT][B] = clip[ 7] - clip[ 4];
    m_Frustum[RIGHT][C] = clip[11] - clip[ 8];
    m_Frustum[RIGHT][D] = clip[15] - clip[12];

    // Now that we have a normal (A,B,C) and a distance (D) to the plane,
    // we want to normalize that normal and distance.

    // Normalize the RIGHT side
    NormalizePlane(m_Frustum, RIGHT);

    // This will extract the LEFT side of the frustum
    m_Frustum[LEFT][A] = clip[ 3] + clip[ 0];
    m_Frustum[LEFT][B] = clip[ 7] + clip[ 4];
    m_Frustum[LEFT][C] = clip[11] + clip[ 8];
    m_Frustum[LEFT][D] = clip[15] + clip[12];

    // Normalize the LEFT side
    NormalizePlane(m_Frustum, LEFT);

    // This will extract the BOTTOM side of the frustum
    m_Frustum[BOTTOM][A] = clip[ 3] + clip[ 1];
    m_Frustum[BOTTOM][B] = clip[ 7] + clip[ 5];
    m_Frustum[BOTTOM][C] = clip[11] + clip[ 9];
    m_Frustum[BOTTOM][D] = clip[15] + clip[13];

    // Normalize the BOTTOM side
    NormalizePlane(m_Frustum, BOTTOM);

    // This will extract the TOP side of the frustum
    m_Frustum[TOP][A] = clip[ 3] - clip[ 1];
    m_Frustum[TOP][B] = clip[ 7] - clip[ 5];
    m_Frustum[TOP][C] = clip[11] - clip[ 9];
    m_Frustum[TOP][D] = clip[15] - clip[13];

    // Normalize the TOP side
    NormalizePlane(m_Frustum, TOP);

    // This will extract the BACK side of the frustum
    m_Frustum[BACK][A] = clip[ 3] - clip[ 2];
    m_Frustum[BACK][B] = clip[ 7] - clip[ 6];
    m_Frustum[BACK][C] = clip[11] - clip[10];
    m_Frustum[BACK][D] = clip[15] - clip[14];

    // Normalize the BACK side
    NormalizePlane(m_Frustum, BACK);

    // This will extract the FRONT side of the frustum
    m_Frustum[FRONT][A] = clip[ 3] + clip[ 2];
    m_Frustum[FRONT][B] = clip[ 7] + clip[ 6];
    m_Frustum[FRONT][C] = clip[11] + clip[10];
    m_Frustum[FRONT][D] = clip[15] + clip[14];

    // Normalize the FRONT side
    NormalizePlane(m_Frustum, FRONT);

    //CALCULATE THE MIN/MAX AABB VALUES FOR EACH OF THE SIX PLANES.
    //THE MIN/MAX VALUES ARE EACH STORED AS A SINGLE INT THAT IS AN INDEX INTO THE 8 VERTEXS OF AN AABB
    //THE AABB VERTEXS ARE CONSIDERED TO BE INDEXED IN A CERTAIN ORDER FROM 0 TO 7, BEING THAT ALL AABB ARE
    //ALIGNED EXACTLY ALIKE, THESE MIN/MAX INDICIES WILL WORK ON ALL AABB IN THE PROGRAM
    for (int x=0; x<6; x++)
    {
        int index = 0;
        index  = (m_Frustum[x][A] > 0.0f) ? 1 : 0;
        index |= (m_Frustum[x][B] > 0.0f) ? 2 : 0;
        index |= (m_Frustum[x][C] > 0.0f) ? 4 : 0;

        MinMax[x][1] = index;
        MinMax[x][0] = index ^ 7;
    }

}


bool CFrustrum::PointInFrustrum( float x, float y, float z )
{
    // Go through all the sides of the frustum
    for (int i = 0; i < 6; i++ )
    {
        // Calculate the plane equation and check if the point is behind a side of the frustum
        if (m_Frustum[i][A] * x + m_Frustum[i][B] * y + m_Frustum[i][C] * z + m_Frustum[i][D] <= 0)
        {
            // The point was behind a side, so it ISN'T in the frustum
            return false;
        }
    }

    // The point was inside of the frustum (In front of ALL the sides of the frustum)
    return true;
}

bool CFrustrum::SphereInFrustrum( float x, float y, float z, float radius )
{
    // Go through all the sides of the frustum
    for (int i = 0; i < 6; i++ )
    {
        // If the center of the sphere is farther away from the plane than the radius
        if ( m_Frustum[i][A] * x + m_Frustum[i][B] * y + m_Frustum[i][C] * z + m_Frustum[i][D] <= -radius )
        {
            // The distance was greater than the radius so the sphere is outside of the frustum
            return false;
        }
    }

    // The sphere was inside of the frustum!
    return true;
}

bool CFrustrum::CubeInFrustrum( float x, float y, float z, float size )
{
    // Basically, what is going on is, that we are given the center of the cube,
    // and half the length.  Think of it like a radius.  Then we checking each point
    // in the cube and seeing if it is inside the frustum.  If a point is found in front
    // of a side, then we skip to the next side.  If we get to a plane that does NOT have
    // a point in front of it, then it will return false.

    // *Note* - This will sometimes say that a cube is inside the frustum when it isn't.
    // This happens when all the corners of the bounding box are not behind any one plane.
    // This is rare and shouldn't effect the overall rendering speed.

    for (int i = 0; i < 6; i++ )
    {
        if (m_Frustum[i][A] * (x - size) + m_Frustum[i][B] * (y - size) + m_Frustum[i][C] * (z - size) + m_Frustum[i][D] > 0)
            continue;
        if (m_Frustum[i][A] * (x + size) + m_Frustum[i][B] * (y - size) + m_Frustum[i][C] * (z - size) + m_Frustum[i][D] > 0)
            continue;
        if (m_Frustum[i][A] * (x - size) + m_Frustum[i][B] * (y + size) + m_Frustum[i][C] * (z - size) + m_Frustum[i][D] > 0)
            continue;
        if (m_Frustum[i][A] * (x + size) + m_Frustum[i][B] * (y + size) + m_Frustum[i][C] * (z - size) + m_Frustum[i][D] > 0)
            continue;
        if (m_Frustum[i][A] * (x - size) + m_Frustum[i][B] * (y - size) + m_Frustum[i][C] * (z + size) + m_Frustum[i][D] > 0)
            continue;
        if (m_Frustum[i][A] * (x + size) + m_Frustum[i][B] * (y - size) + m_Frustum[i][C] * (z + size) + m_Frustum[i][D] > 0)
            continue;
        if (m_Frustum[i][A] * (x - size) + m_Frustum[i][B] * (y + size) + m_Frustum[i][C] * (z + size) + m_Frustum[i][D] > 0)
            continue;
        if (m_Frustum[i][A] * (x + size) + m_Frustum[i][B] * (y + size) + m_Frustum[i][C] * (z + size) + m_Frustum[i][D] > 0)
            continue;

        // If we get here, it isn't in the frustum
        return false;
    }

    return true;
}

#define V_IN 1
#define V_OUT 2
#define V_INTERSECT 3

int CFrustrum::BoxInFrustum(CVector * AABB)
{
    int MIN;
    int MAX;
    int mode = 0;
    for (int i = 0; i < 6; i++)
    {
        MIN  = MinMax[i][0];
        MAX  = MinMax[i][1];

        mode &= V_OUT; //set mode to v_out as a starting point for checking this plane
        if (m_Frustum[i][A] * AABB[MIN].x + m_Frustum[i][B] * AABB[MIN].y + m_Frustum[i][C] * AABB[MIN].z + m_Frustum[i][D] >= 0) mode |= V_IN;
        else mode |= V_OUT;
        if (mode == V_INTERSECT) continue;

        if (m_Frustum[i][A] * AABB[MAX].x + m_Frustum[i][B] * AABB[MAX].y + m_Frustum[i][C] * AABB[MAX].z + m_Frustum[i][D] >= 0) mode |= V_IN;
        else mode |= V_OUT;
        if (mode == V_INTERSECT) continue;

        if (mode == V_IN) continue;  //isn't intersecting, so must be completely in or completely out.
        //if it's completely in this plane, it still might intersect with other planes, so move on and check next plane
        // if the In bit hasn't been turned on at all, it was completely outside the plane, and thus outside the frustum.
        return COMPLETE_OUT;
    }

    // All planes has a vertex IN FRONT so or the box is intersecting or complete IN
    if (mode == V_INTERSECT) return INTERSECT;
    else return COMPLETE_IN;

}

int CFrustrum::BoxInFrustum( float x, float y, float z, float x2, float y2, float z2)
{


    // Detailed explanation on the BSP tutorial 4
    int mode=0;     // set IN and OUT bit to 0
    for (int i = 0; i < 6; i++ )
    {
        // cout<<"SIDE "<<i<<endl;
        mode &= V_OUT;    // clear the IN bit to 0
        if (m_Frustum[i][A] * x  + m_Frustum[i][B] * y  + m_Frustum[i][C] * z  + m_Frustum[i][D] >= 0)    mode |= V_IN; // set IN bit to 1
        else mode |= V_OUT; // set OUT bit to 1
        if (mode == V_INTERSECT) continue;  // if we found a vertex IN for THIS plane and
        // a vertex OUT of ANY plane continue ( we have enough information to say:
        // INTERSECT! IF there is not vertex missing from the FRONT of the remaining planes)
        // cout<<m_Frustum[i][A] * x  + m_Frustum[i][B] * y  + m_Frustum[i][C] * z  + m_Frustum[i][D]<<endl;

        if (m_Frustum[i][A] * x2 + m_Frustum[i][B] * y  + m_Frustum[i][C] * z  + m_Frustum[i][D] >= 0)  mode |= V_IN;
        else mode |= V_OUT;
        if (mode == V_INTERSECT) continue;
        // cout<<m_Frustum[i][A] * x2 + m_Frustum[i][B] * y  + m_Frustum[i][C] * z  + m_Frustum[i][D]<<endl;

        if (m_Frustum[i][A] * x  + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z  + m_Frustum[i][D] >= 0)  mode |= V_IN;
        else mode |= V_OUT;
        if (mode == V_INTERSECT) continue;
        //  cout<<m_Frustum[i][A] * x  + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z  + m_Frustum[i][D]<<endl;

        if (m_Frustum[i][A] * x2 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z  + m_Frustum[i][D] >= 0)  mode |= V_IN;
        else mode |= V_OUT;
        if (mode == V_INTERSECT) continue;
        //      cout<<m_Frustum[i][A] * x2 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z  + m_Frustum[i][D]<<endl;
//
        if (m_Frustum[i][A] * x  + m_Frustum[i][B] * y  + m_Frustum[i][C] * z2 + m_Frustum[i][D] >= 0)  mode |= V_IN;
        else mode |= V_OUT;
        if (mode == V_INTERSECT) continue;
        // cout<<m_Frustum[i][A] * x  + m_Frustum[i][B] * y  + m_Frustum[i][C] * z2 + m_Frustum[i][D]<<endl;

        if (m_Frustum[i][A] * x2 + m_Frustum[i][B] * y  + m_Frustum[i][C] * z2 + m_Frustum[i][D] >= 0)  mode |= V_IN;
        else mode |= V_OUT;
        if (mode == V_INTERSECT) continue;

        if (m_Frustum[i][A] * x  + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z2 + m_Frustum[i][D] >= 0)  mode |= V_IN;
        else mode |= V_OUT;
        if (mode == V_INTERSECT) continue;

        if (m_Frustum[i][A] * x2 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z2 + m_Frustum[i][D] >= 0)  mode |= V_IN;
        else mode |= V_OUT;
        if (mode == V_INTERSECT) continue;

        // if we arrive to this point, then there are two possibilities:
        // there is not vertices in or there is not intersection till know, if
        // there is a vertice in, continue (we are not over!)
        if (mode == V_IN) continue;

        // there is not vertex IN front of this plane, so the box is COMPLETE_OUT
        return COMPLETE_OUT;
    }

    // All planes has a vertex IN FRONT so or the box is intersecting or complete IN
    if (mode == V_INTERSECT) return INTERSECT;
    else return COMPLETE_IN;

}






