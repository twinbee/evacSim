/*********************************************************
** parse.cpp, parse.h
** Primary author: Mike J
** Description: file loaders
** Used in sim: pathing map load
*********************************************************/

#ifndef PARSE_H
#define PARSE_H

#include "region.h"
#include "polygon2.h"

#include <ode/ode.h>
#include <map>

struct MapVertex  //used anymore?
{
    long int ID; //4 bytes
    float x; //4 bytes
    float y;
    float z;
    short int IsPortal;  //2 bytes, used as a flag
};

bool LoadPolyListOBJ( std::vector<CPolygon> & Polygon_list, const char * filename);
bool LoadRegionListOBJ( std::vector<CRegion*>& Region_list, const char* filename);
bool ComputeBBFromOBJ(const char* filename, BoundingBox & BB);

bool GetBBFromNAV(const char* filename, BoundingBox& BB);
bool OBJtoNAV(char* OBJFilename, char* NAVFilename);
bool LoadRegionListNAV( std::vector<CVector>& Vertices,  std::vector<CRegion*>& Region_list, const char* filename,  std::vector< std::vector<float> >& D,  std::vector< std::vector<int> >& P);
bool WriteNAV(char* NAVFilename, BoundingBox& BB,  std::vector<CRegion*>& Region_List,  std::vector< std::vector<float> >& D,  std::vector< std::vector<int> >& P);

#endif
