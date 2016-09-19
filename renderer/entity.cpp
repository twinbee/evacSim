/******************************************************************************
** entity.cpp / entity.h
** Primary author: Mike J, Matthew B
** Description: class that defines our agents
** Used in sim: representing entities phyiscally, geometrically, pathing intelligence, steering behaviors
******************************************************************************/

#include <ODE/ode.h>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include "entity.h"
#include "APSP.h"

#include "physworld.h"

#include "displaylists.h"

using namespace std;

extern GLuint cylDL, sphereDL, ccylDL;

Entity::Entity(CVector & pos,
                 float r_radius, float r_maxspeed,
                 bodyType r_bt, entity_type et)
{
    bodType = r_bt;
    
    Type = et;

    injury = 1;

    moveable = true;

    static int currentID = 0;

    //create the ODE body
    o_bod = dBodyCreate (W);
    dBodySetPosition(o_bod,pos.x,pos.y,pos.z);

    //create the ODE geom
    radius = r_radius;
    height = RHR * radius; 

    if (bodType == CCYLINDER) o_geom = dCreateCCylinder(S,radius,height);
    //else if (bodType == CYLINDER) o_geom = dCreateCylinder(S,radius,height);
    else o_geom = dCreateSphere(S,radius);

    geom2Physical[o_geom] = this;
    
    dGeomSetCategoryBits (o_geom,CAT_ENTITY );
    dGeomSetCollideBits (o_geom,CBIT_ENTITY );

    dGeomSetBody(o_geom,o_bod);

    //set the desired and default entity attributes
    MaxSpeed = r_maxspeed;
    Speed = r_maxspeed;                      //never changes

    viewradius = 6.0;
    viewclip = 180.0;

    //give the entity a skin tone for asthetics
    color = SKINTONES[rand()%NUM_SKINTONES];
    //color = GREEN;

    //ID
    ID = currentID++;

    label = 0;
    FloatWarning = 0;

	 //set the billboard
    CVector offset(0,0,height + 3.0);
    if(Type == BOMB_TECH)
	 {
			label = new CBillboard("textures\\police.tga",5.0,5.0,pos + offset);
            SetColor(NAVY);
    }
	 if(Type == SUICIDE_BOMBER)
     {
			label = new CBillboard("textures\\bomber.tga",5.0,5.0,pos + offset);
			SetColor(GREEN);
     }
	 
}

Entity::~Entity()
{
}

int Entity::getClass()
{  return 1; //ENTITY 
}

void Entity::Injure(float inj)
{

 injury += int(inj);
 if (injury > 0) MaxSpeed /= injury;
 return;
}

void Entity::Kill()
{
	/* THIS IS DESTROYING THE BODY AND ~Physical destroys it as well. 
	One of them needs to be removed. */
	
 if (!moveable) return;
 
 //dBodyDisable(o_bod);
 geom2Physical[o_geom] = NULL;

 moveable = false;


 dBodyDestroy(o_bod); 
}

//draw either a sphere, a cappedcylinder, or an open-ended cylinder
//depending on the representation chosen for the physics system
void Entity::Draw(CCamera * Cam)
{
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glEnable (GL_COLOR_MATERIAL);

    glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;

    CVector pos = GetPos();

    glPushMatrix();
    glTranslatef(pos.x,pos.y,pos.z);

    glColor3f(color.r()+injury/100.0, color.g()-injury/100.0, color.b()-injury/100.0);

    glScalef(radius, radius, radius);

    if      (bodType == SPHERE) glCallList(sphereDL);
    else if (bodType == CYLINDER) glCallList(cylDL);
    else if (bodType == CCYLINDER) glCallList(ccylDL);

    glPopMatrix();

    glDisable(GL_LIGHTING);


    if (Cam && label)
    {
        label->SetPos(pos.x,pos.y,pos.z + height + 3.0);
        label->Draw(Cam);
    }
}

