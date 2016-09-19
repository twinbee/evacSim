#ifndef BARRIER_H
#define BARRIER_H

#include "vector.h"
class CBarrier
{
public:
  CBarrier(CVector & pP1, CVector & pP2);
  ~CBarrier();
  void Draw();
private :
  CVector P1;
  CVector P2;
  CVector P1up;
  CVector P2up;
};

#endif
