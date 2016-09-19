/******************************************************************************
** map.cpp / map.h
** Primary author: Mike J, Matthew B
** Description: class that defines simulation environment
** Used in sim: representing world phyiscally, geometrically, links between regions
******************************************************************************/

// DUTIES
//   pathing: by components, by APSP
//   distance to bottleneck, compute components
//   picking of regions
//   displaying itself and keeping track of color schemes, label schemes

#include <iostream>
#include <cstdlib>

#include <GL/gl.h>
#include <GL/glu.h>

#include "Map.h"
#include "Parse.h"
#include "APSP.h"
#include "convert.h"
#include "camera.h"

#include "draw.h"

#include "frustrum.h"
#include "adjlist.h"

#include "entityfactory.h"

#include <fstream>
#include <cstdlib>

#include "main.h"

#include "physworld.h"



CMap::CMap(const char * Filename, float SpawnParameter)
{

    const bool debug = true;

    const float SCALE = 1;
    //sketchup export to OBJ has a real-world unit which is used for exporting
    // if SCALE is set to 1, and the map is exported in Feet, then all linear units
    // in the entire sim will be in feet

    if (debug) cout << "CMap ctor\n";

    Adjacency = 0;
    //init pointer to our adjacency matrix for the region-graph
    EditAdjacency = 0;
    //similarly for pathing matrix between components of the region-graph

    if (strstr(Filename,".obj"))
    {
        if (!ComputeBBFromOBJ(Filename,BB))   return;

    }


    initPhysics(BB); //set up everything we need for ODE to work properly

    if (!LoadMap(Filename)) return;

    if (strstr(Filename,".obj"))
    {
        //---------CREATE THE PATHS------------------
        if (Adjacency) delete [] Adjacency;
        if (EditAdjacency) delete [] EditAdjacency;

        //create adjacency array with only raw dists and incline weights considered
        Adjacency = ComputeAdjacencyArray(Region_list);

        //create the raw pathing representations
        int size = Region_list.size();
        EditAdjacency = new float[size * size];

        memcpy(EditAdjacency,Adjacency,sizeof(float) * size*size);
        //copy from Adjacency to EditAdjacency

        vPath_D.resize(size,vector<float>(size));
        //explain

        vPath_P.resize(size,vector<int>(size));
        //explain

        FloydAPSP (size, Adjacency, vPath_D, vPath_P);
        if (debug) cout << "CMap Ctor: FloydAPSP success\n";
    }

    //find all goal regions
    for (unsigned p=0; p< Region_list.size(); p++)
    {
        if (Region_list[p]->GetPathingDifficulty() == PATHING_EXIT)     SafeRegions.push_back(p);
    }

    SelectBottlenecks(Region_list,Bottleneck_list);

    if (debug) cout << "CMap Ctor: SelectWaypoints success\n";

    //FloodComponents();
    FloodComponents(Region_list,Components);

    if (debug) cout <<"CMap Ctor: Flood Components success\n";

    CreateLocalPaths();

    if (debug) cout << "CMap Ctor: CreateLocalPaths success\n";

    entityFactory = new EntityFactory(Region_list,SafeRegions);
    entityFactory -> SpawnEntitiesSmart(SpawnParameter);
    //entityFactory->SpawnBleachers((int)SpawnParameter);

    if (debug) cout << "CMap Ctor: SpawnEntities success\n";

    NumEntitiesEvacuated = 0;
    NumEntitiesSpawned = entityFactory->GetNumSpawned();
    NumFloaters = 0;

    PCH = 0;//new CPortalClearingHouse(Adjacency,Region_list.size());

    if (debug) cout << "CMap Ctor: PCH ctor success\n";

    ODEFrames = 0; //used for timing

    CreateVicinityList();
    if (debug) cout << "CMap Ctor: CreateVicinityList success\n";

    Tree = Create_KD_Tree(Region_list);
    if (debug) cout << "CMap Ctor: CreateKDTree success\n";
    //(the KDTree is used for culling occluded regions when drawing)

    O_Map = 0;
    
    DetectOverlappingRegions();
  
}

void CMap::DetectOverlappingRegions()
{
     for(int r=0; r < Region_list.size(); r++)
     {
             CRegion * ptRegion = Region_list[r];
             
             for(int p=0; p<ptRegion->Portals.size(); p++)
             {
                     CPortal * ptPortal = ptRegion->Portals[p];
                     CVector PortalPos = ptPortal->GetPoint1();
                     CVector PortalNormal = ptPortal->GetNormal();
                     
                     int AdjID = ptPortal->GetDest();
                     
                     CRegion * ptAdjRegion = Region_list[AdjID];
                     
                     vector<CVector> Verts = ptAdjRegion->Polygon->GetVertices();
                     
                     
                     for(int v=0; v<Verts.size(); v++)
                     {
                             float PE = ((PortalPos - Verts[v]) % PortalNormal);
                           
                             if(PE > 0.1)
                             {
                                 cout<<"PLANE EQUATION : "<<r<<" "<<AdjID<<" "<<PE<<endl;
                                 ptRegion->Polygon->SetColor(PURPLE);
                             }     
                                
                     }  
                     
                         
                                  
             }//for each adjacent region           
     } //for each region         
}

CMap::~CMap()
{
    const bool debug = false;
    if (debug) cout<<"CMap dtor"<<endl;

    ClearMap();

    if (debug) cout<<"Map cleared"<<endl;

    if (Adjacency) delete [] Adjacency;
    if (EditAdjacency) delete [] EditAdjacency;

    if (debug) cout<<"Adjacency deleted"<<endl;

    uninitPhysics();

    if (PCH) delete PCH;
    if (Tree) DeleteTree(Tree);
    
    for(int e=0; e<explosions.size(); e++)
       delete explosions[e];

}

///////////////////////////////////////////////////////////////////
void CMap::SpawnInRegion(int region, float Spawnparameter)
{
    if (Spawnparameter >= 1) entityFactory->SpawnEntitiesByCount(region,(int)Spawnparameter,SPECTATOR);
      
    NumEntitiesSpawned = entityFactory->GetNumSpawned() - NumEntitiesEvacuated;
}

