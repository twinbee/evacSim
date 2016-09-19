/******************************************************************************
** entity.cpp / entity.h
** Primary author: Mike J, Matthew B
** Description: class that defines our agents
** Used in sim: representing entities phyiscally, geometrically, pathing intelligence, steering behaviors
******************************************************************************/


#ifndef ENTITY_H
#define ENTITY_H

#include "vector.h"
#include "matrix.h"
#include "color.h"
#include "portal.h"
#include <vector>
#include <ode/ode.h>
#include "billboard.h"

#include <string>

#include "physical.h"

enum {SEEK_EDGE_WITHIN_BOUNDS = -1, SEEK_EDGE_OUTSIDE_BOUNDS, SEEK_EDGE_REACHED};
enum bodyType {SPHERE, CCYLINDER, CYLINDER, BOX, MAN};
enum entity_type {SPECTATOR = 0 ,BOMB_TECH = 1, SUICIDE_BOMBER = 2};

//this struct is used as a return value for the Seek_Edge steering behavior
struct Seek_Edge_Info
{
    int Seek_Status;
    float Seek_Distance;
};

class Entity : public Physical
{
public:
    int getClass();

    static const float LARGEST_ENTITY_RADIUS = 0.7;   //feet
          // 0.66 ft radius corresponds to a 50 inch waist 
          // 0.6 ft radius corresponds to a 45 inch waist
    static const float SMALLEST_ENTITY_RADIUS = 0.5; //feet
          // 0.4 ft radius corresponds to a 30 inch waist
          // 0.3 ft radius corresponds to a 22.6 inch waist (child/teen)

    static const float MIN_ENTITY_SPEED = 4.26509186 * .5; //feet / second
           //these rates (min) correspond to observed walking speeds from papers
    static const float MAX_ENTITY_SPEED = 2*4.26509186 * .5;  //feet / second

    Entity(CVector & pos, float r_radius, float r_maxspeed, bodyType bt, entity_type et);

    ~Entity();

    void Entity::Kill();

    void Draw(CCamera * Cam);
    void ODEMove(float FrameTime);  //finalize all steering influence with the physics system

    //Pathing management functions
    bool StartPath(int StartIndex, int EndIndex, std::vector< std::vector<int> > & P);
    bool IncrementPath();
    int GetNextRegionInPath();
    void SetPath(std::vector<int> & path);
    void InsertIntoPath(int region);


    //Data access functions
    float GetRadius();

    inline int GetID() {
        return ID;
    }

    inline float GetMaxSpeed(){
        return MaxSpeed;
    }

    inline float GetSpeed(){
        return Speed;
    }

    //STEERING BEHAVIORS
    bool Separate(std::vector<Entity *> & peers, float weight,float FrameTime);
    Seek_Edge_Info SeekEdge(CVector & P1, CVector & P2, CVector & Normal, float weight, float FrameTime);
    bool EdgeRepel(CVector & P1, CVector & Normal, float weight, float FrameTime);

    //utility functions
    void AnchorToPlane(CVector  P, CVector  PNormal, float offset = 0.005);
	  bool Entity::PortalWithinViewRadius(CPortal * portal);

    //Parameter setting functions
    bool SetMaxSpeed(float speed);
    
    void SetColor(CColor rcolor);

    int Destination;
    float frustration;
    float FloatWarning;

    //void SetLabel(char * text);
    bool ClearLabel();
    
    float GetHeight();

    void Injure(float inj);
    int InjuryLevel();
    bool IsDead();
    entity_type GetType();

private:

    //Entity Attributes
    float radius, mass, height;
    float Speed,MaxSpeed, mobility;
    float viewradius, viewclip;

    CColor color;

    //Pathing Data
    int Current_PathIndex;     //current index within Path std::vector
    std::vector<int> Path;          //stores the entire path

    int ID;                    //an identification number for this entity

    //Physics parameters
    int injury; //0 is no injury, 100 is dead


    bodyType bodType;
    CVector SteeringForce;
    CBillboard * label;

    entity_type Type;
    

};

#endif


