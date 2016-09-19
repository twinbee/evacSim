/*********************************************************
** ADJLIST.H / ADJLIST.CPP
** Primary author: Mike J
** Description: Adjacency list representation of a graph
** Used in sim: to represent region graph
*********************************************************/

#ifndef ADJ_LIST_H
#define ADJ_LIST_H

#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <set>
#include <queue>
#include <algorithm>
#include <climits>

enum {ASTAR_OPEN,ASTAR_CLOSED,ASTAR_NULL};

struct edge
{
    int ID;
    float weight;
    bool closed;
    edge(int arg_ID, float arg_weight) : ID(arg_ID), weight(arg_weight) {
        closed = false;
    }
};

struct Open_List_Entry
{
    int ID;
    float F;
    Open_List_Entry(int arg_id, float arg_f) : ID(arg_id), F(arg_f) { }
};

struct Astar_Node
{
    int parent;
    int status;
    float G;
};

class CompareOpenListEntry
{
public :
    int operator()( const Open_List_Entry &a, const Open_List_Entry &b )
    {
        return a.F > b.F;
    }
};

class CompareEdgePointer
{
public :
    int operator()( const edge * a, const edge * b )
    {
        return a->weight > b->weight;
    }
};

class CAdjlist
{
public:

    CAdjlist(float * AdjMatrix, int n);
    ~CAdjlist();

    void Print();
    std::vector<int> FindNearest(int src, std::vector<int> & targets);
    std::vector<int> FindPath(int src, int target, std::vector< std::vector<float> > & Heuristic);
    bool CloseLink(int src, int dst);
    bool OpenLink(int src, int dst);

    std::vector< std::list< edge > > Adj_list;

};


#endif