void CMap::SpawnInBleachers(float Spawnparameter)
{
    for(int r=0; r<Region_list.size(); r++)
    {
         if(Region_list[r]->GetPathingDifficulty() == PATHING_SEATING)
            if (Spawnparameter >= 1) entityFactory->SpawnEntitiesByCount(r,(int)Spawnparameter,SPECTATOR);
    }
      
    NumEntitiesSpawned = entityFactory->GetNumSpawned() - NumEntitiesEvacuated;
}

void CMap::DeployBombTech(int src, int num, int dst)
{
	entityFactory->SpawnEntitiesByCount(src,num,BOMB_TECH);
   NumEntitiesSpawned = entityFactory->GetNumSpawned() - NumEntitiesEvacuated; 

   for(int e=0; e<Region_list[src]->Entity_list.size(); e++)
   {
		Entity * ptEntity = Region_list[src]->Entity_list[e];
      if(ptEntity->GetType() == BOMB_TECH)
      {
      	ptEntity->Destination = dst;
			vector<int> Path;
			GetPathInclusive(src,ptEntity->Destination,vPath_P,Path);
   		ptEntity->SetPath(Path);
      }
   }

	//set the path to the optimal path
}

void CMap::DeployBomber(int src, int num, int dst)
{
	entityFactory->SpawnEntitiesByCount(src,num,SUICIDE_BOMBER);
   NumEntitiesSpawned = entityFactory->GetNumSpawned() - NumEntitiesEvacuated; 

   for(int e=0; e<Region_list[src]->Entity_list.size(); e++)
   {
		Entity * ptEntity = Region_list[src]->Entity_list[e];
      if(ptEntity->GetType() == SUICIDE_BOMBER)
      {
      	ptEntity->Destination = dst;
			vector<int> Path;
			GetPathInclusive(src,ptEntity->Destination,vPath_P,Path);
   		ptEntity->SetPath(Path);
      }
   }

	//set the path to the optimal path
}

void CMap::DeploySpectator(int src, int num, int dst)
{
	entityFactory->SpawnEntitiesByCount(src,num,SPECTATOR);
   NumEntitiesSpawned = entityFactory->GetNumSpawned() - NumEntitiesEvacuated; 

   for(int e=0; e<Region_list[src]->Entity_list.size(); e++)
   {
		Entity * ptEntity = Region_list[src]->Entity_list[e];
      if(ptEntity->GetType() == SPECTATOR)
      {
      	ptEntity->Destination = dst;
			vector<int> Path;
			GetPathInclusive(src,ptEntity->Destination,vPath_P,Path);
   		ptEntity->SetPath(Path);
      }
   }
	//set the path to the optimal path
}


///////////////////////////////////////////////////////////////////

bool CMap::LoadMap(const char * filename)
{
    const bool debug = false;

    ClearMap();

    if (debug) cout<<"CMap::LoadMap()"<<endl;

    if (strstr(filename,".obj"))
    {
        if (!LoadRegionListOBJ(Region_list,filename))
        {
            ClearMap();
            return false;
        }
    }
    else
    {
        cout<<"MAP FILE MUST BE OF TYPE .NAV OR .OBJ"<<endl;
        return false;
    }

    //no errors
    return true;
}


void CMap::Draw(CCamera * Cam, CFrustrum & Frustum, float Frametime)
{

    //draw a scale model 100 sq ft per quadrant
    // "crosshatch" in model units (feet) at the origin
    float SCALE = 100; //lines in the crosshatch are 100 ft long for reference
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(SCALE, 0.0, 0.0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, SCALE, 0.0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, SCALE);
    glEnd();

    glBegin(GL_LINES);
    glEnable(GL_LINE_STIPPLE);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(-SCALE, 0.0, 0.0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, -SCALE, 0.0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, -SCALE);
    glDisable(GL_LINE_STIPPLE);
    glEnd();
    //done drawing reference crosshatch

    glDisable(GL_LIGHTING);

    CBitset bitset;
    bitset.Resize(Region_list.size());
    int RegionsDrawn = 0;

    RenderTree(Cam,Tree,false,Frustum, true, RegionsDrawn, bitset, FrameTime);
    //RenderTree draws using the KD Tree (cut out unseen regions and entities)

	 //for (int r = 0; r < Region_list.size(); r++)		Region_list[r]->Draw(Cam);

    glEnable(GL_LIGHTING);
    
    //draw the explosions
    for(int e=0; e<explosions.size(); e++)
        explosions[e]->Draw(FrameTime);

}


void CMap::ClearMap()
{
    const bool debug = false;

    if (debug) cout << "ClearMap\n";
    for (int r=0; r<Region_list.size(); r++) delete Region_list[r];
    if (debug) cout << "all regions deallocated\n";
    Region_list.clear();
    vPath_P.clear();
    vPath_D.clear();
    SafeRegions.clear();
    NumEntitiesSpawned = 0;
    NumEntitiesEvacuated = 0;
}

int CMap::GetNumEvacuated()
{
    return NumEntitiesEvacuated;
}

int CMap::GetNumRemaining()
{
    return NumEntitiesSpawned - NumEntitiesEvacuated;
}

int CMap::GetNumFloaters(){
    return NumFloaters;
};


//////////////// START HIGHLIGHT FUNCTIONS /////////////////////////////////


void CMap::DeHighlightRegion(int index)
{
    if (index < Region_list.size() && index >= 0)
        Region_list[index]->DeHighlight();
}

void CMap::HighlightPathInclusive(int start, int end)
{
    if ( (start < 0) || (start >= Region_list.size())) return;
    if ( (end < 0) || (end >= Region_list.size())) return;

    Region_list[start]->Highlight();
    HighlightPath(start,end);
    Region_list[end]->Highlight();
}


void CMap::HighlightPath(int start,int end)
{

    if ( (start < 0) || (start >= Region_list.size())) return;
    if ( (end < 0) || (end >= Region_list.size())) return;

    int k = vPath_P[start][end];
    if (k == -1)  return;

    HighlightPath(start,k);
    Region_list[k]->Highlight();
    HighlightPath(k,end);
}

