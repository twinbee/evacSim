
#ifndef SHRAPNEL_H
#define SHRAPNEL_H

#include "physical.h"

class Shrapnel : public Physical
{
 public:
  Shrapnel (CVector position, float r = 0.25);
  ~Shrapnel();

  int getClass () {return 2;}

  void Draw();

 private:

    float radius;

};

#endif
