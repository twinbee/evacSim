/******************************************************************************
** region.cpp, region.h
** Primary author: Mike J
** Description: a region is a walkable-surface area of the pathing map
** Used in sim: pathing, pathing map
******************************************************************************/

#ifndef _REGION_H
#define _REGION_H

#include "polygon2.h"
#include "wall.h"
#include "portal.h"
#include "entity.h"
#include "camera.h"
#include "billboard.h"

#include "floor.h"

#include "explosion.h"

#include <SDL_ttf.h>

#include <vector>

#include "barrier.h"

enum {PATHING_EXIT,PATHING_SEATING,PATHING_STAIRS,PATHING_WAYPOINT,PATHING_DEFAULT};

class CRegion
{
public:
    CRegion(std::vector<CVector> Verts, std::vector<PortalInfo> PortalsInfo, int PATHING_ATTR = PATHING_DEFAULT);
    ~CRegion();

    void makeFloor();
    void destroyFloor();

    void Highlight();
    void DeHighlight();
    void SetExitStatus(bool status);
    void Draw(float FrameTime, CCamera * Cam = 0);
    void AddPortal(PortalInfo PInfo);
    float GetArea() const;
    CVector GetCentroid();
    bool IsExit() const;

    CPortal * GetPortalToRegion(int RegionID);

	 void Explode(float);

    inline float GetArea() {
        return Polygon->GetArea();
    }

    void UpdateErrorColor();
    void UpdatePathingColor();
    void SetLabel(char * text);
    bool ClearLabel();

    //public data
    std::vector<CPortal *>  Portals;
    std::vector<Entity *> Entity_list;
    std::vector<Wall *> Walls;
    CPolygon * Polygon;
    std::vector<int> Waypoint_list;
    bool HighlightPortal(int dest);

    inline int GetPathingDifficulty(){
        return PATHING_ATTRIBUTE;
    }
    inline void SetPathingDifficulty(int attr) {
        PATHING_ATTRIBUTE = attr;
    }

    //entity handler functions
    bool RegisterEntity(Entity * entity);
    bool DeRegisterEntity(int EntityIndex);
    bool DeRegisterEntity(Entity * ptEntity);
    void DeleteAllEntities();

    inline float GetIncline(){
        return incline;
    }
    inline int GetID(){
        return ID;
    }
    void SetID(int value){
        ID = value;
    }

    std::vector<int> LocalPath;
    int ComponentID;
    
    bool HasDead();
    bool HasType(entity_type et);
    void BarrierPortal(int index);

private:

    void MergeDuplicatePortals();
    void MergeDuplicateWalls();

    float incline;
    int PATHING_ATTRIBUTE;
    bool bHighlighted;
    void CreateWalls();
    int ID;
    CBillboard * label;
    Floor * floor;

	  std::vector<Explosion *> explosions;
	  
	  std::vector<CBarrier *> Barriers; 
};


#endif
