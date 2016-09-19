/******************************************************************************
** polygon2.cpp, polygon2.h
** Primary author: Matthew B, Mike J
** Description: polygon2 for pathing map
** Used in sim: type of poly owned by a region of the pathing map. geometry data
******************************************************************************/

#include "polygon2.h"
#include "poly.h"
#include "draw.h"
#include "convert.h"
#include "Image.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>

using namespace std;

CPolygon::CPolygon( const vector<CVector>& Verts , float snaptol)
        : Vertices( Verts )
{
    computeAll(snaptol);
    //repair
}

CPolygon::CPolygon()
{
}

void CPolygon::CullRedundantVerts(float threshhold)
{
   // float threshhold = 0.0;  //.5 feet
    //snap close vertices
    //ELEMINATE DUPLICATE VERTICES
    int v = 0;
    while (v < Vertices.size())
    {
        if ((Vertices[v] - Vertices[(v+1) % Vertices.size()]).Length() <= threshhold )
        {
            Vertices[(v+1) % Vertices.size()] = (Vertices[(v+1) % Vertices.size()] + Vertices[v]) / 2.0;
            Vertices.erase(Vertices.begin() + v);
        }
        else
            v++;
    }
}

CColor CPolygon::GetColor() const
{
    return convert( material.diffuse );
//  return color;
}

void CPolygon::SetColor( const CColor& c )
{
//  color = c;
    material.diffuse = convert( c );
}

const Material& CPolygon::GetMaterial() const
{
    return material;
}

void CPolygon::SetMaterial( const Material& m )
{
    material = m;
}

void CPolygon::Create( const vector<CVector>& Verts, float snaptol)
{
    Vertices = Verts;
    computeAll(snaptol);
}

void CPolygon::addVertex( const CVector& v )
{
    Vertices.push_back( v );
}

void CPolygon::addTexCoord( const TexCoord& t )
{
    texCoords.push_back( t );
}

int CPolygon::NumVerts() const
{
    return Vertices.size();
}

void CPolygon::computeAll(float snaptol)
{
    CullRedundantVerts(snaptol);
    ComputeNormal();
    ComputeConvex();
    ComputeArea();
    ComputeCentroid();
    ComputeBoundingBox();
    ComputeIncline();
}


void CPolygon::ComputeConvex()
{
    bConvex = true;
    CVector Up(0,0,1.0);

    //check each edge of the polygon
    for (int v = 0; v< Vertices.size(); v++)
    {
        CVector EdgeStart = Vertices[v];
        CVector EdgeEnd = Vertices[(v + 1) % Vertices.size()];
        CVector EdgeNormal = Up ^ (EdgeEnd - EdgeStart).UnitVector();

        //test all other vertices to make sure they are coplanar or in front of this plane
        for (int v2=0; v2<Vertices.size(); v2++)
        {
            if ((Vertices[v2] == EdgeStart) || (Vertices[v2] == EdgeEnd)) continue;

            float Side = (((Vertices[v2]) - EdgeStart) % EdgeNormal);
            if (Side < 0.0)
            {
                bConvex = false;
                return;
            }
        }
    }
}




void CPolygon::ComputePlanar(float tolerance)
{
// % is the dot product of two vectors (scalar)
// ^ is the cross product of two vectors (vector)
    int n = Vertices.size();

    if ((n > 0) && (n < 4))
    {
        bPlanar = true;
        return;
    }

    if (n >= 4)
    {
        for (int i = 3; i < n; i++)
        {
            float distancefromplane = (Vertices[0] - Vertices[i]) %
                                      ((Vertices[1] - Vertices[0]).UnitVector() ^ (Vertices[2] - Vertices[0]).UnitVector());

            if ((distancefromplane < -tolerance) || (distancefromplane > tolerance))
            {
                bPlanar = false;
                return;
            }
        }
    }

    bPlanar = true;
}//end computeplanar



