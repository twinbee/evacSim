/**********************************************************
** BSPCompile.h, BSPCompile.cpp
** Primary Author: Mike J
** Description: Creates a BSP to speed up rendering and other processes
** Used in sim: might be out of use ??
*********************************************************/

#include "BSPCompile.h"



#include "draw.h"

#include <iostream>

using namespace std;

int ClassifyPoly(CPolygon & Polygon,const t_HyperPlane & HyperPlane)
{
    //for each vertex in this polygon
    bool One_In_Front = false;
    bool One_In_Back = false;

    vector<CVector> Vertices = Polygon.GetVertices();

    for (int vert=0; vert< Vertices.size(); vert++)
    {
        float result = ( Vertices[vert] - HyperPlane.Pos)%HyperPlane.Normal;

        if ( result > 0 )
            One_In_Front = true;
        else if ( result < 0)
            One_In_Back = true;

        if (One_In_Front && One_In_Back) return SPANNING;
    }

    if (One_In_Front) return IN_FRONT_OF;
    else if (One_In_Back) return IN_BACK_OF;
    else return COINCIDENT;
}

int ClassifyPoly(CPolygon & Polygon, int Axis, float SplittingValue)
{
    //for each vertex in this polygon
    bool One_In_Front = false;
    bool One_In_Back = false;

    vector<CVector> Vertices = Polygon.GetVertices();

    for (int vert=0; vert< Vertices.size(); vert++)
    {

        float result;
        switch (Axis)
        {
        case KD_X :
            result = Vertices[vert].x - SplittingValue;
            break;
        case KD_Y :
            result = Vertices[vert].y - SplittingValue;
            break;
        case KD_Z :
            result = Vertices[vert].z - SplittingValue;
            break;
        }//end switch

        if ( result > 0 )
            One_In_Front = true;
        else if ( result < 0)
            One_In_Back = true;

        if (One_In_Front && One_In_Back) return SPANNING;
    }

    if (One_In_Front) return IN_FRONT_OF;
    else if (One_In_Back) return IN_BACK_OF;
    else return COINCIDENT;
}


CVector Ray_Plane_Intersect(CVector & start, CVector & end, t_HyperPlane & HyperPlane)
{
    CVector segment = end - start;
    float intersect = -1.0 * ((start - HyperPlane.Pos) % HyperPlane.Normal) / (HyperPlane.Normal % segment);
    return start + (segment * intersect);
}


void Split_Polygon(CPolygon & Polygon, t_HyperPlane & HyperPlane, CPolygon & Front, CPolygon & Back)
{
    //set the properties for the two new polygons
    // Front.type = Back.type = Polygon.type;

    //start the splitting

    vector<CVector> Vertices = Polygon.GetVertices();
    vector<CVector> FrontVerts;
    vector<CVector> BackVerts;

    int  count = Vertices.size();
    CVector * ptA;
    CVector * ptB;
    float sideA, sideB;

    ptA = &Vertices[count - 1];
    sideA = (*ptA - HyperPlane.Pos)%HyperPlane.Normal;                                                                                          //sideA = ClassifyPpart->Classify_Point (ptA);
    for (int i = 0; i < count; i++)
    {
        ptB = &Vertices[i];
        sideB = (*ptB - HyperPlane.Pos) % HyperPlane.Normal;
        if (sideB > 0)
        {
            if (sideA < 0)
            {

                CVector v = *ptB - *ptA;
                float sect = -1 * ((*ptA - HyperPlane.Pos) % HyperPlane.Normal) / (HyperPlane.Normal % v);

                FrontVerts.push_back(*ptA + (v * sect));
                BackVerts.push_back(*ptA + (v * sect));
            }

            FrontVerts.push_back(*ptB);
        }
        else if (sideB < 0)
        {
            if (sideA > 0)
            {



                CVector v = *ptB - *ptA;
                float sect = -1 * ((*ptA - HyperPlane.Pos) % HyperPlane.Normal) / (HyperPlane.Normal % v);
                FrontVerts.push_back(*ptA + (v * sect));
                BackVerts.push_back(*ptA + (v * sect));
            }


            BackVerts.push_back(*ptB);
        }
        else
        {


            BackVerts.push_back(*ptB);
            FrontVerts.push_back(*ptB);
        }

        ptA = ptB;
        sideA = sideB;
    }
    Front.Create(FrontVerts);
    Back.Create(BackVerts);

}




