
#include "BSPnode.h"
#include "draw.h"

using namespace std;

//Given a set of polygons and a splitting plane, creates it's own children automatically.
CBSPnode::CBSPnode(vector<CPolygon> & Polygon_list, int DesiredSize, int Alignment, int depth)
{
  const bool debug = false; 
  
  NodeDepth = depth;
  Front = 0;
  Back = 0;
  BB.FromPolyList(Polygon_list);
 
  if(debug) BB.PrintInfo();
 
  
  //make the splitting plane
  CVector BBMid = BB.GetCenter();
  SplitPlane = new CPlane(BBMid,Alignment);   
  
  if(debug) SplitPlane->PrintInfo();
  
  if(Polygon_list.size() >  DesiredSize)
  {
    if(debug) cout<<"Splitting "<<Polygon_list.size()<<"Polygons"<<endl;
    //split the polygons into two lists on either side of the splitting plane
   vector<CPolygon> Front_list;
   vector<CPolygon> Back_list;
   DividePolygons(Polygon_list, Front_list, Back_list);
   
 
   //make the front child
   if(Front_list.size())
   {
      Front = new CBSPnode(Front_list, DesiredSize,((Alignment + 1) % 3), depth++);   
   }
   if(Back_list.size())
   {
      Back = new CBSPnode(Back_list, DesiredSize,((Alignment + 1) % 3), depth++);
   }
   
  }//end if > Desired size
  else
  {
    //copy the polygon list over in a way that doesn't duplicate the entire array even temporarily
    for(int p= (Polygon_list.size() - 1); p >= 0; p--)
    {
        Polygons.push_back(Polygon_list[p]);
        Polygon_list.pop_back();  
    }      
  }
}

CBSPnode::~CBSPnode()
{
   //traverse the tree and delete all nodes
   if(Front) delete Front;
   if(Back) delete Back; 
   
   if(SplitPlane) delete SplitPlane;
}

bool CBSPnode::IsLeaf()
{
   if(Polygons.size()) return true;
   
   return false;
}

void CBSPnode::DividePolygons(vector<CPolygon> & Polygon_list, vector<CPolygon> & Front_list, vector<CPolygon> & Back_list)
{
    bool debug = true;
    CPolygon Polygon;

    int OriginalCount = Polygon_list.size();
    int numsplits = 0;

    while ( Polygon_list.size())
    {
        Polygon = Polygon_list.back();
        Polygon_list.pop_back();

        int classification = SplitPlane->ClassifyPolygon(Polygon);
        switch (classification)
        {
        case P_COINCIDENT:
            Front_list.push_back(Polygon);
            break;
            
        case P_BACK:
            Back_list.push_back(Polygon);
            break;
            
        case P_FRONT:
            Front_list.push_back(Polygon);
            break;
            
        case P_SPANNING:
            CPolygon FrontHalf, BackHalf;
            SplitPlane->Split_Polygon(Polygon,FrontHalf,BackHalf);
            if(debug)
            {
              if((FrontHalf.GetArea() + BackHalf.GetArea()) != Polygon.GetArea())
              {
                cout<<"BAD SPLIT"<<endl;
                cout<<"  Polygon verts : "<<Polygon.NumVerts()<<" Area : "<<Polygon.GetArea()<<endl;
                cout<<"  Front verts : "<<FrontHalf.NumVerts()<<" Area : "<<FrontHalf.GetArea()<<endl;
                cout<<"  Back verts : "<<BackHalf.NumVerts()<<" Area : "<<BackHalf.GetArea()<<endl;
                cout<<endl<<endl;
              }
              
            }
            
            BackHalf.SetColor(Polygon.GetColor());
            FrontHalf.SetColor(Polygon.GetColor());
            
            if((BackHalf.NumVerts() >= 3) && (BackHalf.GetArea() > 0.0))
            {
              Back_list.push_back(BackHalf);
            }
            else if(debug) cout<<"BAD POLY"<<endl;
            
            if((FrontHalf.NumVerts() >= 3) && (FrontHalf.GetArea() > 0.0))
            {
              Front_list.push_back(FrontHalf);
            }
            else if(debug) cout<<"BAD POLY"<<endl;
            numsplits++;

            break;

        }//end switch
    }//end while
    
    Polygon_list.clear();
    
    if(Back_list.size() + Front_list.size() != (OriginalCount + numsplits)) cout<<"OMG LOSING POLYS"<<endl;
}

void CBSPnode::PrintInfo()
{   
    if(Front) Front->PrintInfo();
    if(Back) Back->PrintInfo();
    
    cout<<"Node Depth : "<<NodeDepth<<endl;
    if(Polygons.size()) 
       cout<<"Num Polygons in Leaf : "<<Polygons.size()<<endl;
    else
       cout<<"Not a Leaf, Num polys"<<endl;
    SplitPlane->PrintInfo();
    
    cout<<endl;
    
}

void CBSPnode::Draw(CFrustrum & Frustum, bool DrawBounds,bool testing)
{
    CVector Min = BB.GetMin();
    CVector Max = BB.GetMax();
    
    int result;
    if(testing)
       result = Frustum.BoxInFrustum(Min.x,Min.y,Min.z,Max.x,Max.y,Max.z);
    else
       result = COMPLETE_IN;
       
    if(result == COMPLETE_IN)
    {
       if(Back) Back->Draw(Frustum, DrawBounds,false);
       if(Front) Front->Draw(Frustum, DrawBounds,false);                   
    }    
    else if(result == INTERSECT)
    {
       if(Back) Back->Draw(Frustum, DrawBounds,true);
       if(Front) Front->Draw(Frustum, DrawBounds,true);  
    }

    if( result != COMPLETE_OUT)
    for(int p=0; p<Polygons.size(); p++)
        Polygons[p].Draw();    
        
    if(DrawBounds) BB.Draw();    
  
    

}

bool CBSPnode::RayNodeCollision(CVector & start, CVector & end)
{
    if(Polygons.size()) return RayLeafCollision(start,end); 
  
    float startside = SplitPlane->DistanceFrom(start);
    float endside = SplitPlane->DistanceFrom(end);
    
    if((startside > 0) || (endside > 0))
    {
      if(Front)
        if(Front->RayNodeCollision(start,end)) return true; 
    }
    
    if((startside < 0) || (endside < 0))
    {
      if(Back)
        if(Back->RayNodeCollision(start,end))   return true;
    }
      
    return false;
}


bool CBSPnode::RayLeafCollision(CVector & start, CVector & end)
{
    for(int p=0; p< Polygons.size(); p++)
        if(Polygons[p].IntersectsRay(start,end))
        {
           Polygons[p].SetColor(YELLOW);
           cout<<"RAY LEAF COLLISION TRUE"<<endl;
           return true;  
        }

    return false;  
}
