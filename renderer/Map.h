/******************************************************************************
** map.cpp / map.h
** Primary author: Mike J, Matthew B
** Description: class that defines simulation environment
** Used in sim: representing world phyiscally, geometrically, links between regions
**   This is where most of the simulation work takes place
******************************************************************************/

#ifndef MAP_H
#define MAP_H

#include <ode/ode.h>
#include <vector>

#include "vector.h"
#include "bitset.h"
#include "region.h"
#include "PCH.h"
#include "camera.h"

#include "BspCompile.h"

#include "draw.h"
#include "BSPtree.h"
#include "component.h"

class EntityFactory;

enum {COLOR_ERRORS = 0, COLOR_REGIONS, COLOR_COMPONENTS};




class CMap
{

public:

    CMap(const char * Filename, float SpawnParameter = 0.0);
    ~CMap();

    //main sequence functions
    bool LoadMap(const char * filename);
    void Draw(CCamera * Cam, CFrustrum & Frustum, float FrameTime);

    void Update(float FrameTime);

    int NumRegions();

    CVector GetCenter();

	void ExplodeRegion(int region, float power);

    void ClearMap();
    int GetNumEvacuated();   //returns the number of entities that are in safe regions
    int GetNumRemaining();   //returns the number of entities that are not yet in safe regions
    int GetNumFloaters();

    //highlighting functions(highlight different parts of the map)
    void HighlightPathInclusive(int start, int end);
    void HighlightRegion(int index);
    void DeHighlightRegion(int index);
    void HighlightPath(int start,int end);
    void HighlightsOff();

    //void SetEntitySpeeds(float min, float max);

    //visibility functions
    void TraceBeam(CVector  start, CVector  end, int previous, int current, int sourceRegion, int sourcePortal);
    bool TraceBeam2(int region1, CVector  start, int region2 ,CVector  end, int previous, int current);

    void ComputeRegionVisibility(int regionIndex);
    void ComputeLookaheadTable();
    void DrawVisRays(int regionIndex);

    bool ComputeVisset();

    //Used to quickly find out which region an entity is in
    int GetRegionContainingPoint(CVector & Point, int LastKnownRegion, int depth);
    bool ContainsPoint(CVector & Point, int region);

    //portal/region closing functions
    //        void ClosePortal(int regionIndex, int Portalindex);
    //       void OpenPortal(int regionIndex, int Portalindex);

    void CloseRegion(int regionID);

    void CloseRegionIndex(int regionIndex);

    void ClosePortal(int regionIndex, int portalIndex);

    void SpreadEntities();

    bool RerouteRegion(int region);

    void HighlightVicinity(int region);



    bool CreateLocalPaths();

    //diagnostic functions
    void LabelRegionDistances();
    void LabelRegionIDs();
    void LabelComponents();
    void ClearLabels();

    void HighlightOptimal(int regionIndex);
    void HighlightExitPath(int index);

    void SetColorMode(int mode);

    void SpawnInRegion(int region, float spawnParameter);
    void DeployBombTech(int src, int num, int dst);
    void DeploySpectator(int src, int num, int dst);
    void DeployBomber(int src, int num, int dst);
    
    void RegisterOMap(CBSPtree * Omap);

    //todo: delete these functions (refactor with less message passing)
    vector <CRegion*> * CMap::getRegionPList();
    void floorRegion(int a);
    
    void PathAroundDead();
    void PathAroundType(entity_type et);
    void BarrierDead();
    void BarrierRecurse2(int current_region, vector<bool> & visited);
    void BarrierType(entity_type et);
    void BarrierRecurseType(int current_region, entity_type et, vector<bool> & visited);

    int NumDead();
    int NumInjured();
    
    void SpawnInBleachers(float Spawnparameter);
  

private:
    void ComputeBehaviorVectors(float FrameTime);
    void ODEMoveEntities(float FrameTime);
    void UpdateEntityPathing(float FrameTime);

    vector<CRegion *> Region_list; //array of regions

    EntityFactory * entityFactory;

    //Pathing data
    vector< vector<float> > vPath_D;     //raw distances with only sloped weighting taken into account
    vector< vector<int> >   vPath_P;

    float * Adjacency;
    float * EditAdjacency;

    //data for the output
    vector<int> SafeRegions;

    int NumEntitiesSpawned;
    int NumEntitiesEvacuated;
    int NumFloaters;

    //visibility data
    //CBitset visdata;

    BoundingBox BB;

    CPortalClearingHouse * PCH;

    int ODEFrames;

    //vicinity list stuff
    vector< vector<int> > Vicinity_List;
    void BFS(CBitset & Visited, vector<int> & list, int current,int src);
    void CreateVicinityList();
    
    void DetectOverlappingRegions();

    //bottleneck shit
    vector<t_Bottleneck> Bottleneck_list;
    //void RegisterBottleneck(int src, int dst);
    float Dist2Bottleneck(int src, t_Bottleneck bneck);   
    //void SelectBottlenecks(); 

    void ModifyIntelligent(int dim, float * matrix);
    int NearestExit(int index);

    t_KDNode * Tree;

//    void FloodComponents();
//    bool FloodRecursive(int src, vector<t_Bottleneck> & Adj, int ID);

    vector<CComponent> Components;
    vector<int> ComponentPath(int src_region, int dst_region);
    bool ComponentPath2(int current_region, int dst_region, vector<int> & PathC, vector<bool> visited);


    CBSPtree * O_Map;
    
    vector<Explosion *> explosions;
};

#endif
