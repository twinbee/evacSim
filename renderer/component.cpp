
#include "component.h"


CComponent::CComponent()
{
        ID = -1;
        hasexit = false;
        numregions = 0;
}



void FloodComponents(vector< CRegion *> & Region_list, vector< CComponent > & Component_list)
{
    const bool debug = false;

    if(Component_list.size()) Component_list.clear();

    int cID = 0;
    for (int x=0; x<Region_list.size(); x++)
    {
        vector<t_Bottleneck> Bottlenecks;
        CColor color = ALLCOLORS[cID % NUM_ALLCOLORS];
        if (FloodRecursive(Region_list,x,Bottlenecks,cID))
        {
            CComponent component;
            component.Bottlenecks = Bottlenecks;
            component.ID = cID;
            component.color = color;
            Component_list.push_back(component);
            Bottlenecks.clear();
            cID++;
        }
    }

    //replace the region IDs with Component IDs
    for (int c=0; c< Component_list.size(); c++)
    {
        Component_list[c].Adj_Component.resize(Component_list[c].Bottlenecks.size());
        for (int a=0; a<Component_list[c].Bottlenecks.size(); a++)
            Component_list[c].Adj_Component[a] = Region_list[Component_list[c].Bottlenecks[a].dst]->ComponentID;
    }


    //compute the number of regions for each component
    //and mark components that contain exits
    for (int r=0; r<Region_list.size(); r++)
    {
        Component_list[Region_list[r]->ComponentID].numregions++;
        if(Region_list[r]->GetPathingDifficulty() == PATHING_EXIT)
            Component_list[Region_list[r]->ComponentID].hasexit = true;            
    }
}



//returns true if a new component was flooded.
bool FloodRecursive(vector< CRegion * > & Region_list,int src, vector<t_Bottleneck> & Adj, int ID)
{
    const bool debug = false;
    CRegion * Region = Region_list[src];

    if (Region->ComponentID != -1) return false;

    Region->ComponentID = ID;

    for (int p=0; p < Region->Portals.size(); p++)
    {

        int adj = Region->Portals[p]->GetDest();
        CRegion * AdjRegion = Region_list[adj];

        if (Region->Portals[p]->bottleneck)
        {
            t_Bottleneck bneck(src,adj);
            Adj.push_back(bneck);
            continue;
        }

        if (AdjRegion->ComponentID != -1) continue;

        FloodRecursive(Region_list,adj,Adj,ID);

    }//end for

    return true;
}




