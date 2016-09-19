#include "physworld.h"

#include "shrapnel.h"

#include "displaylists.h"

Shrapnel::~Shrapnel()
{
 //physics body and geom are destroyed in ~Physical

}

Shrapnel::Shrapnel(CVector pos, float r )
{
    moveable = true;

    //create the ODE body
    o_bod = dBodyCreate (W);
    dBodySetPosition(o_bod,pos.x,pos.y,pos.z);

    radius = r;

    o_geom = dCreateSphere(S,radius);

    geom2Physical[o_geom] = this;
    
    dGeomSetCategoryBits (o_geom,CAT_SHRAPNEL );
    dGeomSetCollideBits (o_geom,CBIT_SHRAPNEL );

    dGeomSetBody(o_geom,o_bod);

   dMass mass;
   dMassSetSphere (&mass, 1, radius); 
   dBodySetMass(o_bod, &mass);

}

void Shrapnel::Draw()
{

  glEnable(GL_LIGHTING);

  glColor3f(0.6, 0.3, 0.3);

    CVector poss = GetPos();
       
    glPushMatrix();
    glTranslatef(poss.x,poss.y,poss.z);

    glScalef(radius, radius, radius);

    glCallList(sphereDL);
    glPopMatrix();

}
