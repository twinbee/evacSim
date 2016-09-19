#include "barrier.h"
#include <GL/gl.h>
#include <GL/glu.h>

CBarrier::CBarrier(CVector & pP1, CVector & pP2)
{
  CVector Up(0,0,1);
  Up *= 3;
  P1 = pP1;
  P2 = pP2;  
  P1up = P1 + Up;
  P2up = P2 + Up;
  
  
}

CBarrier::~CBarrier()
{
  
  
}

void CBarrier::Draw()
{
  
  glColor3f(1.0,0,0);
  glBegin(GL_LINES);
    glVertex3f(P1.x,P1.y,P1.z);
    glVertex3f(P1up.x,P1up.y,P1up.z);
    
    glVertex3f(P2.x,P2.y,P2.z);
    glVertex3f(P2up.x,P2up.y,P2up.z);
    
    glVertex3f(P1up.x,P1up.y,P1up.z);
    glVertex3f(P2up.x,P2up.y,P2up.z);
    
    glVertex3f(P1up.x,P1up.y,P1up.z - 1.0);
    glVertex3f(P2up.x,P2up.y,P2up.z - 1.0);
  
  glEnd();
  glColor3f(1,1,1);
  
}