void CPolygon::ComputeCentroid()
{
    //not tested

    CVector R(0,0,0);
    float A;
    CVector Sum1(0,0,0);
    float Sum2=0;

    int n = Vertices.size();
    if (n < 3)
    {
        Centroid.Set(0,0,0);
        return;
    }

    for (int i=2; i < n; i++)
    {
        R = (Vertices[0] + Vertices[i-1] + Vertices[i]) / 3.0;
        A = ((Vertices[i-1] - Vertices[0]) ^ (Vertices[i] - Vertices[i-1])).Length();
        Sum1 += A * R;
        Sum2 += A;
    }

    Centroid = Sum1 / Sum2;
}//end ComputeMidpoint

void CPolygon::ComputeArea()
{
    int n = Vertices.size();
    if (n < 3)
    {
        Area = 0;
        return;
    }

    Area = 0;
    for (int p=2; p < n; p++)
        Area+= ((Vertices[p-1] - Vertices[0]) ^ (Vertices[p] - Vertices[p-1])).Length();

    Area = Area/2.0;
}//end computearea

void CPolygon::ComputeNormal()
{
    if (Vertices.size() < 3)
    {
        Normal.Set(0,0,1);
        return;
    }

    int NumVerts = Vertices.size();

    for (int x=0; x < Vertices.size(); x++)
    {
        CVector edge1 = (Vertices[(x+1) % NumVerts] - Vertices[x]).UnitVector();
        CVector edge2 = (Vertices[(x+2) % NumVerts] -  Vertices[(x+1) % NumVerts]).UnitVector();

        float dotproduct = edge1 % edge2;

        if ( ((dotproduct) < -.996) || ((dotproduct) > .996) ) continue;
        Normal =  edge2^edge1;
        if (Normal.Length() > 0) break;
    }

    Normal.Normalize();
    Normal *= -1;

    //this part marks the polys if the winding is bad


    CVector Up(0,0,1);
    CVector Down(0,0,-1);
    CVector Diff1 = Up - Normal;
    CVector Diff2 = Down - Normal;
    if (Diff2.Length() < Diff1.Length())
    {
        Normal *= -1;
        reverse(Vertices.begin(),Vertices.end());
    }


}//end computenormal

