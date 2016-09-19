
#include "physworld.h"

#include "physical.h"

#include "entity.h"
#include <iostream>


#include "BoundingBox.h"

dWorldID W;

dSpaceID S;

dJointGroupID contacts;

std::map <dGeomID, Physical *> geom2Physical;

void initPhysics(const BoundingBox & BB)
{
    dVector3 MapCenter = {(BB.maxx + BB.minx)/2, (BB.maxy + BB.miny)/2, (BB.maxz + BB.minz)/2};
    dVector3 Extents = {BB.maxx - MapCenter[0], BB.maxy - MapCenter[1], BB.maxz - MapCenter[2]};

    S = dQuadTreeSpaceCreate (0, MapCenter, Extents, 12);
    //S = dSimpleSpaceCreate(0);

    W = dWorldCreate();

    //dWorldSetGravity (W, 0, 0, -9.80);
    //dWorldSetGravity (W, 0, 0, -32.0);

    contacts = dJointGroupCreate(0); //create an empty group of contacts for sotring contact joints
}

void uninitPhysics()
{
    //destroy the geometries
    dSpaceDestroy(S);
    dJointGroupDestroy(contacts);
    dWorldDestroy(W); //make sure this is destroyed LAST
}

void updatePhysics(const double & Frametime)
{
    std::cout<<"DEBUG dWorldQuickStep begin."<<std::endl;
    dWorldQuickStep (W, Frametime);
    //dWorldStep (W, FrameTime); //slower, more accurate
    std::cout<<"DEBUG dJointGroupEmpty begin."<<std::endl;
    dJointGroupEmpty(contacts); //empty the list of joint contacts
}

void nearCallback(void *unused, dGeomID o1, dGeomID o2) {
const bool debug = true;
//--------------------start broken
    Physical * obj1  = geom2Physical[o1];
    Physical * obj2  = geom2Physical[o2];

    if (obj1 && obj2)
    if ( obj1->getClass() == ENTITY && obj2->getClass() == SHRAPNEL )
    {
     obj1->Injure(obj2->GetSpeed());
    }
    else if ( obj1->getClass() == SHRAPNEL && obj2->getClass() == ENTITY )
    {
     obj2->Injure(obj1->GetSpeed());
    }


//-----------------------end broken


	dBodyID body1 = dGeomGetBody(o1);
	dBodyID body2 = dGeomGetBody(o2);
	
//------------------------start broken 
    if (body1 && body2 && dAreConnectedExcluding(body1,body2,dJointTypeContact)) return;
//------------------------end broken
     
    const int MAX_CONTACTS = 1;
    
	dContact contact[MAX_CONTACTS];
 
/*	for (int i = 0; i < MAX_CONTACTS; i++) {
		contact[i].surface.mode = dContactBounce; // Bouncy surface
		contact[i].surface.bounce = 0.5;
		contact[i].surface.mu = 100.0; // Friction
	}*/
    for (int i=0; i<MAX_CONTACTS; i++)
    {
        contact[i].surface.mode = dContactBounce;
        contact[i].surface.mu = 0.1;
        contact[i].surface.bounce = 0.5;
        //contact[i].surface.bounce_vel = 0.5;
    }

    int cat1 = dGeomGetCategoryBits (o1);
    int cat2 = dGeomGetCategoryBits (o2);
    int col1 = dGeomGetCollideBits (o1);
    int col2 = dGeomGetCollideBits (o2);
    if ((cat1 & col2) || (cat2 & col1))  
    {
	   int collisions = dCollide(o1, o2, MAX_CONTACTS, &contact[0].geom, sizeof(dContact));
	   if (collisions) 
       {
		  for (int i = 0; i < collisions; ++i) 
          {
			dJointID c = dJointCreateContact(W, contacts, contact + i);
			dJointAttach(c, body1, body2);
		  }
	   }
     }
}