void CMap::HighlightExitPath(int index)
{
    for ( int p= 0 ; p < Region_list[index]->LocalPath.size(); p++)
        Region_list[Region_list[index]->LocalPath[p]]->Highlight();

}

void CMap::HighlightsOff()
{
    for (unsigned p=0; p<Region_list.size(); p++)
        Region_list[p]->DeHighlight();

}

void CMap::HighlightRegion(int index)
{
    Region_list[index]->Highlight() ;
}

//////////////// END HIGHLIGHT FUNCTIONS /////////////////////////////////

void CMap::Update(float FrameTime)
{
    cout<<"DEBUG ComputerBehaviorVectors() begin."<<endl;
    ComputeBehaviorVectors(FrameTime); //leaves us with non-normalized steering vector
    cout<<"DEBUG ComputerBehaviorVectors() end."<<endl;
    cout<<"DEBUG ODEMoveEntities() begin."<<endl;
    ODEMoveEntities(FrameTime);
    cout<<"DEBUG ODEMoveEntities() end."<<endl;
    
    //PCH->Purge(ODEFrames++);

    //
//    for (int r1=0; r1<Region_list.size(); r1++)
//    {
//        for (int r2=0; r2<Region_list.size(); r2++)
//        {
//            if (PCH->PortalClosed(r1,r2) || PCH->PortalClosed(r2,r1))
//            {
//                CPortal * Portal = Region_list[r1]->GetPortalToRegion(r2);
//                if (Portal)
//                    Portal->SetColor(RED);
//
//                Portal = Region_list[r2]->GetPortalToRegion(r1);
//                if (Portal)
//                    Portal->SetColor(RED);
//            }
//            else
//            {
//                CPortal * Portal = Region_list[r1]->GetPortalToRegion(r2);
//                if (Portal)
//                    Portal->SetColor(GREEN);
//                Portal = Region_list[r2]->GetPortalToRegion(r1);
//                if (Portal)
//                    Portal->SetColor(GREEN);
//            }
//        }//end for r2
//    }//end for r1


    //reroute based on closure reports
 //   Report rep(0,0,0);

/*
    while (PCH->GetClosureReport(rep))
    {

        for (int v=0; v<Vicinity_List[rep.src].size(); v++)
        {

            RerouteRegion(Vicinity_List[rep.src][v]);

        }
    }*/

}//end update

void CMap::ComputeBehaviorVectors(float FrameTime)
{
    const bool debug =  false;
    if (debug) cout << "ComputeBehaviorVectors()\n";

    for (unsigned p=0; p< Region_list.size(); p++)
    {
        //if this is an exit region, just delete all the entities in it and add them to
        //the evacuated count because they are already out of the building.
        if (debug) cout<<endl<<endl<<"Region : "<<p<<" NumEntities :"<<Region_list[p]->Entity_list.size()<<endl;

        if (Region_list[p]->IsExit())
        {
            NumEntitiesEvacuated += Region_list[p]->Entity_list.size();
            Region_list[p]->DeleteAllEntities();
            continue;
        }

        //if this isn't an exit region, go through every entity and do some AI and collision
        for ( int e=0; e<Region_list[p]->Entity_list.size(); e++)
        {
            //declare a pointer to the entity to make code look cleaner
            Entity * E = Region_list[p]->Entity_list[e];

            if (debug) cout<<"Entity #"<<E->GetID()<<" "<<E->GetPos()<<endl;

            //Found out the next region that this entity wants to go to
            int NextRegion = E->GetNextRegionInPath();
            if (debug) cout<<"Current Destination = "<<NextRegion<<endl;
            //done with our current path.

            CPortal * Portal = Region_list[p]->GetPortalToRegion(NextRegion);
            if (!Portal)
            {
                if (debug) cout<<"NO PORTAL TO NEXT REGION"<<endl;
                continue;
            }

            CVector MidPortal = Portal->GetMidPoint();
            CVector P1 = Portal->GetPoint1();
            CVector P2 = Portal->GetPoint2();
            CVector PNormal = Portal->GetNormal();

            Seek_Edge_Info Seek_Info = E->SeekEdge(P1,P2,PNormal,15.0 * 2,FrameTime);
            int Seek_Status = Seek_Info.Seek_Status;

//-------------------------------------------------------------------
            //repel away from walls
            if (Seek_Status == SEEK_EDGE_OUTSIDE_BOUNDS)
                for (unsigned c=0; c<Region_list[p]->Walls.size(); c++)
                {
                    //Wall * ptWall = Region_list[p]->Walls[c];
                    //Entity->EdgeRepel(ptWall->GetPoint1(), ptWall->GetNormal(),4 );
                }

            //try to separate from all adjacent regions
            //find out how close this entity is to the target portal and get
            // a weight that will make him more eager to push towards the portal
            // if he is close to it.

            float weight = 5.1;
//
//                float Portallength = Portal->GetWidth();
//                 if(Portallength <= (6 * Entity->GetRadius()))
//                 {
//                        weight = 1;
//                 }
//                 else if(Portallength < (8 * Entity->GetRadius()))
//                 {
//                    if(Seek_Info.Seek_Distance <= 3.0)
//                    {
//                         weight *= (Seek_Info.Seek_Distance * 8.4) ;
//                    }
//                 }




//            //separate from Entity_list in the same region
//
//            Entity->Separate(Region_list[p]->Entity_list,weight,FrameTime);
//
//        //separate from Entity_list in any region that this entity's view radius overlaps with.
//                 for(unsigned adj = 0; adj < Region_list[p]->Portals.size(); adj++)
//                 {
//                    if( Entity->PortalWithinViewRadius(Region_list[p]->Portals[adj]))
//                    {
//                        int adjregion = Region_list[p]->Portals[adj]->GetDest();
//                        Entity->Separate(Region_list[adjregion]->Entity_list,weight,FrameTime);
//                    }
//                 }


        }//end for each entity
    }//end for each region


}//end CMap ComputeBehaviors()

