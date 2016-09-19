/*********************************************************
** MST.cpp, MST.h
** Primary author: Mike J
** Description: algorithms directly related to graph manipulation
**   minimum spanning tree
** Used in sim: not currently used
*********************************************************/

#include "MST.h"
#include "adjlist.h"

#include <algorithm>
#include <queue>

using namespace std;

struct MST_Node
{
    int ID;
    float dist;
};


inline bool operator < (const MST_Node& t1, const MST_Node& t2)
//op overload needed for comparision to create a priority queue
{
    return t1.dist < t2.dist;
}

vector<int> MST(float * AdjArray, int dim, int source)
//creates a Minimum spanning tree
{

    vector<float> min_distance(dim);
    vector<int> parent(dim);
    vector<bool> inQ(dim);
    vector< vector<int> > min_adj_list(dim);
    vector < MST_Node > Q;

    //initialize the data
    for (int x=0; x<dim; x++)
    {
        min_distance[x] = 1000000;         //set initial min distances to undefined
        parent[x] = -1;               //set initial parent to undefined
        inQ[x] = true;
        MST_Node node;
        node.ID = x;
        if (x == source)
            node.dist = 0;
        else
            node.dist = 1000000;

        Q.push_back(node);
    }

    CAdjlist Adjlist(AdjArray,dim);

    while (Q.size())
    {
        sort(Q.begin(),Q.end());
        MST_Node minnode = Q[0];
        Q.erase(Q.begin());
        inQ[minnode.ID] = false;

        int parentID = parent[minnode.ID];
        if (parentID != -1)
        {
            min_adj_list[parentID].push_back(minnode.ID);
            min_adj_list[minnode.ID].push_back(parentID);
        }

        for (list<edge>::iterator adj= Adjlist.Adj_list[minnode.ID].begin(); adj != Adjlist.Adj_list[minnode.ID].end(); adj++)
        {
            int adjID = adj->ID;
            float adjweight = adj->weight;

            if (inQ[adjID] && (adjweight < min_distance[adjID]))
            {
                parent[adjID] = minnode.ID;
                min_distance[adjID] = adjweight;

                //update adjacent in Q, order by min_distance
                for (int b=0; b<Q.size(); b++)
                {
                    if (Q[b].ID == adjID)
                    {
                        Q[b].dist = adjweight;
                    }
                }//end for Q
            }

        }//end for

    }//end while Q.size

    return parent;
}//end MST
