#ifndef PHYSICAL_H
#define PHYSICAL_H

#include <ode/ode.h>

#include "vector.h"

#include "physworld.h"

class Physical
{
 public:

  Physical(bool m=false);

  ~Physical();

  CVector GetLinearVel();
 float GetSpeed();

 virtual CVector GetPos();

 virtual int getClass();

 virtual void Injure(float a) {};

 virtual void Physical::Drive(CVector force);

 protected:
  bool moveable;

  dBodyID o_bod;
  dGeomID o_geom;
};

#endif
