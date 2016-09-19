

#ifndef BSPTREE_H
#define BSPTREE_H
#include <vector>
#include "region.h"
#include "BB.h"
#include "BSPnode.h"

//temporary tree structures used for creating the initial tree

struct Ray
{
  CVector src;
  CVector dst;
  CColor color;  
};

class CBSPtree
{
  public:
    CBSPtree(const char * filename, int DesiredLeafSize);
    ~CBSPtree(); 
    
    void Draw(CFrustrum & Frustum, bool DrawBounds = false);
    CVector GetCenter();
    bool Obstructed(CVector & start, CVector & end);
    void AddRay(CVector start, CVector end);
    
  private:
    CBSPnode * RootNode;
    CBoundingBox BB;
     std::vector<Ray> Ray_list;

};

#endif
