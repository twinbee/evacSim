#include "plane.h"

using namespace std;

CPlane::CPlane(CVector & P, CVector & N)
{
    Normal = N;
    Pos = P; 
    
    if(Normal.Length() != 1.0) Normal.Normalize();
    
    if((Normal.x == 1.0) || (Normal.x == -1.0)) AlignmentFlag = AA_DIVIDE_X;
    else if((Normal.y == 1.0) || (Normal.y == -1.0)) AlignmentFlag = AA_DIVIDE_Y;
    else if((Normal.z == 1.0) || (Normal.z == -1.0)) AlignmentFlag = AA_DIVIDE_Z;
    else AlignmentFlag = NOT_AA;
}

CPlane::CPlane(CVector & P, int Alignment)
{
    Normal.Set(0.0,0.0,0.0);
    Pos = P; 
    AlignmentFlag = Alignment;
  
    if(AlignmentFlag == AA_DIVIDE_X) Normal.x = 1.0;
    else if(AlignmentFlag == AA_DIVIDE_Y) Normal.y = 1.0;
    else if(AlignmentFlag == AA_DIVIDE_Z) Normal.z = 1.0;
    else Normal.Set(1,0,0);
}

CPlane::~CPlane() {}

float CPlane::DistanceFrom(CVector & P)
{
  // if(AlignmentFlag == NOT_AA)
      return (P - Pos) % Normal; 
/*   else 
   {
      if(AlignmentFlag == AA_DIVIDE_X) return P.x - Pos.x;
      else if(AlignmentFlag == AA_DIVIDE_Y) return P.y - Pos.y;
      else if(AlignmentFlag == AA_DIVIDE_Z) return P.z - Pos.z;   
   }*/
}

int CPlane::ClassifyPolygon(CPolygon & Polygon)
{
    //for each vertex in this polygon
    bool One_In_Front = false;
    bool One_In_Back = false;

    vector<CVector> Verts = Polygon.GetVertices();

    for (int v=0; v< Verts.size(); v++)
    {
        float result = DistanceFrom(Verts[v]);

        if ( result > 0 )
            One_In_Front = true;
        else if ( result < 0)
            One_In_Back = true;

        if (One_In_Front && One_In_Back) return P_SPANNING;
    }

    if (One_In_Front) return P_FRONT;
    else if (One_In_Back) return P_BACK;
    else return P_COINCIDENT; 
}

void CPlane::Split_Polygon(CPolygon & Polygon, CPolygon & Front, CPolygon & Back)
{

    vector<CVector> Vertices = Polygon.GetVertices();
    vector<CVector> FrontVerts;
    vector<CVector> BackVerts;

    int  count = Vertices.size();
    CVector * ptA;
    CVector * ptB;
    float sideA, sideB;

    ptA = &Vertices[count - 1];
    sideA = DistanceFrom(*ptA);                                                                                          //sideA = ClassifyPpart->Classify_Point (ptA);
    for (int i = 0; i < count; i++)
    {
        ptB = &Vertices[i];
        sideB = DistanceFrom(*ptB);
        if (sideB > 0)
        {
            if (sideA < 0)
            {

                CVector v = *ptB - *ptA;
                float sect = -1 * ((*ptA - Pos) % Normal) / (Normal % v);

                FrontVerts.push_back(*ptA + (v * sect));
                BackVerts.push_back(*ptA + (v * sect));
            }

            FrontVerts.push_back(*ptB);
        }
        else if (sideB < 0)
        {
            if (sideA > 0)
            {



                CVector v = *ptB - *ptA;
                float sect = -1 * ((*ptA - Pos) % Normal) / (Normal % v);
                FrontVerts.push_back(*ptA + (v * sect));
                BackVerts.push_back(*ptA + (v * sect));
            }


            BackVerts.push_back(*ptB);
        }
        else
        {


            BackVerts.push_back(*ptB);
            FrontVerts.push_back(*ptB);
        }

        ptA = ptB;
        sideA = sideB;
    }
    Front.Create(FrontVerts);
    Back.Create(BackVerts);  
    
    int randnum = rand();
    Front.SetColor(ALLCOLORS[randnum % NUM_ALLCOLORS]);
    Back.SetColor(ALLCOLORS[(randnum + 1) % NUM_ALLCOLORS]);
}

void CPlane::PrintInfo()
{
  cout<<"Plane Pos : "<<Pos<<endl;
  cout<<"Plane Normal : "<<Normal<<endl;
  
}
      
