/******************************************************************************
** region.cpp, region.h
** Primary author: Mike J
** Description: a region is a walkable-surface area of the pathing map
** Used in sim: pathing, pathing map
******************************************************************************/

#include "region.h"
#include <iostream>
#include "draw.h"


using namespace std;

CRegion::CRegion(vector<CVector> Verts, vector<PortalInfo> PortalsInfo, int PATHING_ATTR)
{
    bool debug = false;
    if (debug) cout<<"CRegion ctor"<<endl;

    floor = 0;
    Polygon = new CPolygon(Verts);

    //floor = new Floor(Verts, Polygon->GetCentroid());

    PATHING_ATTRIBUTE = PATHING_ATTR;

    bHighlighted = false;

    //create the portals
    for (int p=0; p<PortalsInfo.size(); p++)
    {
        CPortal * ptPortal = new CPortal(PortalsInfo[p].P1,PortalsInfo[p].P2,PortalsInfo[p].DestinationIndex,PortalsInfo[p].Up,PortalsInfo[p].color);
        Portals.push_back(ptPortal);
    }

    //------------------
    if (debug) cout<<"Created the "<<Portals.size()<<" portals"<<endl;

    CreateWalls();
    MergeDuplicateWalls();

    MergeDuplicatePortals();
    //cout<<"PATHING ATT "<<PATHING_ATTRIBUTE<<endl;
    UpdatePathingColor();

    CVector Up(0,0,1);
    incline = Polygon->GetNormal() % Up;

    label = 0;
    
    ComponentID = -1;
}

void CRegion::makeFloor()
{
 floor = new Floor(Polygon->GetVertices(), Polygon->GetCentroid());
}

void CRegion::destroyFloor()
{
 delete floor;
}


void CRegion::MergeDuplicatePortals()
{
    //ELiMINATE PARALLEL EDGES
    
    cout<<"Eliminate portals"<<endl;
  
    int v = 0;
    while(v < Portals.size())
    {
          if(Portals.size() <=1 )
          {
            cout<<"JUST ONE Portal"<<endl;
            cout<<"End eliminate portals"<<endl<<endl;
            return;
          }
          
         cout<<"Num portals : "<<Portals.size()<<" v = "<<v<<endl; 
            
         CVector Edge1 = Portals[v]->GetPoint2() - Portals[v]->GetPoint1();
         CVector Edge2 = Portals[(v+1) % Portals.size()]->GetPoint2() - Portals[(v+1) % Portals.size()]->GetPoint1();
         Edge1.Normalize();
         Edge2.Normalize();
         bool Adjacent1 = Portals[v]->GetPoint2() == Portals[(v+1) % Portals.size()]->GetPoint1();
         bool Adjacent2 = Portals[v]->GetPoint1() == Portals[(v+1) % Portals.size()]->GetPoint2();
         bool redundant = Portals[v]->GetDest() == Portals[(v+1) % Portals.size()]->GetDest();
         if( (Adjacent1 || Adjacent2) && (Edge1 % Edge2 == 1.0) && redundant)
         {
              cout<<"Parrallel found at "<<v<<" "<<v+1<<endl;
              CPortal * NewPortal = new CPortal(Portals[v]->GetPoint1(), Portals[(v+1) % Portals.size()]->GetPoint2(), Portals[v]->GetDest(),Polygon->GetNormal(),GREEN);
              cout<<"created portal"<<endl;
              delete Portals[v];
              cout<<"deleted v"<<endl;
              delete Portals[(v+1) % Portals.size()]; 
              cout<<"deleted v+1"<<endl;
              Portals.erase(Portals.begin() + v);
              cout<<"erased first one"<<endl;

              //Portals.erase(Portals.begin() + (v % Portals.size())); 
              Portals[v % Portals.size()] = NewPortal;
              cout<<"erased second one"<<endl;  
                   
              //Portals.push_back(NewPortal);       
              cout<<"pushed new portal onto stack"<<endl;    
         } 
         else
              v++;
     }   
     cout<<"End eliminate portals"<<endl<<endl; 
}

