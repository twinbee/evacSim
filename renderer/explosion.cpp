#include <iostream>

#include "explosion.h"

#include "shrapnel.h"

#include "sound.h"
#include "convert.h"

#include "displaylists.h"

 using namespace std;

Explosion::Explosion(CVector pos, int shel)
{
 const bool debug = true;
 if (debug) std::cout << "Explosion ctor\n";
 
 origin = pos;

 sound = new Sound ( shells > 1 ? ("./sounds/explosion-01.wav") : ("./sounds/bomb-02.wav"));

 effectdi = 0.0;
 radius = 0.2; //per-fragment radius, feet
 shells = shel;

 float force = 100;

for (int i = 1; i <= shells; i++) //number of spherical "shells" an explosion has
{
 for (float theta = 0; theta < 2*PI; theta += radius+randomN(0,radius/10.0) )
 for (float phi = 0; phi < PI/2; phi += radius) //top half of a sphere
 {
  Shrapnel * s = new Shrapnel ( CVector ( pos.x+i*cos(theta)*sin(phi), pos.y+i*sin(theta)*sin(phi),	pos.z+i*cos(phi) ) );            

  //add initial forces  
  s->Drive(CVector ( force*cos(theta)*sin(phi), force*sin(theta)*sin(phi), force/10.0*cos(phi) ));

  fragments.push_back(s);

 }
}

}

void Explosion::Draw(float FrameTime)
{ 
   const bool debug = true;
   if (debug) cout << "explosion::draw\n";
   time += FrameTime;
   effectdi += 12.0 * FrameTime;

   Purge(FrameTime);
	for (int i = 0; i < fragments.size(); i++)
	{
   fragments[i]->Draw();
	}
 
   //draw the graphics

   if(effectdi < 20.0)
   { 
		float colordim = 1.0 - (effectdi / 30.0);
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
   	glBlendFunc(GL_ONE, GL_ONE);

   	glColor3f(1 * colordim, 0.7 * colordim, 0.7 * colordim);

       
   	glPushMatrix();
   	glTranslatef(origin.x,origin.y,origin.z);

   	glScalef(effectdi, effectdi, effectdi);

   	glCallList(sphereDL);
   	glPopMatrix();
    glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
   }
   

}

void Explosion::Purge(float FrameTime)
{
   const bool debug = true;
   if (debug) cout << "explosion::purge\n";
   int i=0;
	while( i < fragments.size())
	{
   	if ( /*(time > 5.0) || */((fragments[i]->GetPos() - origin).Length() > 500))
   	{
    		delete fragments[i];
    		fragments.erase(fragments.begin() + i);
   	}
      else
      	i++;
	}
}