//SteeringForce is a vector that represents the entity's desired motion.
//This function sends SteeringForce into the physics system for resolution
void Entity::ODEMove(float FrameTime)
{
    const bool debug = false;
    if (injury > 100) Kill();

    if (debug) cout << "!!ODEMove Steering force : " << SteeringForce << endl;

    //Input the SteeringForce into ODE and let it handle the movement resolution
    if (moveable) dBodyAddForce (o_bod, SteeringForce.x, SteeringForce.y, SteeringForce.z );

    //reset the steering force for the next timestep
    SteeringForce = CVector(0);
}

//Given a start and end index within the walkable surfaces map, create
//a path between the two by utilizing the 'P' pathing data array.
bool Entity::StartPath(int StartIndex, int EndIndex, vector< vector<int> > & P)
{
    if (StartIndex < 0 || StartIndex >= P.size()) return false;
    if (EndIndex < 0 || EndIndex >= P.size()) return false;

    Path.clear();
    GetPathInclusive(StartIndex, EndIndex,P,Path);

    if (Path.size() <= 1) return false;
    Current_PathIndex = 0;

    return true;
}

//Get the next Region in this entity's current path
//Returns a -1 if there is no path or if the current path is finished
int Entity::GetNextRegionInPath()
{
    if ((Current_PathIndex+1) < Path.size())
        return Path[Current_PathIndex+1];
    else
        return -1;
}

//the radius cannot be changed throughout the life of this entity
//because it corresponds to data in the physics system
float Entity::GetRadius()
{
    return radius;
}


//This behavior accepts a list of other entities, called the peers list, and
//creates a steering force that attempts to move this entity away from
//any of those peers that might be invading his personal space.
bool Entity::Separate(vector<Entity *> & peers, float weight, float FrameTime)
{
    CVector force = 0.0;       //This vector will be used to accumulate the individual repellant
    //forces between this entity and each of its peers

    for (int e=0; e<peers.size(); e++)
    {
        if (peers[e] == this) continue; //if this entity is in his own peers list, ignore it

        CVector pos1 = GetPos();              //pos of this entity
        CVector pos2 = peers[e]->GetPos();    //pos of the current peer

        float distance = sqrt(pow(pos2.x-pos1.x,2) + pow(pos2.y-pos1.y,2));

        if (distance == 0)  continue; //if they are right on top of eachother, something
        //is wrong elsewhere in the sim, so ignore this peer

        if ((distance <= (viewradius + peers[e]->radius))) //if peer is within proximity
        {
            CVector Velocity = GetLinearVel();

            CVector Tempv = Velocity;
            CVector temptarget = pos2 - pos1;

            if (distance) temptarget.Normalize();
            if (Tempv.Length()) Tempv.Normalize();

            float dprod = temptarget.DotProduct(Tempv);

            float diffangle = acosf(dprod);

            //if the peer is not within the clipping portion of the proximity radius
            if ( (diffangle*(180/PI)) < viewclip )
            {
                force += temptarget;     //accumulate all individual separating forces
            }

        }//end if within proximity

    }//end for each peer

    if (!force.Length()) return false; //no peers were close enough to consider, nothing will be changed

    force.Normalize();   //normalize our accumulated force

    CVector DesiredVel = -force * Speed ;  //find the Desired velocity

    //create a steering force by taking the difference between the actual velocity
    //and the desired velocity.
    SteeringForce += (DesiredVel - GetLinearVel()) * weight * FrameTime;

    return true;
}

//the Maxspeed value may be changed but cannot be below 0 because that wouldn't
//make much sense.
bool Entity::SetMaxSpeed(float speed)
{
    if (speed < 0) return false;
    MaxSpeed = speed;
    return true;
}

