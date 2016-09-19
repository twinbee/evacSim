/*********************************************************
** graphics.h
** Primary author: Mike E
** Description: inclusion guards
** Used in sim: cross-compatibility
*********************************************************/


#ifndef mce_graphics
#define mce_graphics

//-------------------------------------------------------------

#ifdef __APPLE__
// Mac OS X
#include <OpenGL/glu.h>
#else
// Linux or Windows
#include <GL/glu.h>
#endif

#define ShowGLError { GLenum e = glGetError(); if ( e != GL_NO_ERROR ) fprintf( stderr, "%s on line %d of %s\n",  gluErrorString( e ), __LINE__, __FUNCTION__ ); }

//-------------------------------------------------------------

#endif
