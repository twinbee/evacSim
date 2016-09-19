/*********************************************************
** ADJLIST.H / ADJLIST.CPP
** Primary author: Mike J
** Description: Adjacency list representation of a graph
** Used in sim: to represent region graph
*********************************************************/

#include "adjlist.h"

using namespace std;

bool operator<( const Open_List_Entry &a, const Open_List_Entry &b )
{
    return a.F < b.F;
}

bool operator<=( const Open_List_Entry &a, const Open_List_Entry &b )
{
    return a.F <= b.F;
}

CAdjlist::CAdjlist(float * AdjMatrix, int n)
{
    Adj_list.resize(n);
    for (int row=0; row < n; row++)
        for (int col=0; col < n; col++)
        {
            if (AdjMatrix[row * n + col] > 0.0)
            {
                edge temp(col,AdjMatrix[row * n + col]);
                Adj_list[row].push_back(temp);
            }

        }

}

CAdjlist::~CAdjlist()
{


}

void CAdjlist::Print()
{
    for (int src=0; src<Adj_list.size(); src++)
    {
        cout<<src<<" : ";
        for (list<edge>::iterator b = Adj_list[src].begin(); b != Adj_list[src].end(); b++)
        {
            cout<<"["<<b->ID<<","<<b->weight<<"] ";
        }
        cout<<endl;
    }

}

/*
float CAdjlist::length(int src, int dest)
{
    for(list<edge>::iterator b = Adj_list[src].begin(); b != Adj_list[src].end(); b++)
    {
        if(b->ID == dest) return b->weight;
    }
    return 0.0;
}
*/

vector<int> CAdjlist::FindNearest(int src, vector<int> &targets)
// similar to djikstra's algorithm
{
    bool debug = false;

    if (debug) cout<<"here 1"<<endl;

    vector<int> path;
    vector<edge> Dijkstra_Q;

    int * previous = new int[Adj_list.size()];
    float * dist = new float[Adj_list.size()];
    bool * popped = new bool[Adj_list.size()];
    int numpopped = 0;

    //setup all the data structures
    for (int i=0; i<Adj_list.size(); i++)
    {
        if (i == src)
            dist[i] = 0.0;
        else
            dist[i] = LONG_MAX;
        popped[i] = false;
        previous[i] = -1;
    }
    if (debug) cout<<"here 2"<<endl;

    while (numpopped < Adj_list.size() )
    {

        //find the smallest
        float u_weight = LONG_MAX;
        int u = -1;
        for (int x=0; x<Adj_list.size(); x++)
        {
            if (!popped[x])
                if (dist[x] < u_weight)
                {
                    u = x;
                    u_weight = dist[x];
                }

        }
        if (u == -1)
        {
            //free all the data structures
            delete [] popped;
            delete [] previous;
            delete [] dist;
            return path;
        }
        numpopped++;
        popped[u] = true;

        if (debug) cout<<"here 2.1"<<endl;

        for (list<edge>::iterator adj= Adj_list[u].begin(); adj != Adj_list[u].end(); adj++)
        {
            //cout<<"u ="<<u<<endl;
            if (adj->closed) continue;

            //cout<<"Adj :"<<adj->ID<<"Current weight : "<<dist[u]<<" proposed weight : "<<dist[u] + adj->weight;
            int v = adj->ID;
            //cout<<"Adj ID = "<<adj->ID<<endl;

            float dist_u_to_v = adj->weight;

            float alt = dist[u] + dist_u_to_v;

            if (alt < dist[v])
            {
                dist[v] = alt;
                previous[v] = u;
            }

            //cout<<" ,New Distance from source :"<<dist[v]<<endl;
        }

        if (debug) cout<<"here 2.2"<<endl;

    }//end while

    if (debug) cout<<"here 3"<<endl;

    //cout<<"results: "<<endl;
    //for(int d=0; d<Adj_list.size(); d++)
    // cout<<"["<<d<<", "<<dist[d]<<"] ";

    //find which computed path is shortest
    int nearest_target = targets[0];
    for (int t=1; t<targets.size(); t++)
    {
        if (dist[targets[t]] < dist[nearest_target])
        {
            nearest_target = targets[t];
        }

    }//end for targets

    if (debug) cout<<"here 4"<<endl;
    //create the path array
    int next_in_path = nearest_target;
    while (next_in_path != -1)
    {
        //path.push_back(next_in_path);
        path.insert(path.begin(),next_in_path);
        next_in_path = previous[next_in_path];
    }
    if (debug) cout<<"here 5"<<endl;
    //free all the data structures
    delete [] popped;
    delete [] previous;
    delete [] dist;

    if (debug) cout<<"here 6"<<endl;
    return path;
}

