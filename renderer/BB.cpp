#include "BB.h"
#include <vector>

using namespace std;

CBoundingBox::CBoundingBox()
{
    empty = true;
}

CBoundingBox::CBoundingBox(vector <CVector> & Vert_list)
{
    FromVertList(Vert_list);
}

CBoundingBox::CBoundingBox(vector <CPolygon> & Poly_list)
{
    FromPolyList(Poly_list);
}

CBoundingBox::~CBoundingBox()
{
  
}

void CBoundingBox::FromPolyList(vector <CPolygon> & Poly_list)
{
    empty = true;
    
    for(int p=0; p<Poly_list.size(); p++)
    {
      vector<CVector> Vert_list = Poly_list[p].GetVertices();
      
      for(int v=0; v<Vert_list.size(); v++)
        Engulf(Vert_list[v]);       
    }  
}

void CBoundingBox::FromVertList(vector <CVector> & Vert_list)
{
    empty = true;
        
    for(int v=0; v<Vert_list.size(); v++)
      Engulf(Vert_list[v]);  
}


void CBoundingBox::Clear()
{
  empty = true;
}

void CBoundingBox::Engulf(CVector & P)
{
        if(empty) 
        {
            Min = P;
            Max = P;
            empty = false;
            return;
        }

        if(P.x < Min.x) Min.x = P.x;
        if(P.x > Max.x) Max.x = P.x;  
        
        if(P.y < Min.y) Min.y = P.y;
        if(P.y > Max.y) Max.y = P.y;
        
        if(P.z < Min.z) Min.z = P.z;
        if(P.z > Max.z) Max.z = P.z;     
}

CVector CBoundingBox::GetMin()
{
  return Min; 
}
CVector CBoundingBox::GetMax()
{
  return Max;
}

CVector CBoundingBox::GetCenter()
{
  return (Min + Max) / 2.0;  
}

void CBoundingBox::Draw()
{
    glColor3f(1,0,0);
    glBegin(GL_LINES);
    
    glVertex3f(Min[0], Min[1], Min[2]); //bottom front
    glVertex3f(Max[0], Min[1], Min[2]);

    glVertex3f(Min[0],Min[1], Min[2]); //left front
    glVertex3f(Min[0],Min[1], Max[2]);

    glVertex3f(Max[0], Min[1],  Min[2]); //right front
    glVertex3f( Max[0], Min[1],  Max[2]);

    glVertex3f( Min[0], Min[1],  Max[2]); //top front
    glVertex3f( Max[0], Min[1],  Max[2]);


    glVertex3f( Min[0], Max[1],  Min[2]); //bottom back
    glVertex3f( Max[0], Max[1],  Min[2]);

    glVertex3f( Min[0], Max[1],  Min[2]); //left back
    glVertex3f( Min[0], Max[1],  Max[2]);

    glVertex3f( Max[0], Max[1],  Min[2]); //right back
    glVertex3f( Max[0], Max[1],  Max[2]);

    glVertex3f( Min[0], Max[1],  Max[2]); //top back
    glVertex3f( Max[0], Max[1],  Max[2]);


    glVertex3f( Min[0], Min[1],  Min[2]); //left bottom side
    glVertex3f( Min[0], Max[1],  Min[2]);

    glVertex3f( Min[0], Min[1],  Max[2]); //left top side
    glVertex3f( Min[0], Max[1],  Max[2]);

    glVertex3f( Max[0], Min[1],  Min[2]); //right bottom side
    glVertex3f( Max[0], Max[1],  Min[2]);

    glVertex3f( Max[0], Min[1],  Max[2]); //right top side
    glVertex3f( Max[0], Max[1],  Max[2]);
    
    glEnd();  
  
}

void CBoundingBox::PrintInfo()
{
    cout<<"Min : "<<Min<<endl;
    cout<<"Max : "<<Max<<endl;   
}
