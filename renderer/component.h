
#ifndef COMPONENT_H
#define COMPONENT_H

#include <vector>
#include "region.h"
#include "bottleneck.h"

using namespace std;



class CComponent
{
public:
    CComponent();
    
    int ID;
    bool hasexit;
    vector<t_Bottleneck> Bottlenecks;
    vector<int> Adj_Component;
    CColor color;
    int numregions;
};

void FloodComponents(vector< CRegion *> & Region_list, vector< CComponent > & Component_list);
bool FloodRecursive(vector< CRegion *> & Region_list,int src, vector<t_Bottleneck> & Adj, int ID);

#endif