void RenderBoundingBox(t_BoundingBox & BBox)
{
    float * maxs = BBox.vMax;
    float * mins = BBox.vMin;

    glColor3f(1,0,0);
    glBegin(GL_LINES);
    glVertex3f(mins[0], mins[1], mins[2]); //bottom front
    glVertex3f(maxs[0], mins[1], mins[2]);

    glVertex3f(mins[0],mins[1], mins[2]); //left front
    glVertex3f(mins[0],mins[1], maxs[2]);

    glVertex3f(maxs[0], mins[1],  mins[2]); //right front
    glVertex3f( maxs[0], mins[1],  maxs[2]);

    glVertex3f( mins[0], mins[1],  maxs[2]); //top front
    glVertex3f( maxs[0], mins[1],  maxs[2]);


    glVertex3f( mins[0], maxs[1],  mins[2]); //bottom back
    glVertex3f( maxs[0], maxs[1],  mins[2]);

    glVertex3f( mins[0], maxs[1],  mins[2]); //left back
    glVertex3f( mins[0], maxs[1],  maxs[2]);

    glVertex3f( maxs[0], maxs[1],  mins[2]); //right back
    glVertex3f( maxs[0], maxs[1],  maxs[2]);

    glVertex3f( mins[0], maxs[1],  maxs[2]); //top back
    glVertex3f( maxs[0], maxs[1],  maxs[2]);


    glVertex3f( mins[0], mins[1],  mins[2]); //left bottom side
    glVertex3f( mins[0], maxs[1],  mins[2]);

    glVertex3f( mins[0], mins[1],  maxs[2]); //left top side
    glVertex3f( mins[0], maxs[1],  maxs[2]);

    glVertex3f( maxs[0], mins[1],  mins[2]); //right bottom side
    glVertex3f( maxs[0], maxs[1],  mins[2]);

    glVertex3f( maxs[0], mins[1],  maxs[2]); //right top side
    glVertex3f( maxs[0], maxs[1],  maxs[2]);
    glEnd();

}




t_BoundingBox MakeBoundingBox(vector<CRegion *> & Region_list)
{
    t_BoundingBox BBox;

    if (!Region_list.size()) return BBox;

    CVector FirstVert = Region_list[0]->Polygon->GetVertices()[0];

    BBox.vMax[0] = BBox.vMin[0] = FirstVert.x;
    BBox.vMax[1] = BBox.vMin[1] = FirstVert.y;
    BBox.vMax[2] = BBox.vMin[2] = FirstVert.z;

    int numpolys = Region_list.size();
    for (int p=0; p<numpolys; p++)
    {


        vector<CVector> Vertices = Region_list[p]->Polygon->GetVertices();

        int numvertices = Vertices.size();

        for (int v = 0; v<numvertices; v++)
        {
            CVector * Vertex = &Vertices[v];
            if (Vertex->x > BBox.vMax[0]) BBox.vMax[0] = Vertex->x;
            if (Vertex->y > BBox.vMax[1]) BBox.vMax[1] = Vertex->y;
            if (Vertex->z > BBox.vMax[2]) BBox.vMax[2] = Vertex->z;

            if (Vertex->x < BBox.vMin[0]) BBox.vMin[0] = Vertex->x;
            if (Vertex->y < BBox.vMin[1]) BBox.vMin[1] = Vertex->y;
            if (Vertex->z < BBox.vMin[2]) BBox.vMin[2] = Vertex->z;

        }
    }

    return BBox;
}

t_BoundingBox MakeBoundingBox(vector<CPolygon> & Polygon_list)
{
    t_BoundingBox BBox;

    if (!Polygon_list.size()) return BBox;

    CVector FirstVert;// = Polygon_list[0].GetVertices()[0];
    
    for(int p=0; p<Polygon_list.size(); p++)
      if(Polygon_list[p].NumVerts())
      {
        FirstVert = Polygon_list[p].GetVertices()[0];
        break;
      } 

    BBox.vMax[0] = BBox.vMin[0] = FirstVert.x;
    BBox.vMax[1] = BBox.vMin[1] = FirstVert.y;
    BBox.vMax[2] = BBox.vMin[2] = FirstVert.z;

    int numpolys = Polygon_list.size();
    for (int p=0; p<numpolys; p++)
    {


        vector<CVector> Vertices = Polygon_list[p].GetVertices();

        int numvertices = Vertices.size();

        for (int v = 0; v<numvertices; v++)
        {
            CVector * Vertex = &Vertices[v];
            if (Vertex->x > BBox.vMax[0]) BBox.vMax[0] = Vertex->x;
            if (Vertex->y > BBox.vMax[1]) BBox.vMax[1] = Vertex->y;
            if (Vertex->z > BBox.vMax[2]) BBox.vMax[2] = Vertex->z;

            if (Vertex->x < BBox.vMin[0]) BBox.vMin[0] = Vertex->x;
            if (Vertex->y < BBox.vMin[1]) BBox.vMin[1] = Vertex->y;
            if (Vertex->z < BBox.vMin[2]) BBox.vMin[2] = Vertex->z;

        }
    }

    return BBox;
}//end Make Bounding Box


