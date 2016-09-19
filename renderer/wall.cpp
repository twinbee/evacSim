/******************************************************************************
** wall.cpp, wall.h
** Primary author: Mike J, Matthew B
** Description: a wall is the edge of a region where no region is adjacent
**  may or may not have some physical contraint
** Used in sim: pathing, pathing map
******************************************************************************/


#include "wall.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>

// #include "utility.h"
#include "draw.h"

#include "physworld.h"


using namespace std;

Wall::Wall(CVector P1, CVector P2, CVector Up, Material in_mat)
{
    const bool debug = false;

    bool physical = true;

    moveable = false;

    if (debug) cout << "Wall ctor\n";
    Normal = (Up ^ (P1 - P2)).UnitVector() ;
    
	 mat = in_mat;
    vPoints[0] = P1;
    vPoints[1] = P2;

    //initialize ODE stuff

    CVector temp = P1 - P2;
    width  = 0.1;
    height = 0.5;

    length = temp.Length();

    Center = (P1 + P2) / 2;

    CVector offset = Up;
    offset |= height/2; //normalize offset to half the height
    Center+= offset;

	 if (physical) 
    {
	  if (length <= 0.00001) return;
		//in feet; do not make physical component of wall
     
     CVector E1 = (vPoints[0] + vPoints[1]) / 2.0;
     CVector E2 = vPoints[1];
     CVector axis2 = (E2-E1);

     if (axis2.Length() && Normal.Length() && Normal != axis2 && Normal != -1*axis2 )
     {
	  	 if (debug) cout << "Gone physical for : " << axis2 << " " << Normal << endl;
       dMatrix3 R;
		   o_geom = dCreateBox(S,length,width,height);
	     dGeomSetPosition(o_geom,Center.x,Center.y,Center.z);
       dRFrom2Axes (R, axis2.x, axis2.y, axis2.z, Normal.x, Normal.y, Normal.z);
       dGeomSetRotation(o_geom,R);
  		 
  		 dGeomSetCategoryBits (o_geom,CAT_WALL );
       dGeomSetCollideBits (o_geom,CBIT_WALL );
     }
   }               

}//end Wall()

Wall::~Wall()
{
 //if (bPhysical == true)  dGeomDestroy(o_geom);
}

CVector Wall::GetNormal()
{
    return Normal;
}

CVector Wall::GetPoint1()
{
    return vPoints[0];
}

CVector Wall::GetPoint2()
{
    return vPoints[1];
}

void Wall::SetMaterial(Material in_mat)
{
    mat = in_mat;
}

float Wall::GetLength()
{
    return length;
}

void Wall::Draw()
{

    mat.set();

        const dReal * pos = dGeomGetPosition(o_geom);
        const dReal * rot = dGeomGetRotation(o_geom);

        glPushMatrix();

        float out[16];

        ODE2GL(pos, rot, out);

        glMultMatrixf(out);

        glBegin(GL_QUADS);
        /*
            //draw wall normal
            glPushMatrix();
             CVector center = GetCentroid();
             glColor3f(0,1,0);
             glBegin(GL_LINES);
              glVertex3f(center.x, center.y, center.z);
              glVertex3f(center.x + Normal.x, center.y + Normal.y, center.z + Normal.z);
             glEnd();
            glPopMatrix();
        */
        //x,y,z+ (front)
        glVertex3f(-(length/2),width/2,height/2);
        glVertex3f(-(length/2),-(width/2),height/2);
        glVertex3f(length/2,-(width/2),height/2);
        glVertex3f(length/2,width/2,height/2);

        //x,y,z- (back)
        glVertex3f(-(length/2),width/2,-height/2);
        glVertex3f(-(length/2),-(width/2),-height/2);
        glVertex3f(length/2,-(width/2),-height/2);
        glVertex3f(length/2,width/2,-height/2);

        //x,y+,z (top)
        glVertex3f(-(length/2),width/2,-(height/2));
        glVertex3f(length/2,width/2,-(height/2));
        glVertex3f(length/2,(width/2),(height/2));
        glVertex3f(-(length/2),(width/2),(height/2));

        //x,y-,z (bottom)
        glVertex3f(-(length/2),-(width/2),-(height/2));
        glVertex3f(length/2,-(width/2),-(height/2));
        glVertex3f(length/2,-(width/2),(height/2));
        glVertex3f(-(length/2),-(width/2),(height/2));

        //x+,y,z (right)
        glVertex3f((length/2),(width/2),-(height/2));
        glVertex3f(length/2,-(width/2),-(height/2));
        glVertex3f(length/2,-(width/2),(height/2));
        glVertex3f((length/2),(width/2),(height/2));

        //x-,y,z (left)
        glVertex3f(-(length/2),(width/2),-(height/2));
        glVertex3f(-(length/2),-(width/2),-(height/2));
        glVertex3f(-(length/2),-(width/2),(height/2));
        glVertex3f(-(length/2),(width/2),(height/2));

        glEnd();

        glPopMatrix();

}//end Wall::Draw()
