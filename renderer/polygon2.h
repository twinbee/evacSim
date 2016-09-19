/******************************************************************************
** polygon2.cpp, polygon2.h
** Primary author: Matthew B, Mike J
** Description: polygon2 for pathing map
** Used in sim: type of poly owned by a region of the pathing map. geometry data
******************************************************************************/

#ifndef usm_polygon
#define usm_polygon

#include "vector.h"
#include "Color.h"
#include "Material.h"
#include "TexCoord.h"

#include "BoundingBox.h"

#include <vector>


class CPolygon
{
public:
    CPolygon();
    CPolygon( const std::vector<CVector>& Verts, float snaptol = 0.0);
    void Create( const std::vector<CVector>& Verts, float snaptol = 0.0);

    //modification functions
    void addVertex( const CVector& );
    void addTexCoord( const TexCoord& );
    void SetColor( const CColor& );
    void SetMaterial( const Material& );

    void Draw() const;
    void computeAll(float snaptol);

    //accessor functions
    const std::vector<CVector>& GetVertices() const;
    CVector GetCentroid() const;
    CVector GetNormal() const;
    float GetArea() const;
    int NumVerts() const;
    bool IsConvex() const;
    bool IsPlanar() const;
    bool IsVertical() const;
    bool IsLevel() const;
    BoundingBox GetBoundingBox() const;
    CColor GetColor() const;
    const Material& GetMaterial() const;

    //queries
    bool BBIntersects( CPolygon & PolyB) const;
    bool ContainsCircle(float radius, const CVector & pos, const CVector Up) const;
    bool IntersectsRay(CVector & start, CVector & end);
    bool PlaneRayIntersection(CVector & start, CVector & end, CVector & result);
    bool OverlapsXY(CPolygon * B);
    bool operator==(CPolygon & B) const;

    //debug
    void PrintVerts();

private:

    void CullRedundantVerts(float threshhold);
    void ComputeArea();
    void ComputeNormal();
    void ComputeCentroid();
    void ComputeBoundingBox();
    void ComputeIncline();
    void ComputeConvex();
    void ComputePlanar(float tolerance);


    //DATA
    std::vector<CVector> Vertices;          //vertex list for this polygon
    CVector Centroid;                  //Midpoint of the polygon
    CVector Normal;
    float Area;                        //the area of this region

    //quality of our polygon
    bool bPlanar;
    bool bConvex;
    bool bVertical;  //true if vertical
    bool bLevel;

    std::vector<TexCoord> texCoords;
    Material material;
    CColor color;
    BoundingBox BB;

};

#endif
