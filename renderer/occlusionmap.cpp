#include "occlusionmap.h"
#include "parse.h"
#include <vector>

#include <GL/gl.h>
#include <GL/glu.h>

COcclusionMap::COcclusionMap(const char * Filename)
{
    Tree = 0;
    vector<CPolygon> Polygon_list;

    if(LoadPolyListOBJ(Polygon_list,Filename))
    {
     BB = MakeBoundingBox(Polygon_list);
     Tree = Create_BSP_Tree(Polygon_list);
     
    }
     
}

CVector COcclusionMap::GetCenter()
{
  CVector B(((BB.vMin[0] + BB.vMax[0]) / 2.0),((BB.vMin[1] + BB.vMax[1]) / 2.0),((BB.vMin[2] + BB.vMax[2]) / 2.0));
  return B;
}

COcclusionMap::~COcclusionMap()
{
    if(Tree) DeleteTree(Tree);
}

bool COcclusionMap::Obstructed(CVector & start, CVector & end)
{
   bool collided = false;
   TraceRay(Tree,start,end,collided);
   return collided; 
}

void COcclusionMap::TraceRay(t_TempNode * CurrentNode,CVector & start, CVector & end, bool & collided)
{
    if(CurrentNode == 0) return;
    if(collided) return;
    
    //if this is a leaf node, check it and return;
    if((CurrentNode->Front == 0) && (CurrentNode->Back == 0))
    {
      if(LeafCollision(CurrentNode,start,end)) collided = true;
      return;
    }

    //branch control
    float SplittingValue = CurrentNode->SplittingValue;
    
    if(CurrentNode->Axis == ALIGNX)
    {
      if((start.x > SplittingValue) && (end.x > SplittingValue)) TraceRay(CurrentNode->Front,start,end,collided);
      if((start.x < SplittingValue) && (end.x < SplittingValue)) TraceRay(CurrentNode->Back,start,end,collided);
      if((end.x > SplittingValue) && (start.x < SplittingValue)) 
      {   
          TraceRay(CurrentNode->Front,start,end,collided);
          TraceRay(CurrentNode->Back,start,end,collided);
      }
      if((end.x < SplittingValue) && (start.x > SplittingValue)) 
      {   
          TraceRay(CurrentNode->Front,start,end,collided);
          TraceRay(CurrentNode->Back,start,end,collided);
      }
      
    }
    if(CurrentNode->Axis == ALIGNY)
    {
      if((start.y > SplittingValue) && (end.y > SplittingValue)) TraceRay(CurrentNode->Front,start,end,collided);
      if((start.y < SplittingValue) && (end.y < SplittingValue)) TraceRay(CurrentNode->Back,start,end,collided);
      if((end.y > SplittingValue) && (start.y < SplittingValue)) 
      {   
          TraceRay(CurrentNode->Front,start,end,collided);
          TraceRay(CurrentNode->Back,start,end,collided);
      }
      if((end.y < SplittingValue) && (start.y > SplittingValue)) 
      {   
          TraceRay(CurrentNode->Front,start,end,collided);
          TraceRay(CurrentNode->Back,start,end,collided);
      }  
      
    }
    if(CurrentNode->Axis == ALIGNZ)
    {
      if((start.z > SplittingValue) && (end.z > SplittingValue)) TraceRay(CurrentNode->Front,start,end,collided);
      if((start.z < SplittingValue) && (end.z < SplittingValue)) TraceRay(CurrentNode->Back,start,end,collided);
      if((end.z > SplittingValue) && (start.z < SplittingValue)) 
      {   
          TraceRay(CurrentNode->Front,start,end,collided);
          TraceRay(CurrentNode->Back,start,end,collided);
      }
      if((end.z < SplittingValue) && (start.z > SplittingValue)) 
      {   
          TraceRay(CurrentNode->Front,start,end,collided);
          TraceRay(CurrentNode->Back,start,end,collided);
      }          
    } 
}

bool COcclusionMap::LeafCollision(t_TempNode * CurrentNode, CVector & start, CVector & end)
{
    for(int p=0; p<CurrentNode->Polygons.size(); p++)
    {
        if(CurrentNode->Polygons[p].IntersectsRay(start,end)) return true;  
    }
    return false;  
}

void COcclusionMap::Draw(CFrustrum & Frustum)
{
  int Drawn = 0;
  RenderTree(Tree,true,Frustum,true,Drawn); 
  
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

void COcclusionMap::AddRay(CVector start, CVector end)
{
  Ray r;
  r.src = start;
  r.dst = end;
  if(Obstructed(start, end)) r.color = RED;
  else r.color = BLUE;
  Ray_list.push_back(r);
  
}