int CMap::GetRegionContainingPoint(CVector & Point, int currentregion, int depth) //depth == 2 is best
{
    CVector Up(0,0,1);
    CRegion * ptCurrentRegion = Region_list[currentregion];

    if (ptCurrentRegion->Polygon->ContainsCircle(0,Point,Up)) return currentregion;

    //for each adjacent region
    if (depth > 0)
    {
        depth--;
        for (unsigned p=0; p< ptCurrentRegion->Portals.size(); p++ )
        {
            int adjindex = ptCurrentRegion->Portals[p]->GetDest();

            int result = GetRegionContainingPoint(Point, adjindex, depth);
            if (result != -1) return result;

        }//end for each adjacent
    }//end if depth

    return -1;
}

void CMap::UpdateEntityPathing(float FrameTime)
{
    for (unsigned p=0; p< Region_list.size(); p++)
    {
        for (unsigned e=0; e<Region_list[p]->Entity_list.size(); e++)
        {
            Entity * Entity = Region_list[p]->Entity_list[e];

            //find out if the entity has been forced into any other portal
            CVector Position = Entity->GetPos();
            int NewRegion = GetRegionContainingPoint(Position, p, 2 );

            if (NewRegion == -1) //entity is outside the bounds of the pathing map
            {
                Entity->FloatWarning += FrameTime;

                if (Entity->FloatWarning > 8.0) //if entity has been out of bounds for 8 seconds or more
                {
                    NumFloaters++;

                    Region_list[p]->DeRegisterEntity(Entity);
                    delete Entity;
                    //Entity->Kill();
                    //NewRegion = p;
                }
                continue;
            }

            Entity->FloatWarning = 0;

            if (NewRegion != p)
            {
                //if entity pushed into proper next region
                if (NewRegion == Entity->GetNextRegionInPath())
                {
                    Entity->IncrementPath();
                    Region_list[NewRegion]->RegisterEntity(Entity);
                    Region_list[p]->DeRegisterEntity(Entity);
                    char buf[20];
                    itoa(NewRegion,buf,10);

                  /*  //update frustration
                    CPortal * Portal = Region_list[NewRegion]->GetPortalToRegion(Entity->GetNextRegionInPath());
                    if (Portal) Entity->frustration = ((Portal->GetMidPoint() - Entity->GetPos()).Length() / Entity->GetMaxSpeed()) * 3.0;
                    if (Entity->frustration < CPortalClearingHouse::minfrustration)
                        Entity->frustration = CPortalClearingHouse::minfrustration;*/


                }//end if next region
                else //if entity pushed off path
                {
                    if(Entity->GetType() == SPECTATOR)
						  { 
                        if(Region_list[Entity->Destination]->GetPathingDifficulty() == PATHING_EXIT)
                    			Entity->SetPath(Region_list[NewRegion]->LocalPath);
                        else //sent to a specific destination
                        {
									vector<int> Path;
									GetPathInclusive(NewRegion,Entity->Destination,vPath_P,Path);
                        	Entity->SetPath(Path);
								}
                       
                    		// Entity->InsertIntoPath(p);
						  }
						  else if((Entity->GetType() == BOMB_TECH) || (Entity->GetType() == SUICIDE_BOMBER))
						  {
								vector<int> Path;
								GetPathInclusive(NewRegion,Entity->Destination,vPath_P,Path);
                        Entity->SetPath(Path);
						  }

                    Region_list[NewRegion]->RegisterEntity(Entity);
                    Region_list[p]->DeRegisterEntity(Entity);
                    char buf[20];
                    itoa(NewRegion,buf,10);

                    //update frustration

                } //end else if entity pushed off path

            }  //end if NewRegion != p  (changed regions for whatever reason)

					  Entity->AnchorToPlane(Region_list[NewRegion]->GetCentroid(), Region_list[NewRegion]->Polygon->GetNormal(),-.1);

        }//end for e
    }//end for p

}//end UpdateEntityPaths()

void CMap::ExplodeRegion(int region, float power)
{
   CVector center = Region_list[region]->Polygon->GetCentroid();
   Explosion * e = new Explosion (center, power);
   explosions.push_back(e);
}

void CMap::ODEMoveEntities(float FrameTime)
{
    bool debug = false;
    if (debug) cout << "!! CMap::ODEMoveEntities\n";

    //move each entity
    for (unsigned p=0; p< Region_list.size(); p++)
    {
        for (unsigned e=0; e<Region_list[p]->Entity_list.size(); e++)
        {
            Entity * Entity = Region_list[p]->Entity_list[e];

//            //make sure the entity's position hasn't become unstable or invalid
//            CVector pos = Entity->GetPos();
//
//            if ((pos.x != pos.x) || (pos.y != pos.y) || (pos.z != pos.z))
//            {
//                cerr << "ENTITY in region " << p << " QNAN ERROR.\n";
//                Region_list[p]->DeRegisterEntity(e);
//                delete Entity;
//                continue;
//            }

            Entity->ODEMove(FrameTime);

//            //-----update fustration----
//            Entity->frustration -= FrameTime;
//            if (Entity->frustration <= 0)
//            {
//                PCH->ReportClosure(p,Entity->GetNextRegionInPath(),ODEFrames);
//                Entity->frustration = CPortalClearingHouse::ttl;
//            }

        }//end for each entity
    }//end for each region

    //STEP THE WORLD
    cout<<"DEBUG dSpaceCollide() begin."<<endl;
    if (!g_PHYS_MODE) dSpaceCollide(S, contacts, nearCallback);

	 cout<<"DEBUG updatePhysics() begin."<<endl;
    updatePhysics(FrameTime);
	 cout<<"DEBUG updatePhysics() end."<<endl;

	 cout<<"DEBUG updateEntityPathing() begin."<<endl;
    UpdateEntityPathing(FrameTime);
	 cout<<"DEBUG updateEntityPathing() end."<<endl;

} //end ODEMoveEntity_list

CVector CMap::GetCenter()
{
    return CVector ((BB.minx + BB.maxx)/2.0, (BB.miny + BB.maxy)/2.0, (BB.minz + BB.maxx)/2.0);
}

int CMap::NumRegions()
{
    return Region_list.size();
}

