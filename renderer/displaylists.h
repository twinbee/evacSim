/**********************************************************
** displayliusts.h
** Primary Author: Matthew B
** Description: some simple geometric primitive display lists
** Used in sim: entities are made of these
*********************************************************/

#ifndef DISPLAYLISTS_H
#define DISPLAYLISTS_H

#include <GL/gl.h>
#include <GL/glu.h>

const float RHR = 10; //radius - height - ratio
extern GLuint sphereDL, ccylDL, cylDL;

GLuint createSphereDL();
GLuint createCylDL();
GLuint createCCylDL();
#endif