t_TempNode * Create_BSP_Tree(vector<CPolygon> & Polygon_list)
{
    const bool debug = false;
    if (debug) cout<<"Create BSP Tree"<<endl;

    t_TempNode * Tree = new t_TempNode;
    Tree->Front = 0;
    Tree->Back = 0;
    Build_BSP_Tree(Tree,Polygon_list,ALIGNX);

    if (debug) cout<<"End Create BSP Tree"<<endl;
    return Tree;
}

t_KDNode * Create_KD_Tree(vector< CRegion *> & Region_list)
{
    t_KDNode * Tree =  new t_KDNode;
    Tree->BoundingBox = MakeBoundingBox(Region_list);

    Build_KD_Tree(Tree,Region_list,KD_Z);

    return Tree;
}
//need to pass the last axis and splitting value for use in the new bounding box.
//right now, it recomputes the bounding box around even the shared regions, thus the
//splitting plane ends up in the same damned place.
//*Easiest solution is to pass the BB in

void Build_KD_Tree(t_KDNode * Tree, vector<CRegion *>  Region_list, int axis)
{
    bool debug = false;

    if (debug) cout<<"Start New KD Node"<<endl;
    if (debug) cout<<"Num Regions :"<<Region_list.size()<<endl;
    if (debug) cout<<"Plane : "<<(int)axis<<endl;


    //If we have less than the number we allow per leaf, just create the leaf and quit
    if (Region_list.size() <= 15)
    {
        for (int p=0; p<Region_list.size(); p++)
        {
            Tree->Regions.push_back(Region_list[p]);

        }
        Region_list.clear();
        return;
    }

    //create the axis-aligned splitting plane

    if (axis == KD_X)
    {
        Tree->Axis = KD_X;
        Tree->SplittingValue = (Tree->BoundingBox.vMax[0] + Tree->BoundingBox.vMin[0]) /2;
    }
    else if (axis == KD_Y)
    {
        Tree->Axis = KD_Y;
        Tree->SplittingValue = (Tree->BoundingBox.vMax[1] + Tree->BoundingBox.vMin[1]) /2;
    }
    else
    {
        Tree->Axis = KD_Z;
        Tree->SplittingValue = (Tree->BoundingBox.vMax[2] + Tree->BoundingBox.vMin[2]) /2;
    }

    if (debug) cout<<"Splitting value :"<<Tree->SplittingValue<<endl;
    vector<CRegion *> Front_list;
    vector<CRegion *> Back_list;

    CRegion * Region;

    int NumSpanning = 0;
    while (Region_list.size())
    {
        Region = Region_list.back();
        Region_list.pop_back();

        int classification = ClassifyPoly(*(Region->Polygon),Tree->Axis,Tree->SplittingValue);
        switch (classification)
        {
        case COINCIDENT:
            if (debug) cout<<"Coincident"<<endl;
            Front_list.push_back(Region);
            break;
        case IN_BACK_OF:
            if (debug) cout<<"Back Poly"<<endl;
            Back_list.push_back(Region);
            break;
        case IN_FRONT_OF:
            if (debug) cout<<"Front Poly"<<endl;
            Front_list.push_back(Region);
            break;
        case SPANNING:
            if (debug) cout<<"Spanning Poly"<<endl;
            Back_list.push_back(Region);
            Front_list.push_back(Region);
            NumSpanning ++;
            break;
        }//end switch
    }//end while

    Region_list.clear();
    if ((Front_list.size() > 0) && (Front_list.size() > NumSpanning))
    {
        Tree->Front = new t_KDNode;
        Tree->Front->BoundingBox = Tree->BoundingBox;
        Tree->Front->BoundingBox.vMin[axis] = Tree->SplittingValue;
        Build_KD_Tree (Tree->Front, Front_list,((axis +1 )%3));
    }
    Front_list.clear();


    if ((Back_list.size() > 0) && (Back_list.size() > NumSpanning))
    {
        Tree->Back = new t_KDNode;
        Tree->Back->BoundingBox = Tree->BoundingBox;
        Tree->Back->BoundingBox.vMax[axis] = Tree->SplittingValue;
        Build_KD_Tree (Tree->Back, Back_list,((axis + 1)%3));
    }
    Back_list.clear();


    if (debug) cout<<"End KD Compile"<<endl;


}