void CPolygon::Draw() const
{
// if ( material.texture )
// {
// enableTextures();
    if (DrawFlags & DRAW_SURFACE)
    {

        glBegin( GL_POLYGON );

        material.set();
//    glColor3f( color.r(), color.g(), color.b());
        glNormal3f(Normal.x, Normal.y, Normal.z);

        int n = Vertices.size();
        for ( int i = 0; i < n; ++i )
        {
            if ( material.texture )
                texCoords[i].set();
            set( Vertices[i]);
        }
        glEnd();
    }
// disableTextures();
// }


    if (DrawFlags & DRAW_NORMAL)
    {
        //draw the polygon normal
        glColor3f(0,0,1);
        glBegin(GL_LINES);
        glVertex3f(Centroid.x, Centroid.y, Centroid.z);
        glVertex3f(Centroid.x+Normal.x, Centroid.y+Normal.y, Centroid.z+Normal.z);
        glEnd();
    }

        if (DrawFlags & DRAW_BB)
        {
            CColor c = DRED;
            glColor3f(c.r(),c.g(),c.b());

            ///draw the bounding box
            glBegin(GL_LINES);

            glVertex3f(BB.minx,BB.miny,BB.minz); //000
            glVertex3f(BB.minx,BB.miny,BB.maxz); //001

            glVertex3f(BB.minx,BB.miny,BB.maxz); //001
            glVertex3f(BB.minx,BB.maxy,BB.maxz); //011

            glVertex3f(BB.minx,BB.maxy,BB.maxz); //011
            glVertex3f(BB.minx,BB.maxy,BB.minz); //010

            glVertex3f(BB.minx,BB.maxy,BB.minz); //010
            glVertex3f(BB.maxx,BB.maxy,BB.minz); //110

            glVertex3f(BB.maxx,BB.maxy,BB.minz); //110
            glVertex3f(BB.maxx,BB.maxy,BB.maxz); //111

            glVertex3f(BB.maxx,BB.maxy,BB.maxz); //111
            glVertex3f(BB.maxx,BB.miny,BB.maxz); //101

            glVertex3f(BB.maxx,BB.miny,BB.maxz); //101
            glVertex3f(BB.maxx,BB.miny,BB.minz); //100

            glVertex3f(BB.maxx,BB.miny,BB.minz); //100
            glVertex3f(BB.minx,BB.miny,BB.minz); //000
            //end grey codes

            glVertex3f(BB.minx,BB.miny,BB.minz); //000
            glVertex3f(BB.minx,BB.maxy,BB.minz); //010

            glVertex3f(BB.minx,BB.miny,BB.maxz); //001
            glVertex3f(BB.maxx,BB.miny,BB.maxz); //101

            glVertex3f(BB.minx,BB.maxy,BB.maxz); //011
            glVertex3f(BB.maxx,BB.maxy,BB.maxz); //111

            glVertex3f(BB.maxx,BB.miny,BB.minz); //100
            glVertex3f(BB.maxx,BB.maxy,BB.minz); //110

            glEnd();
        } //end draw bounding box


        if (DrawFlags & DRAW_OUTLINE)
          {
          glBegin( GL_LINE_LOOP );
          glColor3f(0.25,0.25,0.25);
          vector<CVector>::const_iterator i;
          for ( i = Vertices.begin(); i != Vertices.end(); ++i )
            set( *i );
          glEnd();
          }

}//END POLY DRAW

void CPolygon::ComputeIncline()
{
    bVertical = false;
    CVector Up(0,0,1);
    float incline = Normal % Up;
    if (incline < 0.3)  bVertical = true;
    if (incline == 1.0) bLevel = true;
}

bool CPolygon::IsLevel() const
{
    return bLevel;
}

bool CPolygon::IsVertical() const
{
    return bVertical;
}


CVector CPolygon::GetCentroid() const
{
    return Centroid;
}

CVector CPolygon::GetNormal() const
{
    return Normal;
}

float CPolygon::GetArea() const
{
    return Area;
}

bool CPolygon::IsConvex() const
{
    return bConvex;
}

bool CPolygon::IsPlanar() const
{
    return bPlanar;
}

const vector<CVector>& CPolygon::GetVertices() const
{
    return Vertices;
}

void CPolygon::ComputeBoundingBox()
{
    if (Vertices.size() < 1) return;

    BB.minx = Vertices[0].x;
    BB.miny = Vertices[0].y;
    BB.minz = Vertices[0].z;

    BB.maxx = Vertices[0].x;
    BB.maxy = Vertices[0].y;
    BB.maxz = Vertices[0].z;

    for (int v=1; v<Vertices.size(); v++)
    {
        if (BB.minx > Vertices[v].x)  BB.minx =   Vertices[v].x;
        if (BB.maxx < Vertices[v].x)  BB.maxx =   Vertices[v].x;
        if (BB.miny > Vertices[v].y)  BB.miny =   Vertices[v].y;
        if (BB.maxy < Vertices[v].y)  BB.maxy =   Vertices[v].y;
        if (BB.minz > Vertices[v].z)  BB.minz =   Vertices[v].z;
        if (BB.maxz < Vertices[v].z)  BB.maxz =   Vertices[v].z;
    }

    //compute the lengths along the axes
    BB.lx = BB.maxx - BB.minx;
    BB.ly = BB.maxy - BB.miny;
    BB.lz = BB.maxz - BB.minz;
}

BoundingBox CPolygon::GetBoundingBox() const
{
    return BB;
}