//void CMap::SetEntitySpeeds(float min, float max)
//{
//    for (unsigned r=0; r < Region_list.size(); r++)
//    {
//        for (unsigned e=0; e<Region_list[r]->Entity_list.size(); e++)
//            Region_list[r]->Entity_list[e]->SetMaxSpeed(randomN(min,max));
//    }
//}


bool CMap::RerouteRegion(int region)
{
    bool debug = false;

    if ((region < 0) || (region >= Region_list.size())) return false;

    if (debug) cout<<"Start pathing"<<endl;
    vector<int> path = PCH->FindNearest(region, SafeRegions);
    if (debug) cout<<"end pathing"<<endl;

    if (!path.size()) return false;

    if (debug)
    {
        cout<<"Path : ";
        for (int x=0; x<path.size(); x++)
            cout<<path[x]<<" ";
        cout<<endl;
    }

    //give all the entities in this region the new path
    CRegion * Region = Region_list[region];

    for (int e=0; e< Region->Entity_list.size(); e++)
    {
        Region->Entity_list[e]->SetPath(path);
        Region->Entity_list[e]->SetColor(RED);
    }

    return true;
}


void CMap::BFS(CBitset & Visited, vector<int> & list, int current,int src)
{
    //cout<<"visiting :"<<current<<endl;
    CRegion * Region = Region_list[current];


    if (vPath_D[src][current] < 40.0)
    {
        if (current != src)
            list.push_back(current);
    }

    Visited.Set(current);

    for (int p=0; p<Region->Portals.size(); p++)
    {
        CPortal * Portal = Region->Portals[p];

        if (!Visited.On(Portal->GetDest()))
        {
            BFS(Visited,list,Portal->GetDest(),src);

        }
    }

    return;
}

void CMap::CreateVicinityList()
{
    const bool debug = false;
    if (debug) cout<<"CREATE VICINITY LIST"<<endl;

    CBitset Visited;
    Visited.Resize(Region_list.size());

    Vicinity_List.clear();
    Vicinity_List.resize(Region_list.size());

    for (int r=0; r<Region_list.size(); r++)
    {
        if (debug) cout<<"Computing Vicinity for "<<r<<endl;
        Visited.ClearAll();
        BFS(Visited,Vicinity_List[r],r,r);
    }

    if (debug)
    {
        cout<<"VICINITY LIST : "<<endl;
        for (int v=0; v<Vicinity_List.size(); v++)
        {
            cout<<"["<<v<<"]";
            for (int a=0; a<Vicinity_List[v].size();a++)
            {
                cout<<Vicinity_List[v][a]<<" ";
            }
            cout<<endl;
        }
        cout<<"End Vicinity List"<<endl;
    }
}

void CMap::HighlightVicinity(int region)
{
    int size = Vicinity_List[region].size();
    for (int x=0; x <= size; x++)
    {
        HighlightRegion(Vicinity_List[region][x]);
    }

}


int CMap::NearestExit(int index)
{
    if (!SafeRegions.size()) return -1;

    int firstindex = -1;
    for (int s=0; s<SafeRegions.size(); s++)
        if (SafeRegions[s] > 0.0)
        {
            firstindex = s;
            break;
        }

    if (firstindex == -1) return -1;

    int smallestindex = firstindex;
    for (int s=firstindex; s< SafeRegions.size(); s++)
    {
        if (vPath_D[SafeRegions[s]][index] >  0.0)
            if ( vPath_D[SafeRegions[s]][index] < vPath_D[SafeRegions[smallestindex]][index])
                smallestindex = s;
    }

    if ((vPath_D[SafeRegions[smallestindex]][index] < 0.0) || (vPath_D[SafeRegions[smallestindex]][index] > 100000000.0))
        return -1;
    return SafeRegions[smallestindex];

}

/*
void CMap::RegisterBottleneck(int src, int dst)
{
    bool found = false;
    for (int w=0; w<Bottleneck_List.size(); w++)
    {
        if ((Bottleneck_List[w].src == src) && (Bottleneck_List[w].dst == dst))
        {
            found = true;
            break;
        }
    }
    if (!found)
    {
        t_Bottleneck bneck;
        bneck.src = src;
        bneck.dst = dst;
        Bottleneck_List.push_back(bneck);
    }
}
*/

void CMap::LabelRegionDistances()
{
    for (int r1=0; r1<Region_list.size(); r1++)
    {
        CRegion * Region = Region_list[r1];

        int nr = NearestExit(r1);

        if (nr > -1)
        {
            char buf[20];
            snprintf(buf,20,"%.2f",vPath_D[r1][nr]);
            Region->SetLabel(buf);
        }
        else
            Region->SetLabel("No exit");
    }
}


void CMap::LabelRegionIDs()
{
    for (int r1=0; r1<Region_list.size(); r1++)
    {
        CRegion * Region = Region_list[r1];

        char buf[20];
        snprintf(buf,20,"%d",Region->GetID());
        Region->SetLabel(buf);
    }
}

void CMap::LabelComponents()
{
    for (int r1=0; r1<Region_list.size(); r1++)
    {
        CRegion * Region = Region_list[r1];

        char buf[20];
        snprintf(buf,20,"%d",Region->ComponentID);
        Region->SetLabel(buf);
    }
}

void CMap::ClearLabels()
{
    for (int r1=0; r1<Region_list.size(); r1++)
        Region_list[r1]->ClearLabel();
}


void CMap::HighlightOptimal(int regionIndex)
{
    int ne = NearestExit(regionIndex);
    if (ne != -1)
        HighlightPath(regionIndex, ne);
}

