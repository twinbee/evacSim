/*********************************************************
** APSP.cpp, APSP.h
** Primary author: Mike J, Matthew B
** Description: algorithms directly related to graph manipulation
**   including graph search, Floyd-Warshall All-Pairs Shortest-Paths,
**   reward/penalty edge-weighting based on steepness changes,
**   weighting based on region-type, and minimum spanning tree
** Used in sim: region-to-region and pathing decision support
*********************************************************/

#include <queue>
#include <algorithm>

#include "convert.h"

#include "APSP.h"

using namespace std;

void FloydAPSP (int N, float* C, std::vector<std::vector<float> > &D, std::vector<std::vector<int> > &P)
{

    int i,j,k;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            if (C[i*N + j] && i != j) D[i][j] = C[i*N + j];
            else D[i][j] = (std::numeric_limits < float >::max)();
            P[i][j] = -1;
        }

        D[i][i] = 0.0;
    }

    for (k = 0; k < N; k++)
    {
        for (i = 0; i < N; i++)
        {
            for (j = 0; j < N; j++)
            {
                if (D[i][k] + D[k][j] < D[i][j])
                {
                    D[i][j] = D[i][k] + D[k][j];
                    P[i][j] = k;
                }
            }
        }
    }
} /* FloydAPSP */

void GetPath (int u, int v, std::vector < std::vector <int> > &P, std::vector<int> & Path)
{

    int k = P[u][v];
    //cout<<"K : "<<k<<endl;
    if (k == -1)
    {
        return;
    }
    GetPath(u,k,P,Path);
    Path.push_back(k);
    GetPath(k,v,P,Path);

} /* Path */



void GetPathInclusive(int u, int v, std::vector < std::vector <int> > &P, std::vector<int> & Path)
{
    Path.push_back(u);
    GetPath(u,v,P,Path);
    Path.push_back(v);
}


float InclineWeight(CVector &direction, CRegion * Region)
{
    //calculates a one-way "penalty" as function of slope of a region

    if (direction.z <= 0)
    {
        //going down
        return Region->GetIncline();
        //cout<<"Going Down : "<<inclineweight<<endl;
    }

    //going up
    return (2 - Region->GetIncline()) * 2;
    //cout<<"Going up : "<<inclineweight<<endl;
}

/*
float DifficultyWeight(CRegion * Region)
{
    //set difficulty
            float Difficulty = 1;
            if(Region->GetPathingDifficulty() == PATHING_STAIRS)
                  Difficulty = 0.8;
            else if(Region->GetPathingDifficulty() == PATHING_SEATING)
                  Difficulty = 2;
        else if(Region->GetPathingDifficulty() == PATHING_WAYPOINT)
                  Difficulty = 1;

    return Difficulty;
}
*/

float * ComputeAdjacencyArray(vector<CRegion *> & Region_list)
{
    bool debug = false;

    if (debug)cout<<"ComputeRawDistAdjacencyArray"<<endl;

    int size = Region_list.size();

    if (debug) cout << "Size : " << size << endl;

    //allocate the array and zero it out
    float * adjacency = new float[size * size];
    for (int i=0; i < (size * size); i++) adjacency[i] = 0.0;

    //for every region
    for (int p=0; p < size; p++)
    {
        //for every portal in this region
        for (int portal = 0; portal < Region_list[p]->Portals.size(); portal++)
        {
            int DestinationIndex = Region_list[p]->Portals[portal]->GetDest();
            if ((DestinationIndex >= size) || (DestinationIndex < 0))
            {
                cout<<"invalid portal destination"<<endl;
                return adjacency;
            }

            //if this portal is too small to be passed through, close it off to the AI
            if (Region_list[p]->Portals[portal]->GetWidth() < (Entity::LARGEST_ENTITY_RADIUS * 2.2))
            {
                adjacency[p * size + DestinationIndex] = 0.0;
                continue;
            }

            //compute the two segments of the connection vector
            CVector start = Region_list[p]->Polygon->GetCentroid();
            CVector end = Region_list[DestinationIndex]->Polygon->GetCentroid();
            CVector PortalMidpoint = (Region_list[p]->Portals[portal]->GetPoint1() + Region_list[p]->Portals[portal]->GetPoint2()) / 2;
            CVector segment1 = PortalMidpoint - start;
            CVector segment2 = end - PortalMidpoint;

            //compute the incline weight for the two parts of this connection
            float inclineweight1 = InclineWeight(segment1,Region_list[p]);
            float inclineweight2 = InclineWeight(segment2,Region_list[DestinationIndex]);

            if (debug) cout << "Region Index : " << p << "\tDestinationIndex : " << DestinationIndex << endl;

            CRegion * Destination = Region_list[DestinationIndex];
            CRegion * Source = Region_list[p];

            float Distance = (segment1.Length() * inclineweight1) + (segment2.Length() * inclineweight2);

            if (Distance >= 0) //protect against negative weights
                adjacency[p * size + DestinationIndex] = Distance;
            else
                adjacency[p * size + DestinationIndex] = 0.0;
        }
    }

    if (debug) cout<<"End ADJACENCY ARRAY STUFF"<<endl;
    return adjacency;
}


void PrintAdjacencyList(float * array, int size)
{

    cout<<"PrintAdjacencyList"<<endl<<endl;
    for (int index = 0; index < size; index ++)
    {
        cout<< index <<" : ";
        for (int index2 = 0; index2<size; index2++)
        {
            if ( array[index * size + index2] != 0)
                cout<<index2<<" (" << array[index * size + index2] << ") " ;
        }
        cout<<endl;
    }

}

