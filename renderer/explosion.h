#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "physworld.h"

#include <vector>
#include "vector.h"

class Sound;
class Shrapnel;

class Explosion
{
 public: 
 Explosion(CVector location, int shelss);

 void Draw(float FrameTime);
 void Purge(float FrameTime);
 void Explode(float);

 private:
   int shells;
   float radius;
   float effectdi;
   CVector origin;
 
	std::vector <Shrapnel *> fragments;

   Sound * sound;

   float time;
};

#endif
