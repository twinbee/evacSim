/*********************************************************
** boundingbox.cpp, boundingbox.h
** Primary author: Mike E
** Description: calculates a bounding box for 3d geometry data (vertices)
** Used in sim: to bound the detail map
*********************************************************/


#ifndef mce_bounding_box
#define mce_bounding_box

#include "Vec3.h"

struct BoundingBox_
{
    float xmin, xmax;
    float ymin, ymax;
    float zmin, zmax;

    bool empty;

    //-------------------------------
    // constructor
    BoundingBox_();

    // mutators
    void rectify();
    void expand();		// snap to more permissive integer boundaries
    void engulf( const Point& );	// grow to include this point

    // queries
    bool contains( const Point& ) const;
    Point center() const;
    float radius() const;
};

//todo: merge the two bounding box representations/get rid of the struct
struct BoundingBox
{
    float minx;
    float miny;
    float minz;
    float maxx;
    float maxy;
    float maxz;
    float lx;    //length along x axis
    float ly;
    float lz;
};

#endif
