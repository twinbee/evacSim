
#ifndef BB_H
#define BB_H

#include "vector.h"
#include "polygon2.h"

class CBoundingBox
{
  public:
    CBoundingBox();
    CBoundingBox( std::vector <CVector> & Vert_list);
    CBoundingBox( std::vector <CPolygon> & Poly_list);
    ~CBoundingBox();
    void Engulf(CVector & P);
    CVector GetMin();
    CVector GetMax();
    CVector GetCenter();
    void Clear();
    void Draw();   
    void FromPolyList( std::vector <CPolygon> & Poly_list);
    void FromVertList( std::vector <CVector> & Vert_list);
    
    void PrintInfo();
    
  private:
    CVector Min;
    CVector Max;
    bool empty;
  
};

#endif
