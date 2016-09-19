/**********************************************************
** BSPCompile.h, BSPCompile.cpp
** Primary Author: Mike J
** Description: Creates a BSP to speed up rendering and other processes
** Used in sim: might be out of use ??
*********************************************************/


#ifndef BSPCOMPILE_H
#define BSPCOMPILE_H

#include <vector>

#include "vector.h"
#include "frustrum.h"
#include "polygon2.h"
#include "region.h"
#include "bitset.h"
#include <GL/gl.h>
#include "camera.h"

using namespace std;

#define ALIGNX 0
#define ALIGNY 1
#define ALIGNZ 2

struct t_HyperPlane
{
    CVector Pos;
    CVector Normal;
};

struct t_BoundingBox
{
    float vMax[3];
    float vMin[3];
};

//temporary tree structures used for creating the initial tree
struct t_TempNode
{
    int Axis;
    float SplittingValue;
    t_HyperPlane HyperPlane;
    vector<CPolygon>  Polygons;
    t_TempNode *       Front;
    t_TempNode *       Back;
    t_BoundingBox      BoundingBox;
    GLuint ListID;
};

enum {KD_X = 0,KD_Y,KD_Z};

struct t_KDNode
{
    int Axis;
    float SplittingValue;
    t_KDNode * Front;
    t_KDNode * Back;
    vector <CRegion *> Regions;
    t_BoundingBox      BoundingBox;
    GLuint ListID;
    t_KDNode() {
        Front = NULL;
        Back = NULL;
    }
};


//end temporary structures

#define IN_FRONT_OF 0
#define COINCIDENT  1
#define IN_BACK_OF  2
#define SPANNING    3


int ClassifyPoly( CPolygon & ,const t_HyperPlane & );
int ClassifyPoly(CPolygon &, int, float);


void Split_Polygon(CPolygon & Polygon, t_HyperPlane & HyperPlane, CPolygon & Front, CPolygon & Back);

t_TempNode * Create_BSP_Tree(vector<CPolygon> & Polygon_list);
void Build_BSP_Tree(t_TempNode * Tree, vector<CPolygon> Polygon_list,int flag);
t_KDNode * Create_KD_Tree(vector< CRegion *> & Region_list);

void Build_KD_Tree(t_KDNode * Tree, vector<CRegion *>  Region_list, int axis);
t_BoundingBox MakeBoundingBox(vector<CPolygon> & Polygon_list);
t_BoundingBox MakeBoundingBox(vector<CRegion *> & Region_list);
void RenderBoundingBox(t_BoundingBox & BBox);
void DeleteTree(t_TempNode * root);
void DeleteTree(t_KDNode * root);
void RenderTree(t_TempNode * root,bool DrawBounds,CFrustrum & Frustum,bool testing,int &PolysDrawn);
void RenderTree(CCamera * Cam,t_KDNode * root,bool DrawBounds,CFrustrum & Frustum, bool testing, int &RegionsDrawn, CBitset & Drawn, float FrameTime);
CVector Ray_Plane_Intersect(CVector & start, CVector & end, t_HyperPlane & HyperPlane);
void TreeCreateDisplayLists(t_TempNode * root);
void RenderTreeLists(t_TempNode * root,bool DrawBounds, CFrustrum & Frustum,bool testing, int &PolysDrawn);
void CullDuplicatePolys(t_TempNode * root);
#endif
