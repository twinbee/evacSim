#ifndef OCCLUSION_H
#define OCCLUSION_H
#include "BSPCompile.h"
#include "vector.h"
#include "frustrum.h"
#include "color.h"

struct Ray
{
  CVector src;
  CVector dst;
  CColor color;  
};

class COcclusionMap
{
  public:
    COcclusionMap(const char * Filename);
    ~COcclusionMap();
    bool Obstructed(CVector & start, CVector & end);
    void Draw(CFrustrum & Frustum);
    CVector GetCenter();
    
    //testing
    void AddRay(CVector start,CVector end);
    
  private:
    t_TempNode * Tree;  
    void TraceRay(t_TempNode * CurrentNode,CVector & start, CVector & end, bool & collided);
    bool LeafCollision(t_TempNode * CurrentNode, CVector & start, CVector & end);
    vector<Ray> Ray_list;
    t_BoundingBox BB;
};

#endif
