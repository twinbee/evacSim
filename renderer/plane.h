
#ifndef PLANE_H
#define PLANE_H

#include "vector.h"
#include "polygon2.h"

//axis alignment flags
enum{AA_DIVIDE_X = 0 ,AA_DIVIDE_Y = 1,AA_DIVIDE_Z = 2, NOT_AA = 3};
//Flags for describing a polygon's position relative to the plane
enum{P_FRONT = 0, P_BACK, P_SPANNING, P_COINCIDENT};

class CPlane
{
    public :
      CPlane(CVector & P, CVector & N);
      CPlane(CVector & P, int Alignment);
      ~CPlane();
      float DistanceFrom(CVector & P);  //+ value is in front of plane, - behind plane, and 0 coincident with plane
      int ClassifyPolygon(CPolygon & Polygon);
      void Split_Polygon(CPolygon & Polygon, CPolygon & Front, CPolygon & Back);
      void PrintInfo();
      
    private :
      CVector Normal;
      CVector Pos;
      int AlignmentFlag;
};

#endif
