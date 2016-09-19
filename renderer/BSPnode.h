
#include "BB.h"
#include <vector>
#include "polygon2.h"
#include "plane.h"
#include "frustrum.h"

class CBSPnode
{
  public:
    CBSPnode( std::vector<CPolygon> & Polygon_list, int DesiredSize = 50, int Alignment = AA_DIVIDE_X, int depth = 0); //create a leaf node
    ~CBSPnode();
    bool IsLeaf();
    void PrintInfo();
    void Draw(CFrustrum & Frustum, bool DrawBounds = false, bool testing = true );
    bool RayNodeCollision(CVector & start, CVector & end);
    
  private: 
    bool RayLeafCollision(CVector & start, CVector & end);
    void DividePolygons( std::vector <CPolygon> & Polygon_list,  std::vector<CPolygon> & Front_list,  std::vector<CPolygon> & Back_list);
     std::vector<CPolygon> Polygons;
    CPlane   *       SplitPlane;
    CBSPnode *       Front;
    CBSPnode *       Back;
    CBoundingBox     BB;
    int NodeDepth;
};
