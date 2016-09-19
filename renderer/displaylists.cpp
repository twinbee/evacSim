#include "displaylists.h"

GLuint sphereDL, ccylDL, cylDL;

GLuint createSphereDL()
{
  GLuint sphereDL = glGenLists(1);

	// start list
	glNewList(sphereDL,GL_COMPILE);

	// call the function that contains the rendering commands
  static GLUquadric * quadricc = gluNewQuadric();
  gluSphere  (quadricc, 1.0f, 8, 8);

	// endList
	glEndList();

 return sphereDL;
}

GLuint createCylDL()
{
  GLuint cylDL = glGenLists(1);

	// start list
	glNewList(cylDL,GL_COMPILE);

	// call the function that contains the rendering commands
  static GLUquadric * quadricc = gluNewQuadric();
  gluCylinder(quadricc, 1.0, 1.0, RHR, 8, 2);

	// endList
	glEndList();
	
	return cylDL;
}

GLuint createCCylDL()
{
  GLuint ccylDL = glGenLists(1);

	// start list
	glNewList(ccylDL,GL_COMPILE);

	// call the function that contains the rendering commands
  GLUquadric * quadricc = gluNewQuadric();
  gluCylinder(quadricc, 1.0, 1.0, RHR-2, 8, 2);
  glTranslatef(0.0, 0.0, RHR-1);
  gluSphere  (quadricc, 1.0f, 8, 8);
  
  glTranslatef(0.0, 0.0, -RHR+2);
  gluSphere  (quadricc, 1.0f, 8, 8);

	// endList
	glEndList();
	
	return ccylDL;
}