bool CPolygon::BBIntersects(CPolygon & PolyB) const
{
    BoundingBox BB2 = PolyB.GetBoundingBox();

    //test x axis
    float midx1 = (BB.maxx - BB.minx) /2;
    float radx1 = BB.maxx - midx1;
    float midx2 = (BB2.maxx - BB2.minx) / 2;
    float radx2 = BB2.maxx - midx2;
    float diffx = abs(midx1 - midx2);
    if (diffx > (radx1 + radx2)) return false;

    //test y axis
    float midy1 = (BB.maxy - BB.miny) / 2;
    float rady1 = BB.maxy - midy1;
    float midy2 = (BB2.maxy - BB2.miny) / 2;
    float rady2 = BB2.maxy - midy2;
    float diffy = abs(midy1 - midy2);
    if (diffy > (rady1 + rady2)) return false;

    //test z axis
    float midz1 = (BB.maxz - BB.minz) / 2;
    float radz1 = BB.maxz - midz1;
    float midz2 = (BB2.maxz - BB2.minz) / 2;
    float radz2 = BB2.maxz - midz2;
    float diffz = abs(midz1 - midz2);
    if (diffz > (radz1 + radz2)) return false;

    return true;
}

bool CPolygon::ContainsCircle(float radius, const CVector & pos, const CVector Up) const
{
    //check the BB first
    if (  pos.x + radius < BB.minx) return false;
    if (  pos.x - radius > BB.maxx) return false;
    if (  pos.y - radius > BB.maxy) return false;
    if (  pos.y + radius < BB.miny) return false;

    //check each edge of the polygon
    for (int v = 0; v< Vertices.size(); v++)
    {
        CVector EdgeStart = Vertices[v];
        CVector EdgeEnd = Vertices[(v + 1) % Vertices.size()];
        CVector EdgeNormal = Up ^ (EdgeEnd - EdgeStart).UnitVector();

        if ((((pos) - EdgeStart) % EdgeNormal)  <= radius) return false;
    }

    return true;
}

bool CPolygon::PlaneRayIntersection(CVector & start, CVector & end, CVector & result)
{
    float side1 = (Centroid - start) % Normal;  
    float side2 = (Centroid - end) % Normal;  
    
    if( (side1 > 0) && (side2 > 0)) return false;
    if( (side1 < 0) && (side2 < 0)) return false;
    if( (side1 == 0) && (side2 == 0)) return false; 
    
    CVector v = end - start;
    float sect = -1.0 * ((start - Centroid) % Normal) / (Normal % v);
    result = (start + (v * sect));
    return true;
}

bool CPolygon::IntersectsRay(CVector & start, CVector & end)
{
    CVector MappedPoint;
    if(PlaneRayIntersection(start,end,MappedPoint))
    {
       
        return ContainsCircle(0, MappedPoint, Normal);
    } 
    return false;
 
}


bool CPolygon::operator==(CPolygon & B) const
{
    vector<CVector> BVertices = B.GetVertices();

    if (BVertices.size() != Vertices.size()) return false;

    for (int x=0; x< Vertices.size(); x++)
        if (Vertices[x] != BVertices[x])
            return false;

    return true;
}

void CPolygon::PrintVerts()
{
    cout<<"Vert list ["<<Vertices.size()<<" verts]:"<<endl;
    for (int v=0; v<Vertices.size(); v++)
        cout<<Vertices[v]<<endl;
    cout<<"End Vert list"<<endl;

}

bool CPolygon::OverlapsXY(CPolygon * B)
{
    CVector Up(0,0,1);
    for (int v=0; v<Vertices.size(); v++)
    {
        if (B->ContainsCircle(0.2, Vertices[v],Up))
            return true;
    }
    vector< CVector> BVerts = B->GetVertices();
    for (int v=0; v< BVerts.size(); v++)
    {
        if (ContainsCircle(0.2, BVerts[v],Up))
            return true;
    }
    return false;
}