vector<int> CAdjlist::FindPath(int src, int target, vector< vector <float> > & Heuristic)
// similar to A*Star algorithm
{
    bool debug = false;

    if (debug) cout<<"Astar source : "<<src<<" target : "<<target<<endl;

    vector<Open_List_Entry> Open_List;
    Astar_Node * Node_List = new Astar_Node[Adj_list.size()];
    if (debug) cout<<Adj_list.size()<<" Nodes in graph "<<endl;
    vector<int> Path;

    if (src < 0 || src > Heuristic.size()) return Path;
    if ( target < 0 || target > Heuristic.size()) return Path;

    //set all the Astar nodes to default settings
    for (int n=0; n<Adj_list.size(); n++)
    {
        Node_List[n].parent = -1;
        Node_List[n].G = 0;
        Node_List[n].status = ASTAR_NULL;
    }

    //add the source node
    Open_List_Entry source(src,0.0);  //ID, F(G + H)
    Open_List.push_back(source);

    while (Open_List.size())
    {
        //find the smallest
        float u_F = LONG_MAX;
        int u = -1;
        int uindex = 0;

        for (int x=0; x<Open_List.size(); x++)
        {

            if (Open_List[x].F < u_F)
            {
                u = Open_List[x].ID;
                u_F = Open_List[x].F;
                uindex = x;
            }

        }
        if (debug) cout<<endl<<"Popped "<<u<<" from open list and CLOSED it!"<<endl;
        Open_List.erase(Open_List.begin() + uindex);
        Node_List[u].status = ASTAR_CLOSED;

        //if(debug)
//    {   cout<<"Open List : ";
//    for(int c=0; c<Open_List.size();c++)
//     cout<<Open_List
//    }

        if (u == target) break;

        if (debug) cout<<"Number adj : "<<Adj_list[u].size()<<endl;
        //for each adjacent node to the current node
        for (list<edge>::iterator adj= Adj_list[u].begin(); adj != Adj_list[u].end(); adj++)
        {
            if (debug) cout<<" considering adj : "<<adj->ID;
            if (adj->closed)
            {
                continue;
            }

            float H = Heuristic[adj->ID ][target];

            if (Node_List[adj->ID].status == ASTAR_CLOSED)
            {
                if (debug) cout<<" node closed"<<endl;
                continue;
            }
            else if (Node_List[adj->ID].status == ASTAR_NULL)
            {
                //add it to the open list
                float G = Node_List[u].G + adj->weight;
                float F = H + G;

                Open_List_Entry AdjNode(adj->ID,F);
                Open_List.push_back(AdjNode);
                if (debug) cout<<" Pushed "<<adj->ID<<" Onto Open List"<<endl;

                Node_List[adj->ID].parent = u;
                Node_List[adj->ID].G = G;
                Node_List[adj->ID].status = ASTAR_OPEN;

            }
            else if (Node_List[adj->ID].status == ASTAR_OPEN)
            {
                if (debug) cout<<" node open, updated"<<endl;
                //if it's on the open list, check to see if u->v is a better path;
                float G = Node_List[u].G + adj->weight;
                if (G < Node_List[adj->ID].G)
                {
                    Node_List[adj->ID].G = G;
                    for (int o=0; o<Open_List.size(); o++)
                        if (Open_List[o].ID == adj->ID)
                            Open_List[o].F = G + H;

                }

            }
        }//end for adj

    }//end while

    //create the path array
    //if(Open_List.size())
    //{
    int next_in_path = target;
    while (next_in_path != -1)
    {
        //path.push_back(next_in_path);
        Path.insert(Path.begin(),next_in_path);
        if (debug) cout<<"Added "<<next_in_path<<" to Path"<<endl;
        next_in_path = Node_List[next_in_path].parent;
    }
    // }

    //delete temp data;
    delete [] Node_List;

    return Path;
}

bool CAdjlist::CloseLink(int src, int dst)
{
    if ((src < 0) || (src >= Adj_list.size())) return false;
    if ((dst < 0) || (dst >= Adj_list.size())) return false;

    //for each adjacent node to the current node
    for (list<edge>::iterator adj= Adj_list[src].begin(); adj != Adj_list[src].end(); adj++)
    {
        if (adj->ID == dst)
        {
            adj->closed = true;
            return true;
        }
    }

    return false;
}

bool CAdjlist::OpenLink(int src, int dst)
{
    if ((src < 0) || (src >= Adj_list.size())) return false;
    if ((dst < 0) || (dst >= Adj_list.size())) return false;

    //for each adjacent node to the current node
    for (list<edge>::iterator adj= Adj_list[src].begin(); adj != Adj_list[src].end(); adj++)
    {
        if (adj->ID == dst)
        {
            adj->closed = false;
            return true;
        }
    }

    return false;
}