void CRegion::MergeDuplicateWalls()
{
     const bool debug = true;
    //ELiMINATE PARALLEL EDGES
    
    if (debug) cout<<"MergeDuplicateWalls()"<<endl;
  
    int v = 0;
    while(v < Walls.size())
    {
          if(Walls.size() <=1 )
          {
            if (debug) cout<<"JUST ONE WALL"<<endl;
            if (debug) cout<<"End eliminate walls"<<endl<<endl;
            return;
          }
          
         if (debug) cout<<"Num walls : "<<Walls.size()<<" v = "<<v<<endl; 
            
         CVector Edge1 = Walls[v]->GetPoint2() - Walls[v]->GetPoint1();
         CVector Edge2 = Walls[(v+1) % Walls.size()]->GetPoint2() - Walls[(v+1) % Walls.size()]->GetPoint1();
         Edge1.Normalize();
         Edge2.Normalize();
         bool Adjacent = Walls[v]->GetPoint2() == Walls[(v+1) % Walls.size()]->GetPoint1();
         if(Adjacent && (Edge1 % Edge2 == 1.0))
         {
              if (debug) cout<<"Parrallel found at "<<v<<" "<<v+1<<endl;
              Material blue(Color(0,0,1));
              Wall * NewWall = new Wall(Walls[v]->GetPoint1(), Walls[(v+1) % Walls.size()]->GetPoint2(),Polygon->GetNormal(), blue);
              if (debug) cout<<"created wall"<<endl;
              delete Walls[v];
              if (debug) cout<<"deleted v"<<endl;
              delete Walls[(v+1) % Walls.size()]; 
              if (debug) cout<<"deleted v+1"<<endl;
              Walls.erase(Walls.begin() + v);
              if (debug) cout<<"erased first one"<<endl;

             // Walls.erase(Walls.begin() + (v % Walls.size())); 
              Walls[v % Walls.size()] = NewWall;
              if (debug) cout<<"erased second one"<<endl;  
                   
              //Walls.push_back(NewWall);       
              if (debug) cout<<"pushed new wall onto stack"<<endl;    
         } 
         else
              v++;
     }   
     if (debug) cout<<"End eliminate walls"<<endl<<endl;

    //highlight segmented walls

     v = 0;
    while(v < Walls.size())
    {             
                    CVector Edge1 = Walls[v]->GetPoint2() - Walls[v]->GetPoint1();
                    CVector Edge2 = Walls[(v+1) % Walls.size()]->GetPoint2() - Walls[(v+1) % Walls.size()]->GetPoint1();
                    bool Adjacent = Walls[v]->GetPoint2() == Walls[(v+1) % Walls.size()]->GetPoint1();
                    Edge1.Normalize();
                    Edge2.Normalize();
                    if( Adjacent && ((Edge1 % Edge2) == 1.0))
                    {
                        Material yellow(Color(1,1,0));
                        Walls[v]->SetMaterial(yellow);
                        Walls[(v+1) % Walls.size()]->SetMaterial(yellow);
                    }
          v++;
    }

  
}
    
void CRegion::UpdateErrorColor()
{
    if (0) ; //for interchaning precedence of following tests
    else if (bHighlighted) 
    {     Polygon->SetColor(YELLOW);
    }
    else if(!Polygon->IsConvex())
          Polygon->SetColor(BROWN);   
    else        
    {  
          Polygon->SetColor(DGREY);
    }
}

void CRegion::UpdatePathingColor()
{
    if(0)
    {
    }
    else if (bHighlighted) 
    {     Polygon->SetColor(YELLOW);
    }
    else if(PATHING_ATTRIBUTE == PATHING_EXIT) 
    {     Polygon->SetColor(BLUE);
    }
    else if(PATHING_ATTRIBUTE == PATHING_STAIRS)   
    {     Polygon->SetColor(ORANGE);
    }
    else if(PATHING_ATTRIBUTE == PATHING_SEATING) 
    {     Polygon->SetColor(BLACK);
    }
    else if(PATHING_ATTRIBUTE == PATHING_WAYPOINT)
    {
          Polygon->SetColor(RED);  
    }    
    else        
    {  
          Polygon->SetColor(DGREY);
    }
}

void CRegion::CreateWalls()
{
    bool debug = false;

    vector<CVector> Vertices = Polygon->GetVertices();
    int n = Vertices.size();
    for (int v=0; v<n; v++)
    {
        CVector P2 = Vertices[(v+1) % n];
        CVector P1 = Vertices[v];
        bool IsWall = true;

        for (int p=0; p<Portals.size(); p++)
        {
            CVector PortP1 = Portals[p]->GetPoint1();
            CVector PortP2 = Portals[p]->GetPoint2();

            if ((PortP1 == P1) && (PortP2 == P2))
            {
                IsWall = false;
            }
            else if ((PortP2 == P1) && (PortP1 == P2))
                IsWall = false;
        }

        if (IsWall)
        {
            if (debug) cout<<"Wall detected, NORMAL ["<<Polygon->GetNormal().x<<Polygon->GetNormal().y<<Polygon->GetNormal().z<<endl;
            
    		Material blue(Color(0,0,1));

            Wall * ptWall = new Wall(P1, P2, Polygon->GetNormal(), blue);

            Walls.push_back(ptWall);
        }

    }


}

void CRegion::DeleteAllEntities()
{
    for (int e=0; e<Entity_list.size(); e++)
        delete Entity_list[e];
    Entity_list.clear();
}