//Given a plane specified by vector P1 and its normal, make this entity
//attempt to repel away from that plane if the entity closes to within
//a distance of that plane specified by the buffer variable.
bool Entity::EdgeRepel(CVector & P1, CVector & Normal, float weight, float FrameTime)
{
    CVector pos = GetPos();
    float distance = (P1-pos) % Normal;       //how far is the entity from the plane?
    float buffer = radius * 2;                //we ideally want to stay a body's length away
    //from this plane/wall

    if (distance > buffer) return false;      //if the entity is not within buffer's distance
    //of the wall/plane, then nothing needs to be done

    float coefficient = buffer - distance;

    if (distance < buffer)
    {
        //create a force that eminates away from the plane/wall
        CVector force = (Normal * -1 * coefficient) * weight * FrameTime;
        SteeringForce += force;
    }
    return true;  //a change was made to the steering force, so true is returned.
}

//The SeekEdge function is intended to steer an entity through a doorway in a realistic manner.
//The doorway is specified by two points, each representing a corner of the door.

//P1 = first corner of door, P2 = second corner of door, Normal = normal of the door's surface
//weight = strength of this steering behavior, FrameTime = length in seconds of the last frame.
//>Returns SEEK_EDGE_WITHIN_BOUNDS if entity is straight in front of door(can walk straight forward and make it through
//the door)
//>Returns SEEK_EDGE_OUTSIDE_BOUNDS if entity is still outside of the door's straightaway area
//>Returns SEEK_EDGE_REACHED if entity has already passed through the door

Seek_Edge_Info Entity::SeekEdge(CVector & P1, CVector & P2, CVector & Normal, float weight, float FrameTime)
{
    bool debug = false; //change this to true if debug output is desired for this function
    if (debug) cout << "!! SeekEdge on Entity ID : " << ID << endl;

    Seek_Edge_Info ReturnInfo;

    CVector pos = GetPos();  //query this entity's current position from the ODE physics system
    if (debug) cout << "Position: " << pos << endl;
    if (debug) cout<<"Normal : "<<Normal<<endl;

    //check to see if this entity has already crossed the threshhold of the portal/door :

    ReturnInfo.Seek_Distance = (P1 - pos) % Normal;  //how far is the entity from the door, and on which side?

    if ( ReturnInfo.Seek_Distance < 0 )              //if Seek_Distance is negative, entity has passed through door
    {
        if (ReturnInfo.Seek_Distance < 0) ReturnInfo.Seek_Distance *= -1; //remove the negation for the return data
        ReturnInfo.Seek_Status = SEEK_EDGE_REACHED;
        return ReturnInfo;                                                //entity is through the door, we are done
    }

    //create the offset points on the edge(if an entity is outside the straightway zone, he approaches one of these
    //two points depending on which side of the door he is on) :

    CVector P1Normal = (P2-P1).UnitVector();
    CVector P2Normal = (P1-P2).UnitVector();
    CVector P1Offset = P1 + (P1Normal * radius * 1.1);    //create the offset for the first side of the door
    CVector P2Offset = P2 + (P2Normal * radius * 1.1);    //create the offset for the second side of the door

    CVector MidPoint = Normal * radius * -1.1 + (P2 + P1) *0.5;
    if (debug) cout<<"MidPoint : "<<MidPoint<<endl;

    bool result1 = ((P1Offset - pos)%P1Normal) < 0.0;   //equation of plane for first side of door
    bool result2 = ((P2Offset - pos)%P2Normal) < 0.0;   //equation of plane for second side of door

    CVector Velocity = GetLinearVel();                  //query the current linear velocity from the physics system
    if (debug) cout << "Velocity: " << Velocity << endl;

    if (result1 && result2) //if entity is on positive side of both planes that run through the door corners,
    {                       //then the entity is in the door's "straightaway zone", a.k.a straight in front of the door

        //create a steeringforce that steers the entity straight towards the door along the door's normal
        SteeringForce += ( Normal*Speed - GetLinearVel() ) * FrameTime * weight;

        ReturnInfo.Seek_Status = SEEK_EDGE_WITHIN_BOUNDS;    //report that the entity is within the straightaway
    }
    else //entity is outside the straightaway zone, thus he is either to the left or to the right of the door
    {
        //create a steering force that steers the entity directly towards the nearest corner of the door
        SteeringForce += ((MidPoint - pos).UnitVector() * Speed - GetLinearVel()) * FrameTime * weight;

        ReturnInfo.Seek_Status = SEEK_EDGE_OUTSIDE_BOUNDS;
    }

    if (debug) cout<<"SteeringForce :"<<SteeringForce<<endl;

    return ReturnInfo;     //the proper steering force has been add to this entity, return the details.

}//end SeekEdge()
float Entity::GetHeight()
{ return height;}

