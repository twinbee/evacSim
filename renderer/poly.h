/*********************************************************
** poly.cpp, poly.h
** Primary author: Mike E
** Description: generic polygon functions
** Used in sim: all over
*********************************************************/


#ifndef usm_poly
#define usm_poly

#include "Material.h"
#include "polygon2.h"
#include "line.h"
#include <vector>
#include <map>

void set( const CVector& v );

// function to fill poly list from OBJ file
void polygonsRead(
    std::map<std::string,Material>& materials,
    std::vector<CVector>& vertices,
    std::vector<CLine*>& lines,
    std::vector<CPolygon*>& polygons,
    const char* path );

// void polygonsReport( const std::vector<Polygon>& );

#endif