/*
void CMap::SelectBottlenecks()
{
    static float bottlenecksize = 10;
    //static float bottlenecksize = Entity::LARGEST_ENTITY_RADIUS * 2*8;

    for (int r=0; r<Region_list.size(); r++)
    {
        CRegion * Region = Region_list[r];

        for (int p=0; p < Region->Portals.size(); p++)
        {
            int adj = Region->Portals[p]->GetDest();
            CRegion * AdjRegion = Region_list[adj];


            if ((AdjRegion->Portals.size() == 2) && (Region->Portals.size() >= 3))
            {

                if ( Region->GetPortalToRegion(adj)->GetWidth() < bottlenecksize)
                {
                    Region->HighlightPortal(adj);
                    AdjRegion->HighlightPortal(r);
                    RegisterBottleneck(adj,r);
                    RegisterBottleneck(r,adj);
                    AdjRegion->GetPortalToRegion(r)->bottleneck = true;
                    Region->GetPortalToRegion(adj)->bottleneck = true;
                }
            }
            else if ((AdjRegion->Portals.size() >= 3) && (Region->Portals.size() == 2))
            {
                if ( AdjRegion->GetPortalToRegion(r)->GetWidth() < bottlenecksize)
                {
                    AdjRegion->HighlightPortal(r);
                    Region->HighlightPortal(adj);
                    RegisterBottleneck(adj,r);
                    RegisterBottleneck(r,adj);
                    AdjRegion->GetPortalToRegion(r)->bottleneck = true;
                    Region->GetPortalToRegion(adj)->bottleneck = true;
                }
            }

        }
    }//end for each region
}

*/	

void CMap::ModifyIntelligent(int dim, float * matrix)
// stairs, bleachers, special region types get special emphasis
{
    for (int r=0; r<Region_list.size(); r++)
    {
        CRegion * Region = Region_list[r];

        if (NearestExit(r) == -1) continue;

        // funnel them down stairwells
        bool found = false;
        for (int p=0; p < Region->Portals.size(); p++)
        {
            int adj = Region->Portals[p]->GetDest();
            CRegion * AdjRegion = Region_list[adj];
            CVector P1 = Region->Polygon->GetCentroid();
            CVector P2 = AdjRegion->Polygon->GetCentroid();

            bool DownwardProgress = (P1.z - P2.z) > 2.0;
            bool ForwardProgress = (vPath_D[adj][NearestExit(adj)] < vPath_D[r][NearestExit(r)]);
            if ((AdjRegion->GetPathingDifficulty() == PATHING_STAIRS) && (ForwardProgress && DownwardProgress))
                found = true;
        }

        if (found)
        {
            for (int p=0; p < Region->Portals.size(); p++)
            {
                int adj = Region->Portals[p]->GetDest();
                CRegion * AdjRegion = Region_list[adj];
                CVector P1 = Region->Polygon->GetCentroid();
                CVector P2 = AdjRegion->Polygon->GetCentroid();

                bool DownwardProgress = (P1.z - P2.z) > 2.0;
                bool ForwardProgress = (vPath_D[adj][NearestExit(adj)] < vPath_D[r][NearestExit(r)]);

                if ((AdjRegion->GetPathingDifficulty() == PATHING_STAIRS) && (ForwardProgress && DownwardProgress) )
                {
                    //lol
                }
                else
                    matrix[r * dim + adj] = 0.0;
            }
        }

        // restrict movement regarding seating
        for (int p=0; p < Region->Portals.size(); p++)
        {
            int adj = Region->Portals[p]->GetDest();
            CRegion * AdjRegion = Region_list[adj];

            //if we aren't in seating, then don't ever move into seating
            if ((AdjRegion->GetPathingDifficulty() == PATHING_SEATING) && (Region->GetPathingDifficulty() != PATHING_SEATING))
                matrix[r * dim + adj] = 0.0;
        }

    }//end for each region

}//end Intelligent Modification of the adjacency matrix (for pathing)


bool CMap::CreateLocalPaths()
{
    const bool debug = false;

    for (int r=0; r<Region_list.size(); r++)
    {
        int ne = NearestExit(r);
        if (ne != -1)
        {
            Region_list[r]->LocalPath = ComponentPath(r, NearestExit(r));
            if (debug)
            {
                cout<<"Path for "<<r<<" : ";
                for (int p=0; p<Region_list[r]->LocalPath.size(); p++) cout<<Region_list[r]->LocalPath[p]<<" ";
                cout<<endl;
            }
        }
    }
    if (debug) cout << "END CreateLocalPaths()\n";
}
/*
void CMap::FloodComponents()
{
    const bool debug = false;

    int cID = 0;
    for (int x=0; x<Region_list.size(); x++)
    {
        vector<t_Bottleneck> Bottlenecks;
        CColor color = ALLCOLORS[cID % NUM_ALLCOLORS];
        if (FloodRecursive(x,Bottlenecks,cID))
        {
            CComponent component;
            component.Bottlenecks = Bottlenecks;
            component.ID = cID;
            component.color = color;
            Components.push_back(component);
            Bottlenecks.clear();
            cID++;
        }
    }

    //replace the region IDs with Component IDs
//   cout<<"Components.size() : "<<Components.size()<<endl;
    for (int c=0; c< Components.size(); c++)
    {
        Components[c].Adj_Component.resize(Components[c].Bottlenecks.size());
        for (int a=0; a<Components[c].Bottlenecks.size(); a++)
            Components[c].Adj_Component[a] = Region_list[Components[c].Bottlenecks[a].dst]->ComponentID;
    }


    //mark the components that contain exit regions
    for (int s=0; s<SafeRegions.size(); s++)
    {
        int index = Region_list[SafeRegions[s]]->ComponentID;
        if (index < Components.size())
            Components[Region_list[SafeRegions[s]]->ComponentID].hasexit = true;
    }


    //compute the number of regions for each component
    for (int r=0; r<Region_list.size(); r++)
        Components[Region_list[r]->ComponentID].numregions++;


    //print out the results
    for (int c=0; c< Components.size(); c++)
    {
        if (debug)
        {
            cout<<"ComponentID : "<<Components[c].ID<<" Numregions : "<<Components[c].numregions;
            if (Components[c].hasexit)cout<<" [Hasexit]";
            cout<<endl;

            cout<<"Adj : ";
            for (int a=0; a<Components[c].Adj_Component.size(); a++)
                cout<<Components[c].Adj_Component[a]<<" ";
            cout<<endl;
        }
    }

}

//returns true if a new component was flooded.
bool CMap::FloodRecursive(int src, vector<t_Bottleneck> & Adj, int ID)
{
    const bool debug = false;
    CRegion * Region = Region_list[src];

    if (Region->ComponentID != -1) return false;

    Region->ComponentID = ID;

    for (int p=0; p < Region->Portals.size(); p++)
    {

        int adj = Region->Portals[p]->GetDest();
        CRegion * AdjRegion = Region_list[adj];

        if (Region->Portals[p]->bottleneck)
        {
            t_Bottleneck bneck(src,adj);
            Adj.push_back(bneck);
            continue;
        }

        if (AdjRegion->ComponentID != -1) continue;

        FloodRecursive(adj,Adj,ID);

    }//end for

    return true;
}

*/