//In order to avoid using physical gravity and ground collision to keep entities
//grounded, it's cheaper and easier to just raycast and force them to the ground plane
void Entity::AnchorToPlane(CVector  P, CVector PNormal, float offset)
{
    bool level = false;
    CVector pos = GetPos();

    float side = (P - pos) % PNormal;  //is the entity above or below the ground plane?

    if ( side < 0 )   //if on top of the ground plane
    {
        CVector pos2(pos.x,pos.y,pos.z-30);    //create a ray shooting down
        CVector v = pos2 - pos;
        float sect = -1 * ((pos - P) % PNormal) / (PNormal % v);
        CVector Mappedpos = pos + (v * sect) + (PNormal * offset);
        pos.z = Mappedpos.z;
    }
    else if (side == 0)//if directly on the plane
    {
        pos.z += offset;  //set the entity a little above the plane
    }
    else //if below the ground plane
    {
        CVector pos2(pos.x,pos.y,pos.z+30);    //shoot a ray up
        CVector v = pos2 - pos;
        float sect = -1 * ((pos - P) % PNormal) / (PNormal % v);
        CVector Mappedpos = pos + (v * sect) + (PNormal * offset);
        pos.z = Mappedpos.z;
    }

    dGeomSetPosition(o_geom, pos.x, pos.y, pos.z); 
   //register the new position with the physics system
}


//This function can tell us if the current entity is close to a particular portal
//or not.  This is mostly a utility function used for optimizing other algorithms.
bool Entity::PortalWithinViewRadius(CPortal * portal)
{
    CVector pos = GetPos();
    CVector closest = pos + (portal->GetNormal() * viewradius);
    if (  (closest - portal->GetPoint1()) % portal->GetNormal()  < 0) return true;
    return false;
}


//Set this entity's current path to a new path
void Entity::SetPath(vector<int> & path)
{
    Path.clear();
    Path = path;
    Current_PathIndex = 0;
}

//Add a detour into the entity's current path
void Entity::InsertIntoPath(int region)
{
    Path.insert(Path.begin() + Current_PathIndex + 1,region);
}

//When an entity crosses the portal between two pathing nodes, the entity's path data
//is incremented.
bool Entity::IncrementPath()
{
    if ((Current_PathIndex+1) < Path.size())
    {
        Current_PathIndex+=1;
        return true;
    }

    return false;
}

bool Entity::ClearLabel()
{
    if (!label) return false;
    delete label;
    label = 0;
    return true;
}

/*
void Entity::SetLabel(char * text)
{
    if (label) delete label;

    SDL_Color sdlcolor;
    sdlcolor.r = 255;
    sdlcolor.g = 255;
    sdlcolor.b = 255;

    CVector centroid = GetPos();
    CVector labelpos(centroid.x,centroid.y,centroid.z + .5);
    float width = 0.5 * strlen(text);
    label = new CBillboard(text,sdlcolor,width,1.0, labelpos);
}*/

void Entity::SetColor(CColor rcolor)
    {     color = rcolor;  }

int Entity::InjuryLevel()
{
  return injury;
}

bool Entity::IsDead()
{
  if(injury >= 100) return true;
  return false;
}

entity_type Entity::GetType()
{
	return Type;
}

