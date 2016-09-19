



#include "physical.h"



  Physical::Physical(bool m)
  {
   moveable = m;
   geom2Physical[o_geom] = this;
  };


  Physical::~Physical()
  { if (moveable) dBodyDestroy (o_bod);
    dGeomDestroy (o_geom);
   }


float Physical::GetSpeed()
{
 return GetLinearVel().Length(); 
}

CVector Physical::GetLinearVel()
{
    if (moveable)
    {
        const dReal * vel = dBodyGetLinearVel(o_bod);
        return CVector (vel[0],vel[1],vel[2]);
    }
    else
    {
        return CVector (0,0,0);
    }
}

 CVector Physical::GetPos()
 {
    const dReal * poss = dGeomGetPosition (o_geom);
   
    return CVector(poss[0],poss[1],poss[2]);
 }

 int Physical::getClass()
 { return 0; }

void Physical::Drive(CVector force)
{
 if (moveable) dBodyAddForce(o_bod, force.x, force.y, force.z);
}
