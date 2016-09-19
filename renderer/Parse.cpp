/*********************************************************
** parse.cpp, parse.h
** Primary author: Mike J
** Description: file loaders
** Used in sim: pathing map load
*********************************************************/


#include "Parse.h"

#include "entity.h"
#include "APSP.h"
#include <fstream>
#include <iostream>
#include <vector>

#include <map>

using namespace std;

char * g_EndTag;
char * g_FileEnd;
char * g_FileStart;
char * g_EndParam;
char * g_EndValue;

const int NAV_FORMAT_ID = 1337;


vector< vector< PortalInfo> > ProcessPortals(vector<CPolygon * > Polygon_list)
{
    bool debug = false;

    vector< vector< PortalInfo > > PortalInfo_list;

    for (int p=0; p<Polygon_list.size(); p++)
    {
        vector<PortalInfo> aportalinfo;
        PortalInfo_list.push_back(aportalinfo);
    }

    int NumPolys = Polygon_list.size();
    for (int p1=0; p1<NumPolys; p1++)
    {
        for (int p2=0; p2<NumPolys; p2++)
        {
            //don't let a polygon check against itself
            if (p1 == p2) continue;
            if (Polygon_list[p1]->OverlapsXY(Polygon_list[p2])) continue;

            vector<CVector> Verts1 = Polygon_list[p1]->GetVertices();
            int Numverts1 = Verts1.size();
            vector<CVector> Verts2 = Polygon_list[p2]->GetVertices();
            int Numverts2 = Verts2.size();

            for (int v1=0; v1< Numverts1; v1++)
            {
                for (int v2=0; v2 < Numverts2; v2++)
                {


                    if ((Verts1[v1] == Verts2[(v2 + 1) % Numverts2]) && (Verts1[(v1 + 1) % Numverts1] == Verts2[v2])  )
                    {
                        //common edge found
                        CVector P1 = Verts1[v1];
                        CVector P2 = Verts1[(v1 + 1) % Numverts1];
                        PortalInfo Info1;
                        Info1.P1 = P1;
                        Info1.P2 = P2;
                        Info1.DestinationIndex = p2;
                        Info1.Up = Polygon_list[p1]->GetNormal();


                        if ((Info1.P1 - Info1.P2).Length() <= (Entity::LARGEST_ENTITY_RADIUS * 2.2))
                        {
                            Info1.color = RED;

                        }
                        else
                        {
                            Info1.color = GREEN;

                        }

                        //add both portals
                        PortalInfo_list[p1].push_back(Info1);


                    }
                }
            }
        }//end for all polys 2
    }//end for all polys


    //DETECT THE PORTALS WITH DUPLICATE DESTINATIONS
    for (int r=0; r<Polygon_list.size(); r++)
    {
        for (int p=0; p< PortalInfo_list[r].size(); p++)
        {
            for (int p2=(p+1); p2<PortalInfo_list[r].size(); p2++)
                if (PortalInfo_list[r][p].DestinationIndex == PortalInfo_list[r][p2].DestinationIndex)
                {
                    PortalInfo_list[r][p2].color = YELLOW;
                    PortalInfo_list[r][p].color = YELLOW;
                }
        }
    }

    return PortalInfo_list;
}