void Build_BSP_Tree(t_TempNode * Tree, vector<CPolygon> Polygon_list,int flag)
{
    const bool debug = false;

    Tree->BoundingBox = MakeBoundingBox(Polygon_list);
    if (debug) 
    {
       cout<<"Made bounding box:"<<endl;
       cout<<"Max : "<<Tree->BoundingBox.vMax[0]<<" "<<Tree->BoundingBox.vMax[1]<<" "<<Tree->BoundingBox.vMax[2]<<endl;
       cout<<"Min : "<<Tree->BoundingBox.vMin[0]<<" "<<Tree->BoundingBox.vMin[1]<<" "<<Tree->BoundingBox.vMin[2]<<endl;
    }
    

    if (Polygon_list.size() < 500)
    {
        for (int p=0; p<Polygon_list.size(); p++)
        {
            Tree->Polygons.push_back(Polygon_list[p]);
            Tree->Front = NULL;
            Tree->Back = NULL;

        }
        Polygon_list.clear();
        return;

    }

    Tree->HyperPlane.Pos.x = (Tree->BoundingBox.vMax[0] + Tree->BoundingBox.vMin[0]) /2.0;
    Tree->HyperPlane.Pos.y = (Tree->BoundingBox.vMax[1] + Tree->BoundingBox.vMin[1]) /2.0;
    Tree->HyperPlane.Pos.z = (Tree->BoundingBox.vMax[2] + Tree->BoundingBox.vMin[2]) /2.0;

    if (debug) cout<<"Set Hyperplane Position :"<<Tree->HyperPlane.Pos<<endl;

    //create the splitting plane
    if (flag == ALIGNX)
    {
        Tree->HyperPlane.Normal.Set(1,0,0);
        Tree->Axis = ALIGNX;
        Tree->SplittingValue = Tree->HyperPlane.Pos.x;
        if(debug) cout<<"AlignX"<<endl;
    }
    else if (flag == ALIGNY)
    {
        Tree->HyperPlane.Normal.Set(0,1,0);
        Tree->Axis = ALIGNY;
        Tree->SplittingValue = Tree->HyperPlane.Pos.y;
        if(debug) cout<<"AlignY"<<endl;
    }
    else
    {
        Tree->HyperPlane.Normal.Set(0,0,1);
        Tree->Axis = ALIGNZ;
        Tree->SplittingValue = Tree->HyperPlane.Pos.z;
        if(debug) cout<<"AlignZ"<<endl;
    }

    if (debug) cout<<"Set Hyperplane Normal : "<<Tree->HyperPlane.Normal<<endl;



    vector<CPolygon> Front_list;
    vector<CPolygon> Back_list;

    CPolygon Polygon;


    if (debug) cout<<" Dividing NumPolygons : "<<Polygon_list.size()<<endl;
    while ( Polygon_list.size())
    {
        Polygon = Polygon_list.back();
        Polygon_list.pop_back();



        int classification = ClassifyPoly(Polygon,Tree->HyperPlane);
        switch (classification)
        {
        case COINCIDENT:
            //if (debug) cout<<"Coincident"<<endl;
            Front_list.push_back(Polygon);
            break;
        case IN_BACK_OF:
            //if (debug) cout<<"Back Poly"<<endl;
            Back_list.push_back(Polygon);
            break;
        case IN_FRONT_OF:
            //if (debug) cout<<"Front Poly"<<endl;
            Front_list.push_back(Polygon);
            break;
        case SPANNING:
            //if (debug) cout<<"Splitting Polygon with "<<Polygon.NumVerts()<<" Vertices : "<<endl;
            CPolygon FrontHalf, BackHalf;
            Split_Polygon(Polygon,Tree->HyperPlane,FrontHalf,BackHalf);
            //if (debug)cout<<"Back["<<BackHalf.NumVerts()<<"], Front["<<FrontHalf.NumVerts()<<"]"<<endl;
            BackHalf.SetColor(Polygon.GetColor());
            FrontHalf.SetColor(Polygon.GetColor());
            Back_list.push_back(BackHalf);
            Front_list.push_back(FrontHalf);
            break;

        }//end switch
    }//end while
    if(debug) cout<<"finished dividing polygon population"<<endl<<endl;

    Polygon_list.clear();
    if (Front_list.size())
    {
        Tree->Front = new t_TempNode;
        Tree->Front->Front = NULL;
        Tree->Front->Back = NULL;
        if(debug) cout<<"recursing front branch with "<<Front_list.size()<<" polygons"<<endl;
        Build_BSP_Tree (Tree->Front, Front_list,(flag+1)%3);
    }
    Front_list.clear();


    if (Back_list.size())
    {
        Tree->Back = new t_TempNode;
        Tree->Back->Front = NULL;
        Tree->Back->Back = NULL;
        if(debug) cout<<"recursing back branch with "<<Back_list.size()<<" polygons"<<endl;
        Build_BSP_Tree (Tree->Back, Back_list,(flag+1)%3);
    }
    Back_list.clear();


}//end Build BSP Tree




