#include "entityfactory.h"

#include "region.h"

#include "polygon2.h"

#include "convert.h"

#include "physworld.h"

using namespace std;

EntityFactory* EntityFactory::currentInstance = NULL;

EntityFactory::EntityFactory(vector <CRegion *> regions, vector <int> safe)
{
    RegionVec = regions;
    SafeRegions = safe;
    NumSpawned = ceiling = 0;
    EntityFactory::currentInstance = this;
}

/////////////////////START SPAWNING PART ///////////////////////////////////
// refactor extract class : entity factory
// needs : RegionVec : vector <Cregion *>
//
// polygon.h, region.h, entity.h, vector.h


void EntityFactory::SpawnEntitiesSmart(double toSpawn)
{
    int numRegions = RegionVec.size();
    if (toSpawn < 0) return;
    else if (toSpawn > 0 && toSpawn < 1) ;//by density
    else if (toSpawn >=  1000 ) SpawnEntitiesByTotal(int(toSpawn) );
    else for (unsigned p=0; p<numRegions; p++) SpawnEntitiesByCount(p,(int) toSpawn, SPECTATOR);

    return;
}


void EntityFactory::SpawnEntitiesByTotal(int total)
{
    int numRegions = RegionVec.size();

    ceiling = total;

    while (1)
    {
        if (GetNumSpawned() >= total) break;
        for (unsigned p=0; p<numRegions; p++)
        {
           SpawnEntitiesByCount(p,(int)total/numRegions, SPECTATOR);
        }
    }

    ceiling = 0;
}

void EntityFactory::SpawnBleachers(int count)
{
    int numRegions = RegionVec.size();

    for (unsigned p=0; p<numRegions; p++)
    {
           if(RegionVec[p]->GetPathingDifficulty() == PATHING_SEATING)
                SpawnEntitiesByCount(p,count, SPECTATOR);
    }
}

void EntityFactory::SpawnEntityAt(CVector pos, unsigned int RegionIndex, entity_type et)
{
    const bool debug = false;

    if (RegionIndex > RegionVec.size()) return;

    if (ceiling && GetNumSpawned() >= ceiling) return;

    unsigned p = RegionIndex;
    {
        // if(RegionVec[p]->GetPathingDifficulty() != PATHING_SEATING) continue;

        if (RegionVec[p]->GetArea() < 3) // square ft
        {
            if (debug) cout<<"Area too small to spawn in"<<endl;
            return;
        }

        if (RegionVec[p]->Portals.size() == 0)
        {
            if (debug) cout<<"No portals, no need spawning"<<endl;
            return;
        }

        if (RegionVec[p]->IsExit())
        {
            if (debug) cout<<"Region is Exit, no need to spawn here"<<endl;
            return;
        }
    }


    if (debug) cout<<"Spawning by position in Region : "<<RegionIndex<<endl;

    //reference the polygon and the up vector
    CPolygon * ptPolygon = RegionVec[RegionIndex]->Polygon;
    CVector Up(0,0,1);

        float radius = randomN(Entity::SMALLEST_ENTITY_RADIUS,Entity::LARGEST_ENTITY_RADIUS);
        float speed  = randomN(Entity::MIN_ENTITY_SPEED,Entity::MAX_ENTITY_SPEED);

        Entity * ptEntity = new Entity(pos, radius, speed, CCYLINDER, et);

        NumSpawned++;

        if (debug) cout<<"Setting destination"<<endl;

        if (SafeRegions.size() > 0)
        {
            int NearestExit = SafeRegions[0];
            for (int s=1; s<SafeRegions.size(); s++)
            {
                int exitindex = SafeRegions[s];
                //if(vPath_D[RegionIndex][exitindex] < vPath_D[RegionIndex][NearestExit]) NearestExit = exitindex;
            }
            ptEntity->Destination = NearestExit;
            //ptEntity->Destination = SafeRegions[rand() % SafeRegions.size()];
        }
        else
            ptEntity->Destination = RegionIndex;
        if (debug) cout<<"Destination set at : "<<ptEntity->Destination<<endl;

        ptEntity->SetPath(RegionVec[RegionIndex]->LocalPath);

        if (debug) cout<<"Good Path"<<endl;


        RegionVec[RegionIndex]->RegisterEntity(ptEntity);
        if (debug) cout<<"Registered Entity w region"<<endl;

        //spawning placement on the plane
        ptEntity->AnchorToPlane(ptPolygon->GetCentroid(),ptPolygon->GetNormal());

        if (debug) cout<<"SpawnAt AnchorToPlane success"<<endl;

        //set the initial fustration value
        CPortal * Portal = RegionVec[RegionIndex]->GetPortalToRegion(ptEntity->GetNextRegionInPath());

        if (debug) cout<<"SpawnAt GetPortalToRegion success"<<endl;


        // divide by zero here (possibly.. crashes on maze1.obj)
        /*
              ptEntity->frustration = ((Portal->GetMidPoint() - ptEntity->GetPos()).Length() / ptEntity->GetSpeed() * 3.0);
                        if(ptEntity->frustration < CPortalClearingHouse::minfrustration)
                                ptEntity->frustration = CPortalClearingHouse::minfrustration;
        */

}