bool ComputeBBFromOBJ(const char * filename, BoundingBox & BB)
{
    bool debug = false;
    float SCALE = 1;

    if (debug) cout<<"START BB Parse"<<endl;
    ifstream infile;
    infile.open(filename);
    if (!infile)
    {
        cout<<"cannot find input file"<<endl;
        return 0;
    }

    char header[60] = {0};
    char trashline[3000];

    vector< CVector > Vertices;

    while (infile)
    {
        //get the line header
        //for (int x=0; x<5; x++) header[x] = 0;

        infile>>header;

        if (strcmp(header,"v") == 0)
        {
            CVector temp;
            infile>>temp.x;
            if (debug) cout<<temp.x<<" ";
            infile>>temp.y;
            if (debug) cout<<temp.y<<" ";
            infile>>temp.z;
            if (debug) cout<<temp.z<<" "<<endl;
            Vertices.push_back(temp/SCALE);

        }
        else
        {
            infile.getline(trashline,3000);
        }
    }//end while

    //compute bounding box
    if (Vertices.size() < 1) return false;

    BB.minx = Vertices[0].x;
    BB.miny = Vertices[0].y;
    BB.minz = Vertices[0].z;

    BB.maxx = Vertices[0].x;
    BB.maxy = Vertices[0].y;
    BB.maxz = Vertices[0].z;

    for (int v=1; v<Vertices.size(); v++)
    {
        if (BB.minx > Vertices[v].x)  BB.minx =   Vertices[v].x;
        if (BB.maxx < Vertices[v].x)  BB.maxx =   Vertices[v].x;
        if (BB.miny > Vertices[v].y)  BB.miny =   Vertices[v].y;
        if (BB.maxy < Vertices[v].y)  BB.maxy =   Vertices[v].y;
        if (BB.minz > Vertices[v].z)  BB.minz =   Vertices[v].z;
        if (BB.maxz < Vertices[v].z)  BB.maxz =   Vertices[v].z;
    }

    //compute the lengths along the axes
    BB.lx = BB.maxx - BB.minx;
    BB.ly = BB.maxy - BB.miny;
    BB.lz = BB.maxz - BB.minz;

    return true;
}


//this function is only used for loading the pathing map info

bool BadPoly(CPolygon * Polygon)
{
    if (Polygon->NumVerts() < 3) return true;
    if (Polygon->IsVertical()) return true;
    if (Polygon->GetArea() == 0.0) return true;
    //if concave

    return false;
}

