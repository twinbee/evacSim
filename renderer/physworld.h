#ifndef PHYSWORLD_H
#define PHYSWORLD_H

#include <map>
#include <ode/ode.h>

enum {UNDEFINED = 0, ENTITY = 1, SHRAPNEL = 2, WALL = 3, FLOOR = 4}; //subclasses of physical for polymorphism

enum {CAT_ENTITY = 2,CAT_SHRAPNEL = 4, CAT_FLOOR = 8, CAT_WALL = 16};

const unsigned int CBIT_ENTITY = CAT_ENTITY + CAT_SHRAPNEL + CAT_WALL;
const unsigned int CBIT_SHRAPNEL = CAT_ENTITY + CAT_FLOOR;
const unsigned int CBIT_FLOOR = CAT_FLOOR;
const unsigned int CBIT_WALL = CAT_ENTITY;

class Physical;
extern std::map <dGeomID, Physical *> geom2Physical;

class BoundingBox;

extern int NumCollisions;

extern dJointGroupID contacts;

extern dWorldID W;

extern dSpaceID S;

extern dJointGroupID contacts;

void initPhysics(const BoundingBox & BB);
void uninitPhysics();

void updatePhysics(const double & Frametime);
void nearCallback (void * data, dGeomID o1, dGeomID o2);

inline void ODE2GL(const dReal *pos, const dReal * rot, float * out)
   {
   out[0]  = rot[0];
   out[1]  = rot[4];
   out[2]  = rot[8];
   out[3]  = 0;
   out[4]  = rot[1];
   out[5]  = rot[5];
   out[6]  = rot[9];
   out[7]  = 0;
   out[8]  = rot[2];
   out[9]  = rot[6];
   out[10] = rot[10];
   out[11] = 0;
   out[12] = pos[0];
   out[13] = pos[1];
   out[14] = pos[2];
   out[15] = 1;
   };



#endif