void DeleteTree(t_TempNode * root)
{

    if (root->Back) DeleteTree(root->Back);
    if (root->Front) DeleteTree(root->Front);
    root->Back = NULL;
    root->Front = NULL;

    if (root->Polygons.size())
        glDeleteLists(root->ListID,1);

    delete root;
}

void DeleteTree(t_KDNode * root)
{

    if (root->Back) DeleteTree(root->Back);
    if (root->Front) DeleteTree(root->Front);
    root->Back = NULL;
    root->Front = NULL;

    delete root;
}



void RenderTree(t_TempNode * root,bool DrawBounds, CFrustrum & Frustum,bool testing, int &PolysDrawn)
{
    bool debug = false;


    if (DrawBounds) RenderBoundingBox(root->BoundingBox);
    if (debug) cout<<"drew bounding box"<<endl;

    if (root->Back)
    {
        float * vMax = root->Back->BoundingBox.vMax;
        float * vMin = root->Back->BoundingBox.vMin;
        if (testing)
        {
            int result = Frustum.BoxInFrustum(vMin[0],vMin[1],vMin[2],vMax[0],vMax[1],vMax[2]);
            if (result == COMPLETE_IN)
                RenderTree(root->Back,DrawBounds,Frustum,false,PolysDrawn);
            else if (result == INTERSECT)
                RenderTree(root->Back,DrawBounds,Frustum,true,PolysDrawn);

        }
        else
            RenderTree(root->Back,DrawBounds,Frustum,false,PolysDrawn);

    }

    if (root->Front)
    {
        float * vMax = root->Front->BoundingBox.vMax;
        float * vMin = root->Front->BoundingBox.vMin;
        if (testing)
        {
            int result = Frustum.BoxInFrustum(vMin[0],vMin[1],vMin[2],vMax[0],vMax[1],vMax[2]);
            if (result == COMPLETE_IN)
                RenderTree(root->Front,DrawBounds,Frustum,false,PolysDrawn);
            else if (result == INTERSECT)
                RenderTree(root->Front,DrawBounds,Frustum,true,PolysDrawn);
        }
        else
            RenderTree(root->Front,DrawBounds,Frustum,false,PolysDrawn);
    }


    for (int p=0; p<root->Polygons.size(); p++)
    {
        //RenderPolygon(root->Polygons[p]);
        root->Polygons[p].Draw();
        PolysDrawn++;
    }
    if (debug) cout<<"drew polygons"<<endl;
}