bool LoadPolyListOBJ(vector<CPolygon> & Polygon_list, const char * filename)
{
    bool debug = true;
    int badface = 0;
    
    float SCALE = 1;

    if (debug) cout<<"START LoadPolyListOBJ"<<endl;
    ifstream infile;
    infile.open(filename);
    if (!infile)
    {
        cout<<"cannot find input file"<<endl;
        return 0;
    }

    char header[160] = {0};
    char trashline[15000] = {0};

    vector< CVector > Vertices;
    vector< vector<int> > Faces;

    while (infile)
    {
        //get the line header
        infile>>header;

        if (strcmp(header,"v") == 0)
        {
            CVector temp;
            infile>>temp.x;
            infile>>temp.y;
            infile>>temp.z;
            Vertices.push_back(temp/SCALE);
            
            if(debug)
                cout<<"Vertex : "<<temp.x<<" "<<temp.y<<" "<<temp.z<<endl;

        }
        else if (strcmp(header,"f") == 0)
        {
            int counter = 3;
            char line[3000];

            infile.getline(line,3000);
            vector<int> Face;
            Faces.push_back(Face);

            char * token;

            //get the first token of the line
            token = strtok(line,"/ \n");
           // if (debug) cout<<endl<<token<<" ";
            Faces[Faces.size()-1].push_back(atoi(token)-1);
            counter++;

            while (token = strtok(NULL,"/ \n"))
            {
                if ((counter % 3) == 0)
                    Faces[Faces.size()-1].push_back(atoi(token)-1);

                counter++;
            }

            vector<CVector> Verts;
            int faceindex = Faces.size() -1;
            for (int v=0; v<Faces[faceindex].size(); v++)
            {
                Verts.push_back(Vertices[Faces[faceindex][v]]);
            }

              
            CPolygon * Polygon = new CPolygon(Verts);

            if(debug) cout<<"Face Made"<<endl;
            
            if((Polygon->NumVerts() >= 3) && (Polygon->GetArea() > 0.0))
            {
              Polygon_list.push_back(*Polygon);
              if(debug) cout<<"Face pushed"<<endl;
            }
            else
            {
               if(debug) cout<<"Bad face"<<endl;
               badface++;
            }

            delete Polygon;
           
        }// end if strcmp f
        else
            infile.getline(trashline,15000);

    }//end while
    
    if(debug) cout<<"NUM BAD FACES : "<<badface<<endl;

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool LoadRegionListOBJ(vector< CRegion * > & Region_list, const char * filename)
{
    
	 bool debug = false;
    float SCALE = 1;

	 float snap = 0.01;
	 map <int, int> vertexmap;

    if (debug) cout<<"START LoadRegionListOBJ"<<endl;
    ifstream infile;
    infile.open(filename);
    if (!infile)
    {
        cout<<"cannot find input file"<<endl;
        return 0;
    }

    char header[60] = {0};
    char trashline[3000] = {0};

  	vector< vector <int> > Faces;
    vector< CVector > Vertices;
    vector< CPolygon * > Polygon_list;
    vector<int> TextureStatus;
    int TEXTURE_STATE = PATHING_DEFAULT;

    while (infile)
    {
        //get the line header
        infile>>header;

        if (strcmp(header,"v") == 0)
        {
            CVector vert;
            infile>>vert.x;
            if (debug) cout<<vert.x<<" ";
            infile>>vert.y;
            if (debug) cout<<vert.y<<" ";
            infile>>vert.z;
            if (debug) cout<<vert.z<<" "<<endl;

				vert /= SCALE;

				vector <int> duplicateID;

				for (int i = 0; i < Vertices.size(); i++)
				{
				 if ( (vert - Vertices[i]).Length() < snap)
				 {
				  if (debug) cout << "Duplicate vertex found: " << i << "\t" << Vertices.size() <<endl;
				  duplicateID.push_back(i);
				 }
				}

			while (!duplicateID.empty())
			{
       int dupe = duplicateID[0];
       duplicateID.erase(duplicateID.begin());

			 vert = (vert + Vertices[dupe]) / 2.0;
			 
			 vertexmap[dupe] = Vertices.size();
 			 
			 if (debug) cout << "Replaced Duplicate vertex ID: " << dupe << " with " << Vertices.size() <<endl;
			}
		
			vertexmap[Vertices.size()] = Vertices.size();
			Vertices.push_back(vert);
     }

        else if (strcmp(header,"f") == 0)
        {
            int counter = 3;
            char line[3000];

            infile.getline(line,3000);
            vector<int> Face;
            Faces.push_back(Face);

            char * token;

            //get the first token of the line
            token = strtok(line,"/ \n");
            if (debug) cout<<endl<<token<<" ";
            Faces[Faces.size()-1].push_back(atoi(token)-1);
            counter++;

            while (token = strtok(NULL,"/ \n"))
            {
                if ((counter % 3) == 0)
                    Faces[Faces.size()-1].push_back(atoi(token)-1);

                if (debug)
                    cout<<token<<" ";

                counter++;
            }
            if (debug) cout<<endl;

            //create the polygon
				
            vector<CVector> Verts;
            int faceindex = Faces.size() -1;

            for (int v=0; v<Faces[faceindex].size(); v++)
            {
             Verts.push_back(Vertices[vertexmap[Faces[faceindex][v]]]);
            }

            CPolygon * Polygon = new CPolygon(Verts,0.2);

            //test to see if the Polygons are valid
            if (!BadPoly(Polygon))
            {
                Polygon_list.push_back(Polygon);
                Polygon->SetColor(BLACK);
                TextureStatus.push_back(TEXTURE_STATE);
            }
            else
                delete Polygon;


        }// end if strcmp f (face)

        else if (strcmp(header,"usemtl") == 0)
        {
            //get the first token of the line
            char line[3000];
            infile.getline(line,3000);

            char * token = strtok(line,"/ \n");
            if (strcmp(token,"EXITEXIT") == 0)
                TEXTURE_STATE = PATHING_EXIT;
            else if (strcmp(token,"STAIRS") == 0)
                TEXTURE_STATE = PATHING_STAIRS;
            else if (strcmp(token,"SEATING") == 0)
                TEXTURE_STATE = PATHING_SEATING;
            else if (strcmp(token,"WAYPOINT") == 0)
                TEXTURE_STATE = PATHING_WAYPOINT;
            else
                TEXTURE_STATE = PATHING_DEFAULT;
        }
        else
            infile.getline(trashline,3000);

    }//end while

    vector< PortalInfo > Portalvector;
    vector< vector< PortalInfo > > PortalInfo_List = ProcessPortals(Polygon_list);

    //create the regions

    int NumRegions = Polygon_list.size();
    if (debug) cout<<"NUMREGIONS : "<<NumRegions<<endl;
    for (int r=0; r< NumRegions; r++)
    {
        CRegion * ptRegion = new CRegion(Polygon_list[r]->GetVertices(),PortalInfo_List[r],TextureStatus[r]);
        ptRegion->SetID(r);
        Region_list.push_back(ptRegion);
        if (debug) cout<<endl<<endl;
    }

    infile.close();
    if (debug) cout<<"END LoadRegionListOBJ"<<endl;

    //cleanup
    for (int p = 0; p < Polygon_list.size(); p++)
    {
        if (Polygon_list[p]) delete Polygon_list[p];
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////

bool GetBBFromNAV(const char * filename, BoundingBox & BB)
{
    const  bool debug = false;

    ifstream infile;
    infile.open(filename,ios::in | ios::binary);
    if (!infile)
    {
        cout<<"cannot find input file"<<endl;
        return 0;
    }

    int Format_ID;
    infile.read((char *)&Format_ID,4);

    if (debug) cout<<"NAV BB Get:  formID : "<<Format_ID<<endl;
    if (Format_ID != NAV_FORMAT_ID)
    {
        if (debug) cout<<"Format ID didn't match"<<endl;
        return false;
    }

    infile.read((char *)&BB.minx,4);
    infile.read((char *)&BB.miny,4);
    infile.read((char *)&BB.minz,4);

    infile.read((char *)&BB.maxx,4);
    infile.read((char *)&BB.maxy,4);
    infile.read((char *)&BB.maxz,4);

    if (debug)  cout<<" "<<BB.minx<<" "<<BB.miny<<" "<<BB.minz<<" "<<BB.maxx<<" "<<BB.maxy<<" "<<BB.maxz<<endl;

    infile.close();

    return true;
}

bool WriteNAV(char * NAVFilename, BoundingBox & BB, vector<CRegion *> & Region_List, std::vector<std::vector<float> > &D, std::vector<std::vector<int> > &P)
{
    bool debug = false;

    //output the NAV file
    ofstream outfile;
    outfile.open(NAVFilename,ios::out | ios::binary);
    if (!outfile)
    {
        cout<<"cannot open output file "<<NAVFilename<<endl;
        return false;
    }

    outfile.write((char *)&NAV_FORMAT_ID,4);

    //DATA SECTION 1 : write out the bounding box

    outfile.write((char *)&BB.minx,4);
    outfile.write((char *)&BB.miny,4);
    outfile.write((char *)&BB.minz,4);

    outfile.write((char *)&BB.maxx,4);
    outfile.write((char *)&BB.maxy,4);
    outfile.write((char *)&BB.maxz,4);

    //DATA SECTION 2 : write out the Region Data

    int NumRegions = Region_List.size();

    if (debug) cout<<"Numregions : "<<NumRegions<<endl;

    outfile.write((char *)&NumRegions,4);

    for (int r=0; r< NumRegions; r++)
    {
        vector<CVector> Verts = Region_List[r]->Polygon->GetVertices();

        if (Verts.size() < 3) continue;

        //output the Region Header
        int NumVerts = Verts.size();
        outfile.write((char *)&NumVerts,4);

        int NumPortals = Region_List[r]->Portals.size();
        outfile.write((char *)&NumPortals,4);

        int Pathing_Attribute = Region_List[r]->GetPathingDifficulty();
        outfile.write((char *)&Pathing_Attribute,4);

        //output the verts
        for (int v=0; v< NumVerts; v++)
        {
            float vx = Verts[v].x;
            float vy = Verts[v].y;
            float vz = Verts[v].z;

            outfile.write((char *)&vx,4);
            outfile.write((char *)&vy,4);
            outfile.write((char *)&vz,4);
        }

        //output the portals
        for (int p=0; p < NumPortals; p++)
        {
            CPortal * Portal = Region_List[r]->Portals[p];

            //write out Point 1
            CVector P1 = Portal->GetPoint1();
            outfile.write((char *)&P1.x,4);
            outfile.write((char *)&P1.y,4);
            outfile.write((char *)&P1.z,4);

            //write out Point 2
            CVector P2 = Portal->GetPoint2();
            outfile.write((char *)&P2.x,4);
            outfile.write((char *)&P2.y,4);
            outfile.write((char *)&P2.z,4);

            //write the destination
            int DestinationIndex = Portal->GetDest();
            outfile.write((char *)&DestinationIndex,4);

            //write out the Up Vector
            CVector Up =  Region_List[r]->Polygon->GetNormal();//Portal->GetNormal() ^ (P1 - P2).UnitVector();
            outfile.write((char *)&Up.x,4);
            outfile.write((char *)&Up.y,4);
            outfile.write((char *)&Up.z,4);
        }
    }//end for each region

    //write out the pathing array
    for (int row=0; row<Region_List.size(); row++)
        for (int col = 0; col< Region_List.size(); col++)
        {
            int value = P[row][col];
            outfile.write((char *)&value,4);
        }

    //write out the distance array
    for (int row=0; row<Region_List.size(); row++)
        for (int col = 0; col< Region_List.size(); col++)
        {
            float value = D[row][col];
            outfile.write((char *)&value,4);
        }

    outfile.close();

    if (debug) cout<<"End OBJ to NAV"<<endl;

}//end WriteNAV

bool OBJtoNAV(char * OBJFilename, char * NAVFilename)
{
    const  bool debug = false;
    if (debug) cout<<"Start OBJ2NAV"<<endl;

    vector<CVector> Vertices;
    vector<CRegion * > Region_List;

    if (debug) cout<<"LoadRegionListOBJ"<<endl;
    if (!LoadRegionListOBJ(Region_List,OBJFilename))
    {
        for (int r=0; r<Region_List.size(); r++)
        {
            if (Region_List[r]) delete Region_List[r];
        }
        cout<<"Error in OBJtoNAV() : LoadRegionListOBJ() failed. "<<endl;
        return false;
    }
    if (debug) cout<<"ComputeBB"<<endl;
    BoundingBox BB;
    if (!ComputeBBFromOBJ(OBJFilename, BB))
    {
        for (int r=0; r<Region_List.size(); r++)
        {
            if (Region_List[r]) delete Region_List[r];
        }
        cout<<"Error in OBJtoNAV() : ComputeBBFromOBJ() failed. "<<endl;
        return false;
    }

    if (debug) cout<<"ComputeAdjacencyArray"<<endl;
    float * AdjArray = ComputeAdjacencyArray(Region_List);
    if (!AdjArray)
    {
        for (int r=0; r<Region_List.size(); r++)
        {
            if (Region_List[r]) delete Region_List[r];
        }
        cout<<"Error in OBJtoNAV() : ComputeAdjacencyArray() failed. "<<endl;
        return false;
    }

    vector<std::vector<float> > D;
    vector<std::vector<int> > P;
    D.resize(Region_List.size(),vector<float>(Region_List.size()));
    P.resize(Region_List.size(),vector<int>(Region_List.size()));

    if (debug) cout<<"Floyd APSP"<<endl;
    FloydAPSP (Region_List.size(), AdjArray,D,P);

    if (debug) cout<<"WriteNAVFile"<<endl;
    if (!WriteNAV(NAVFilename, BB, Region_List, D, P))
    {
        for (int r=0; r<Region_List.size(); r++)
        {
            if (Region_List[r]) delete Region_List[r];
        }
        cout<<"Error in OBJtoNAV() : WriteNAV() failed. "<<endl;
        return false;
    }

    if (debug) cout<<"cleanup"<<endl;
    for (int r=0; r<Region_List.size(); r++)
    {
        if (Region_List[r]) delete Region_List[r];
    }

    if (debug) cout<<"end OBJ2NAV"<<endl;

    return true;

}//end OBJtoNAV

bool LoadRegionListNAV(vector<CVector> & Vertices, vector< CRegion * > & Region_list, const char * filename, std::vector<std::vector<float> > &D, std::vector<std::vector<int> > &P)
{
    bool debug = false;

    if (debug) cout<<"Loading NAV"<<endl;
    ifstream infile;
    infile.open(filename,ios::in | ios::binary);
    if (!infile)
    {
        cout<<"cannot find NAV input file"<<endl;
        return false;
    }

    int Format_ID;
    infile.read((char *)&Format_ID,4);
    if (Format_ID != NAV_FORMAT_ID)
    {
        infile.close();
        cout<<"Cannot open "<<filename<<", not an NAV file."<<endl;
        return false;
    }

    //load the BB, but don't do anything with it.
    BoundingBox BB;

    infile.read((char *)&BB.minx,4);
    infile.read((char *)&BB.miny,4);
    infile.read((char *)&BB.minz,4);

    infile.read((char *)&BB.maxx,4);
    infile.read((char *)&BB.maxy,4);
    infile.read((char *)&BB.maxz,4);

    if (debug) cout<<"BB : "<<BB.minx<<" "<<BB.miny<<" "<<BB.minz<<" "<<BB.maxx<<" "<<BB.maxy<<" "<<BB.maxz<<endl;

    int NumRegions;

    infile.read((char *)&NumRegions,4);

    if (debug) cout<<"NAV numregions : "<<NumRegions<<endl;


    for (int r=0; r< NumRegions; r++)
    {
        //read the Region Header
        int NumVerts;
        infile.read((char *)&NumVerts,4);
        int NumPortals;
        infile.read((char *)&NumPortals,4);
        int Pathing_Attribute;
        infile.read((char *)&Pathing_Attribute,4);

        if (debug) cout<<"NAV Header, Verts : "<<NumVerts<<", Portals : "<<NumPortals<<", Pathing Attr : "<<Pathing_Attribute<<endl;
        //read the verts

        vector<CVector> Verts;
        for (int v=0; v<NumVerts; v++)
        {
            CVector Vertex;

            infile.read((char *)&Vertex.x,4);
            infile.read((char *)&Vertex.y,4);
            infile.read((char *)&Vertex.z,4);

            Verts.push_back(Vertex);
            if (debug) cout<<"V : "<<Vertex<<endl;
        }

        //read the portals
        vector<PortalInfo> PortalInfo_List;
        for (int p=0; p<NumPortals; p++)
        {
            PortalInfo PInfo;

            infile.read((char *)&PInfo.P1.x,4);
            infile.read((char *)&PInfo.P1.y,4);
            infile.read((char *)&PInfo.P1.z,4);

            infile.read((char *)&PInfo.P2.x,4);
            infile.read((char *)&PInfo.P2.y,4);
            infile.read((char *)&PInfo.P2.z,4);
            infile.read((char *)&PInfo.DestinationIndex,4);
            infile.read((char *)&PInfo.Up.x,4);
            infile.read((char *)&PInfo.Up.y,4);
            infile.read((char *)&PInfo.Up.z,4);

            PInfo.color = GREEN;

            PortalInfo_List.push_back(PInfo);

            if (debug) cout<<"Portal : "<<PInfo.P1<<" "<<PInfo.P2<<" Dest : "<<PInfo.DestinationIndex<<endl;
        }

        //create the region
        CRegion * ptRegion = new CRegion(Verts,PortalInfo_List, Pathing_Attribute);
        Region_list.push_back(ptRegion);

    }//end for each region

    //load the P array
    P.clear();
    P.resize(NumRegions,vector<int>(NumRegions));

    for (int row=0; row<NumRegions; row++)
        for (int col=0; col<NumRegions; col++)
        {
            int value;
            if (debug)
                if (value < 0 || value >= NumRegions)
                    cout<<"Weird Pathing Value : "<<value<<endl;

            infile.read((char *)&value,4);
            P[row][col] = value;
        }

    D.clear();
    D.resize(NumRegions,vector<float>(NumRegions));

    for (int row=0; row<NumRegions; row++)
        for (int col=0; col<NumRegions; col++)
        {
            float value;
            infile.read((char *)&value,4);
            D[row][col] = value;
        }

    infile.close();

    return true;
}//end LoadRegionListNAV
