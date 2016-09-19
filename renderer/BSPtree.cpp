#include "BSPtree.h"
#include "parse.h"
#include <vector>
#include "vector.h"

using namespace std;

CBSPtree::CBSPtree(const char * filename, int DesiredLeafSize)
{
    vector< CPolygon > Polygon_list;
    LoadPolyListOBJ(Polygon_list,filename);
    BB.FromPolyList(Polygon_list);
    RootNode = new CBSPnode(Polygon_list,DesiredLeafSize);  
}

CBSPtree::~CBSPtree()
{
    if(RootNode) delete RootNode;
}

void CBSPtree::Draw(CFrustrum & Frustum, bool DrawBounds)
{
   RootNode->Draw(Frustum,DrawBounds);
   
     //draw the debug rays
  for(int r=0; r<Ray_list.size(); r++)
  {
    glColor3f(Ray_list[r].color.r(),Ray_list[r].color.g(),Ray_list[r].color.b());
    glBegin(GL_LINES);
      glVertex3f(Ray_list[r].src.x,Ray_list[r].src.y,Ray_list[r].src.z);
      glVertex3f(Ray_list[r].dst.x,Ray_list[r].dst.y,Ray_list[r].dst.z);
    glEnd();  
    
  }
}

CVector CBSPtree::GetCenter()
{
  return BB.GetCenter();
}

bool CBSPtree::Obstructed(CVector & start, CVector & end)
{
     return RootNode->RayNodeCollision(start,end); 
}


void CBSPtree::AddRay(CVector start, CVector end)
{ 
  Ray r;
  r.src = start;
  r.dst = end;
  if(Obstructed(start, end)) r.color = RED;
  else r.color = BLUE;
  Ray_list.push_back(r);
  
}