vector<int> CMap::ComponentPath(int src_region, int dst_region)
{
    vector<int> Path;
    vector<bool> visited;
    visited.resize(Components.size());
    for (int v=0; v<visited.size(); v++) visited[v] = false;

    ComponentPath2(src_region,dst_region,Path,visited);
    reverse(Path.begin(),Path.end());

    return Path;
}

float CMap::Dist2Bottleneck(int src, t_Bottleneck bneck)
{
    if (src == bneck.src) return 0.0;

    CRegion * BnRegion = Region_list[bneck.src];
    CVector gap = BnRegion->GetPortalToRegion(bneck.dst)->GetMidPoint() - BnRegion->Polygon->GetCentroid();

    return vPath_D[src][bneck.src] + gap.Length();
}

struct IFpair //explain
{
    int ID;
    float value;
    IFpair(int i, float v) : ID(i), value(v){}
};

inline bool operator < (const IFpair& t1, const IFpair& t2)
//we overloaded the < operator for sorting purposes
// we want to use this for keeping a "closest match" to a layer
{
    return t1.value < t2.value;
}


bool CMap::ComponentPath2(int current_region, int dst_region, vector<int> & PathC, vector<bool> visited)
// recursive part of componentPath
{
    bool debug = false;
    int current_comp = Region_list[current_region]->ComponentID;
    if (debug) cout<<"CURRENT COMPONENT : "<<current_comp<<endl;
    int dst_comp = Region_list[dst_region]->ComponentID;

    visited[current_comp] = true;

    if (current_comp == dst_comp)
    {
        if (debug) cout<<"Constructing path : ";
        //construct this portion of the path
        vector<int> p;
        GetPathInclusive(dst_region,current_region, vPath_P, p);
        for (int x=0; x<p.size(); x++)
        {
            if (PathC.size() == 0)
            {
                PathC.push_back(p[x]);
                if (debug) cout<<p[x]<<" ";
            }
            else if (PathC[PathC.size() - 1] != p[x])
            {
                PathC.push_back(p[x]);
                if (debug) cout<<p[x]<<" ";
            }
        }
        if (debug) cout<<endl;

        if (debug) cout<<"return true"<<endl;
        return true;
    }


    vector<IFpair> pairs;

    //SORT THE BOTTLENECKS ACCORDING TO DISTANCE FROM CURRENT_REGION
    if (debug) cout<<"Nonsorted Adj : ";
    for (int b=0; b < Components[current_comp].Bottlenecks.size(); b++)
    {
        t_Bottleneck bneck =  Components[current_comp].Bottlenecks[b];
        if (debug) cout<<Region_list[bneck.dst]->ComponentID<<" ";
        if (visited[Region_list[bneck.dst]->ComponentID]) continue;
        IFpair pair(b,Dist2Bottleneck(current_region, bneck));
        
        pairs.push_back(pair);
    }
    if (debug) cout<<endl;
    if (pairs.size()) sort(pairs.begin(),pairs.end());

    if (debug)
    {
        cout<<"Sorted Adj : ";
        for (int p=0; p<pairs.size(); p++)
            cout<<Region_list[Components[current_comp].Bottlenecks[pairs[p].ID].dst]->ComponentID<<" ";
        cout<<endl;
    }


    //call ComponentPath2 on the bottlenecks in order of nearest to farthest
    for (int b=0; b<pairs.size(); b++)
    {
        t_Bottleneck bneck = Components[current_comp].Bottlenecks[pairs[b].ID];
        
        if(vPath_D[current_region][bneck.dst] == 0.0) continue;
        
        if (ComponentPath2(bneck.dst,dst_region,PathC,visited))
        {
            if (debug) cout<<"Constructing path : ";
            //construct this portion of the path
            vector<int> p;
            GetPathInclusive(bneck.dst,current_region, vPath_P, p);
            for (int x=0; x<p.size(); x++)
            {
                if (PathC.size() == 0)
                {
                    PathC.push_back(p[x]);
                    if (debug) cout<<p[x]<<" ";
                }
                else if (PathC[PathC.size() - 1] != p[x])
                {
                    PathC.push_back(p[x]);
                    if (debug) cout<<p[x]<<" ";
                }
            }
            if (debug) cout<<endl;

            if (debug) cout<<"return true"<<endl;
            return true;
        }
    }

    if (debug) cout<<"return false"<<endl;
    return false;
}

//////////////////// color mode stuff ///////////////////////////////////////

void CMap::SetColorMode(int mode)
{
    if (mode == COLOR_COMPONENTS)
    {
        for (int r=0; r<Region_list.size(); r++)
            Region_list[r]->Polygon->SetColor(Components[Region_list[r]->ComponentID].color);
    }
    else if (mode == COLOR_ERRORS)
    {
        for (int r=0; r<Region_list.size(); r++)
            Region_list[r]->UpdateErrorColor();
    }
    else if (mode == COLOR_REGIONS)
    {
        for (int r=0; r<Region_list.size(); r++)
            Region_list[r]->UpdatePathingColor();
    }
}

void CMap::RegisterOMap(CBSPtree * Omap)
{
    if(Omap) O_Map = Omap;  
    
    CVector start;
    CVector end;
    
    for(int r=0; r<Region_list.size(); r+=25)
    {
      start = Region_list[r]->Polygon->GetCentroid();
      start.z += .5;
      end = Region_list[rand() % Region_list.size()]->Polygon->GetCentroid();
      end.z += .5;    
      O_Map->AddRay(start,end);      
    }
    
}