void RenderTree(CCamera * Cam, t_KDNode * root,bool DrawBounds,CFrustrum & Frustum, bool testing, int &RegionsDrawn, CBitset & Drawn, float FrameTime)
{
    if (DrawBounds) RenderBoundingBox(root->BoundingBox);

    if (root->Back)
    {
        float * vMax = root->Back->BoundingBox.vMax;
        float * vMin = root->Back->BoundingBox.vMin;
        if (testing)
        {
            int result = Frustum.BoxInFrustum(vMin[0],vMin[1],vMin[2],vMax[0],vMax[1],vMax[2]);
            if (result == COMPLETE_IN)
                RenderTree(Cam, root->Back,DrawBounds,Frustum,false,RegionsDrawn , Drawn, FrameTime);
            else if (result == INTERSECT)
                RenderTree(Cam, root->Back,DrawBounds,Frustum,true,RegionsDrawn, Drawn, FrameTime);

        }
        else
            RenderTree(Cam, root->Back,DrawBounds,Frustum,false,RegionsDrawn, Drawn, FrameTime);

    }

    if (root->Front)
    {
        float * vMax = root->Front->BoundingBox.vMax;
        float * vMin = root->Front->BoundingBox.vMin;
        if (testing)
        {
            int result = Frustum.BoxInFrustum(vMin[0],vMin[1],vMin[2],vMax[0],vMax[1],vMax[2]);
            if (result == COMPLETE_IN)
                RenderTree(Cam, root->Front,DrawBounds,Frustum,false,RegionsDrawn, Drawn, FrameTime);
            else if (result == INTERSECT)
                RenderTree(Cam, root->Front,DrawBounds,Frustum,true,RegionsDrawn, Drawn, FrameTime);
        }
        else
            RenderTree(Cam, root->Front,DrawBounds,Frustum,false,RegionsDrawn, Drawn, FrameTime);
    }


    for (int p=0; p<root->Regions.size(); p++)
    {
        if (!Drawn.On(root->Regions[p]->GetID()))
        {
            root->Regions[p]->Draw(FrameTime,Cam);
            RegionsDrawn++;
            Drawn.Set(root->Regions[p]->GetID());
        }
    }
}//end draw KD tree

void CullDuplicatePolys(t_TempNode * root)
{
    if (root->Back) CullDuplicatePolys(root->Back);
    if (root->Front) CullDuplicatePolys(root->Front);

    for (int p=0; p < root->Polygons.size(); p++)
    {

        for (int p2= (p+1); p2 < root->Polygons.size(); p2++)
        {
            if (root->Polygons[p] == root->Polygons[p2])
            {
                root->Polygons.erase(root->Polygons.begin() + p2);
                p2--;
            }

        }

    }
}

void TreeCreateDisplayLists(t_TempNode * root)
{

    if (root->Back) TreeCreateDisplayLists(root->Back);
    if (root->Front) TreeCreateDisplayLists(root->Front);

    root->ListID = glGenLists(1);
    glNewList(root->ListID,GL_COMPILE);

    for (int p=0; p<root->Polygons.size(); p++)
    {
        root->Polygons[p].Draw();
    }

    glEndList();
}



void RenderTreeLists(t_TempNode * root,bool DrawBounds, CFrustrum & Frustum,bool testing, int &PolysDrawn)
{
    if (DrawBounds) RenderBoundingBox(root->BoundingBox);

    if (root->Back)
    {
        float * vMax = root->Back->BoundingBox.vMax;
        float * vMin = root->Back->BoundingBox.vMin;
        if (testing)
        {
            int result = Frustum.BoxInFrustum(vMin[0],vMin[1],vMin[2],vMax[0],vMax[1],vMax[2]);
            if (result == COMPLETE_IN)
                RenderTreeLists(root->Back,DrawBounds,Frustum,false,PolysDrawn);
            else if (result == INTERSECT)
                RenderTreeLists(root->Back,DrawBounds,Frustum,true,PolysDrawn);

        }
        else
            RenderTreeLists(root->Back,DrawBounds,Frustum,false,PolysDrawn);

    }

    if (root->Front)
    {
        float * vMax = root->Front->BoundingBox.vMax;
        float * vMin = root->Front->BoundingBox.vMin;
        if (testing)
        {
            int result = Frustum.BoxInFrustum(vMin[0],vMin[1],vMin[2],vMax[0],vMax[1],vMax[2]);
            if (result == COMPLETE_IN)
                RenderTreeLists(root->Front,DrawBounds,Frustum,false,PolysDrawn);
            else if (result == INTERSECT)
                RenderTreeLists(root->Front,DrawBounds,Frustum,true,PolysDrawn);
        }
        else
            RenderTreeLists(root->Front,DrawBounds,Frustum,false,PolysDrawn);
    }

    glCallList(root->ListID);
    PolysDrawn+= root->Polygons.size();
}