void CRegion::AddPortal(PortalInfo PInfo)
{
    CPortal * ptPortal = new CPortal(PInfo.P1,PInfo.P2,PInfo.DestinationIndex,PInfo.Up,PInfo.color);
    Portals.push_back(ptPortal);
}

CRegion::~CRegion()
{
    delete Polygon;
    for (int n=0; n<Portals.size(); n++) delete Portals[n];
    for (int n=0; n<Walls.size(); n++) delete Walls[n];
    for (int n=0; n<Barriers.size(); n++) delete Barriers[n];
    DeleteAllEntities();

    if(label) delete label;
}

void CRegion::Highlight()
{
    bHighlighted = true;
    Polygon->SetColor(YELLOW);
}

void CRegion::DeHighlight()
{
    bHighlighted = false;
    UpdatePathingColor();
}

void CRegion::Explode(float power)
{
 bool debug = false;
 if (debug) cout << "Region::explode\n";
 
 CVector center = Polygon->GetCentroid();

 //Explosion * e = new Explosion (center, power);
 
 //explosions.push_back(e);
}

void CRegion::Draw(float FrameTime,CCamera * Cam)
{
    //draw the region surface
    Polygon->Draw();

    if (floor) floor->Draw();

	for (int i = 0; i < explosions.size(); i++)
	{ explosions[i]->Draw(FrameTime); }

    //draw the walls
    if(DrawFlags & DRAW_WALL)
          for (int w=0; w<Walls.size(); w++) Walls[w]->Draw();
     
    //draw the portals
    if(DrawFlags & DRAW_PORTAL)
        for (int p=0; p<Portals.size(); p++)
            Portals[p]->Draw(Cam);

    //draw the entities
    if(DrawFlags & DRAW_ENTITIES)
    for (int e=0; e<Entity_list.size(); e++)
    {
        Entity_list[e]->Draw(Cam);
    }

    if(Cam && label) label->Draw(Cam);
    
    for (int n=0; n<Barriers.size(); n++)  Barriers[n]->Draw();
}

bool CRegion::IsExit() const
{
    return PATHING_ATTRIBUTE == PATHING_EXIT;
}

bool CRegion::RegisterEntity(Entity  * entity)
{
    if (!entity) return false;
    Entity_list.push_back(entity);
    return true;
}//end AddEntity

bool CRegion::DeRegisterEntity(int EntityIndex)
{
    if ((EntityIndex < 0) || (EntityIndex >= Entity_list.size())) return false;
    Entity_list.erase(Entity_list.begin() + EntityIndex);
    return true;
}//end RemoveEntity

bool CRegion::DeRegisterEntity(Entity * ptEntity)
{
    for (int e=0; e<Entity_list.size(); e++)
    {
        if (ptEntity == Entity_list[e])
        {
            Entity_list.erase(Entity_list.begin() + e);
            return true;
        }
    }
    return false;
}

float CRegion::GetArea() const
{
    return Polygon->GetArea();
}

CVector CRegion::GetCentroid()
{
    return Polygon->GetCentroid();
}

CPortal * CRegion::GetPortalToRegion(int RegionID)
{
    for (int p=0; p<Portals.size(); p++)
    {
        if (Portals[p]->GetDest() == RegionID)
            return Portals[p];
    }
    return NULL;
}

bool CRegion::HighlightPortal(int dest)
{
    for(int p=0; p<Portals.size(); p++)
        if(Portals[p]->GetDest() == dest)
        {
           Portals[p]->SetColor(PINK); 
           return true;
        }
        
    return false;
}

bool CRegion::ClearLabel()
{
    if(!label) return false;
    delete label;
    label = 0;
    return true;
}
void CRegion::SetLabel(char * text)
{
    if(label) delete label;

    SDL_Color sdlcolor;
    sdlcolor.r = 255;
    sdlcolor.g = 255;
    sdlcolor.b = 255;

    CVector centroid = Polygon->GetCentroid();
    CVector labelpos(centroid.x,centroid.y,centroid.z + .5);
    float width = 0.5 * strlen(text);
    label = new CBillboard(text,sdlcolor,width,1.0, labelpos);
}

bool CRegion::HasType(entity_type et)
{
  for(int e=0; e<Entity_list.size(); e++)
    if(Entity_list[e]->GetType() == et) return true;
    
  return false;
}

bool CRegion::HasDead()
{
  for(int e=0; e<Entity_list.size(); e++)
    if(Entity_list[e]->IsDead()) return true;
    
  return false;
  
}

void CRegion::BarrierPortal(int index)
{
    if((index >= 0) && (index < Portals.size()))
    {
        CVector P1 = Portals[index]->GetPoint1();
        CVector P2 = Portals[index]->GetPoint2();
        CBarrier * B = new CBarrier(P1,P2);
        Barriers.push_back(B);       
    } 
   
}