vector <CRegion*> * CMap::getRegionPList()
{
 return &Region_list;
}

void CMap::floorRegion(int a)
{
 Region_list[a]->makeFloor();
}





void CMap::PathAroundDead()
{
    //edit the adjacency array to make it so that entities can't enter
    //regions with dead people, but that regions with dead people can still
    //be exited
    
    int size = Region_list.size(); 
    
    for(int r1=0; r1<Region_list.size(); r1++)
    {
      for(int r2=r1; r2<Region_list.size(); r2++)
      {
          bool r1Dead = Region_list[r1]->HasDead();
          bool r2Dead = Region_list[r2]->HasDead();
          
          if((!r1Dead) && r2Dead) EditAdjacency[r1 * size + r2] = 0.0;
          if((!r2Dead) && r1Dead) EditAdjacency[r2 * size + r1] = 0.0;
        
      }      
    }
    
    //compute the new paths based on the Adjacency array
    FloydAPSP (Region_list.size(), EditAdjacency, vPath_D, vPath_P);
      
    vector<int> P;
    //assign the new paths to the entities
    for(int r=0; r<Region_list.size(); r++)
    {
      if(NearestExit(r) == -1) continue;
      
        GetPathInclusive(r,NearestExit(r),vPath_P,P);
        Region_list[r]->LocalPath = P;
      
        for(int e=0; e< Region_list[r]->Entity_list.size(); e++)
        {
           Region_list[r]->Entity_list[e]->SetPath(P);    
        }  
      P.clear();
    }
           
}

void CMap::PathAroundType(entity_type et)
{
    //edit the adjacency array to make it so that entities can't enter
    //regions with dead people, but that regions with dead people can still
    //be exited
    
    int size = Region_list.size(); 
    
    for(int r1=0; r1<Region_list.size(); r1++)
    {
      for(int r2=r1; r2<Region_list.size(); r2++)
      {
          bool r1Dead = Region_list[r1]->HasType(et);
          bool r2Dead = Region_list[r2]->HasType(et);
          
          if((!r1Dead) && r2Dead) EditAdjacency[r1 * size + r2] = 0.0;
          if((!r2Dead) && r1Dead) EditAdjacency[r2 * size + r1] = 0.0;
        
      }      
    }
    
    //compute the new paths based on the Adjacency array
    FloydAPSP (Region_list.size(), EditAdjacency, vPath_D, vPath_P);
      
    vector<int> P;
    //assign the new paths to the entities
    for(int r=0; r<Region_list.size(); r++)
    {
      if(NearestExit(r) == -1) continue;
      
        GetPathInclusive(r,NearestExit(r),vPath_P,P);
        Region_list[r]->LocalPath = P;
      
        for(int e=0; e< Region_list[r]->Entity_list.size(); e++)
        {
           Region_list[r]->Entity_list[e]->SetPath(P);    
        }  
      P.clear();
    }
           
}

void CMap::BarrierType(entity_type et)
{
  vector<bool> visited;
  visited.resize(Region_list.size());
  for(int v=0; v<Region_list.size(); v++) visited[v] = false;
  
  for(int r=0; r<Region_list.size(); r++)
  {
      if(Region_list[r]->HasType(et) && (!visited[r]))
          BarrierRecurseType(r, et, visited);  
  }
}

void CMap::BarrierRecurseType(int current_region, entity_type et, vector<bool> & visited)
{   
    bool debug = false;
    
    if(debug) cout<<"visiting region"<<current_region<<endl;
    
    visited[current_region] = true;
    
    for(int p=0; p<Region_list[current_region]->Portals.size(); p++)
    {
        int dest = Region_list[current_region]->Portals[p]->GetDest();
        
        if(Region_list[dest]->HasType(et) && (!visited[dest])) 
        {
             if(debug) cout<<"recursing to region "<<dest<<endl;
             BarrierRecurseType(dest,et,visited);
        }
        else if(!Region_list[dest]->HasType(et))
        {
           if(debug) cout<<"putting a barrier at portal "<<p<<endl;
           Region_list[current_region]->BarrierPortal(p);
        }
      
    }     
  
}


void CMap::BarrierDead()
{
  vector<bool> visited;
  visited.resize(Region_list.size());
  for(int v=0; v<Region_list.size(); v++) visited[v] = false;
  
  for(int r=0; r<Region_list.size(); r++)
  {
      if(Region_list[r]->HasDead() && (!visited[r]))
          BarrierRecurse2(r, visited);  
  }
}
    
void CMap::BarrierRecurse2(int current_region, vector<bool> & visited)
{   
    bool debug = false;
    
    if(debug) cout<<"visiting region"<<current_region<<endl;
    
    visited[current_region] = true;
    
    for(int p=0; p<Region_list[current_region]->Portals.size(); p++)
    {
        int dest = Region_list[current_region]->Portals[p]->GetDest();
        
        if(Region_list[dest]->HasDead() && (!visited[dest])) 
        {
             if(debug) cout<<"recursing to region "<<dest<<endl;
             BarrierRecurse2(dest,visited);
        }
        else if(!Region_list[dest]->HasDead())
        {
           if(debug) cout<<"putting a barrier at portal "<<p<<endl;
           Region_list[current_region]->BarrierPortal(p);
        }
      
    }     
  
}

int CMap::NumDead()
{
  int numdead = 0;
  for(int r=0; r<Region_list.size(); r++)
  {
      for(int e=0; e< Region_list[r]->Entity_list.size(); e++)
          if(Region_list[r]->Entity_list[e]->InjuryLevel() >= 100)
             numdead++;  
  }
  
  return numdead; 
}

int CMap::NumInjured()
{
  int numinjured = 0;
  for(int r=0; r<Region_list.size(); r++)
  {
      for(int e=0; e< Region_list[r]->Entity_list.size(); e++)
      {
          int injury = Region_list[r]->Entity_list[e]->InjuryLevel(); 
          if((injury > 0) && (injury < 100))
             numinjured++;  
      }
  }  
  return numinjured;
}
    
    