void EntityFactory::SpawnEntitiesByCount(int RegionIndex, int count, entity_type et)
{
    const bool debug = false;

    if (ceiling && GetNumSpawned() >= ceiling) return;

    unsigned p = RegionIndex;
    {
        // if(RegionVec[p]->GetPathingDifficulty() != PATHING_SEATING) continue;

        if (RegionVec[p]->GetArea() < 3) // square ft
        {
            if (debug) cout<<"Area too small to spawn in"<<endl;
            return;
        }

        if (RegionVec[p]->Portals.size() == 0)
        {
            if (debug) cout<<"No portals, no need spawning"<<endl;
            return;
        }

        if (RegionVec[p]->IsExit())
        {
            if (debug) cout<<"Region is Exit, no need to spawn here"<<endl;
            return;
        }
    }


    if (debug) cout<<"Spawning by count in Region : "<<RegionIndex<<endl;

    BoundingBox BB = RegionVec[RegionIndex]->Polygon->GetBoundingBox();

    //create an offset along the edges so entities don't overlap walls
    BB.minx += Entity::LARGEST_ENTITY_RADIUS;
    BB.maxx -= Entity::LARGEST_ENTITY_RADIUS;
    BB.miny += Entity::LARGEST_ENTITY_RADIUS;
    BB.maxy -= Entity::LARGEST_ENTITY_RADIUS;

    //find the starting offset for the entity spawn grid
    float startx = BB.minx + (Entity::LARGEST_ENTITY_RADIUS);
    float starty = BB.miny + (Entity::LARGEST_ENTITY_RADIUS);

    //initialize the spawning grid
    int numcols = int(BB.lx / (Entity::LARGEST_ENTITY_RADIUS * 2.3));
    int numrows = int(BB.ly / (Entity::LARGEST_ENTITY_RADIUS * 2.3));
    int numcells = numcols * numrows;

    //reference the polygon and the up vector
    CPolygon * ptPolygon = RegionVec[RegionIndex]->Polygon;
    CVector Up(0,0,1);

    bool * grid = new bool[numcells];
    for (int g=0; g<numcells; g++)
    {
        int grow = g / numcols;
        int gcol = g % numcols;
        CVector gpos(
            startx + ((float)gcol * (Entity::LARGEST_ENTITY_RADIUS * 2.3)), //x
            starty + ((float)grow * Entity::LARGEST_ENTITY_RADIUS * 2.3),   //y
            0);                                                             //z

        if (!RegionVec[RegionIndex]->Polygon->ContainsCircle(Entity::LARGEST_ENTITY_RADIUS * 1.15,gpos,Up))
        {
            grid[g] = true;
        }
        else
            grid[g] = false;
    }

    while (count > 0)
    {
        //if the grid is completely full, quit
        bool foundfree = false;
        for (int g=0; g<numcells; g++)
            if (grid[g] == false)
                foundfree = true;

        if (!foundfree) break;

        //find the first random position
        int row = rand() % numrows;
        int col = rand() % numcols;


        //if the first random position fails, mark it and find a new one.
        if ((grid[row * numcols + col] == true))
        {
            for (int x=1; x<numcells; x++)
            {
                int index = ((row * numcols + col + x) % numcells);
                if ((grid[index] == false))
                {
                    grid[index] = true;

                    row = index / numcols;
                    col = index % numcols;
                    //just to break
                    x = numcells;
                    break;
                }
            }//end for
        }
        else
        {
            grid[row * numcols + col] = true;
        }


        //create the entity
        CVector pos(startx + ((float)col * (Entity::LARGEST_ENTITY_RADIUS * 2.3)), starty + ((float)row * Entity::LARGEST_ENTITY_RADIUS * 2.3), 0);
        float radius = randomN(Entity::SMALLEST_ENTITY_RADIUS,Entity::LARGEST_ENTITY_RADIUS);
        float speed  = randomN(Entity::MIN_ENTITY_SPEED,Entity::MAX_ENTITY_SPEED);

        Entity * ptEntity = new Entity(pos, radius, speed, CCYLINDER, et);

        NumSpawned++;
        //Entity_list.push_back(ptEntity);

		  //set the entities destination
        if (debug) cout<<"Setting destination"<<endl;

        if(et == SPECTATOR)
		  {
        		if (SafeRegions.size() > 0)
        		{
            	int NearestExit = SafeRegions[0];
            	for (int s=1; s<SafeRegions.size(); s++)
            	{
                	int exitindex = SafeRegions[s];
                	//if(vPath_D[RegionIndex][exitindex] < vPath_D[RegionIndex][NearestExit]) NearestExit = exitindex;
            	}
            	ptEntity->Destination = NearestExit;
            	//ptEntity->Destination = SafeRegions[rand() % SafeRegions.size()];
        		}
        		else
            	ptEntity->Destination = RegionIndex;

        		if (debug) cout<<"Destination set at : "<<ptEntity->Destination<<endl;

        		ptEntity->SetPath(RegionVec[RegionIndex]->LocalPath);

        }
		  else if((et == BOMB_TECH) || (et == SUICIDE_BOMBER))
		  {
			  //do nothing, have them sit there
		  }



        //register the entity
        RegionVec[RegionIndex]->RegisterEntity(ptEntity);
        if (debug) cout<<"Registered Entity w region"<<endl;

        //spawning placement on the plane
        ptEntity->AnchorToPlane(ptPolygon->GetCentroid(),ptPolygon->GetNormal());

        if (debug) cout<<"SpawnbyCount AnchorToPlane success"<<endl;

        //set the initial fustration value
        CPortal * Portal = RegionVec[RegionIndex]->GetPortalToRegion(ptEntity->GetNextRegionInPath());

        if (debug) cout<<"SpawnbyCount GetPortalToRegion success"<<endl;


        // divide by zero here (possibly.. crashes on maze1.obj)
        /*
              ptEntity->frustration = ((Portal->GetMidPoint() - ptEntity->GetPos()).Length() / ptEntity->GetSpeed() * 3.0);
                        if(ptEntity->frustration < CPortalClearingHouse::minfrustration)
                                ptEntity->frustration = CPortalClearingHouse::minfrustration;
        */
        count-- ;


    }//end while

    delete [] grid;
    if (debug) cout<<"SpawnbyCount delete [] grid success"<<endl;
}

/////////////////////END      SPAWNING PART ///////////////////////////////////
