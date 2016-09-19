/*********************************************************
** boundingbox.cpp, boundingbox.h
** Primary author: Mike E
** Description: calculates a bounding box for 3d geometry data (vertices)
** Used in sim: to bound the detail map
*********************************************************/


#include "BoundingBox.h"
#include <cmath>
#include <algorithm>

using namespace std;


BoundingBox_::BoundingBox_()
        : empty( true )
{ }

void BoundingBox_::rectify()
{
    if ( not empty )
    {
        if ( xmin > xmax )
            swap( xmin, xmax );

        if ( ymin > ymax )
            swap( ymin, ymax );

        if ( zmin > zmax )
            swap( zmin, zmax );
    }
}

void BoundingBox_::expand()
{
    if ( not empty )
    {
        xmin = floorf( xmin );
        xmax = ceilf( xmax );
        ymin = floorf( ymin );
        ymax = ceilf( ymax );
        zmin = floorf( zmin );
        zmax = ceilf( zmax );
    }
}

void BoundingBox_::engulf( const Point& p )
{
    if ( empty )
    {
        xmin = xmax = p.x;
        ymin = ymax = p.y;
        zmin = zmax = p.z;
        empty = false;
    }
    else
    {
        if ( p.x < xmin )
            xmin = p.x;
        else if ( p.x > xmax )
            xmax = p.x;

        if ( p.y < ymin )
            ymin = p.y;
        else if ( p.y > ymax )
            ymax = p.y;

        if ( p.z < zmin )
            zmin = p.z;
        else if ( p.z > zmax )
            zmax = p.z;
    }
}

bool BoundingBox_::contains( const Point& p ) const
{
    return ( not empty ) and
           ( p.x >= xmin and p.x <= xmax ) and
           ( p.y >= ymin and p.y <= ymax ) and
           ( p.z >= zmin and p.z <= zmax );
}

Point BoundingBox_::center() const
{
    return Point( 0.5f * ( xmin + xmax ),
                  0.5f * ( ymin + ymax ),
                  0.5f * ( zmin + zmax ));
}

float BoundingBox_::radius() const
{
    return ( center() - Point( xmin, ymin, zmin )).magnitude();
}


