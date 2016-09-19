
#include "bottleneck.h"


bool RegisterBottleneck(vector <t_Bottleneck > & Bottleneck_list, int src, int dst)
{
    bool found = false;
    for (int w=0; w<Bottleneck_list.size(); w++)
    {
        if ((Bottleneck_list[w].src == src) && (Bottleneck_list[w].dst == dst))
        {
            found = true;
            break;
        }
    }
    if (!found)
    {
        t_Bottleneck bneck;
        bneck.src = src;
        bneck.dst = dst;
        Bottleneck_list.push_back(bneck);
    }
    
    return found;
}

void SelectBottlenecks(vector< CRegion *> & Region_list, vector< t_Bottleneck > & Bottleneck_list)
{
    static float bottlenecksize = 10;
    //static float bottlenecksize = Entity::LARGEST_ENTITY_RADIUS * 2*8;

    for (int r=0; r<Region_list.size(); r++)
    {
        CRegion * Region = Region_list[r];

        for (int p=0; p < Region->Portals.size(); p++)
        {
            int adj = Region->Portals[p]->GetDest();
            CRegion * AdjRegion = Region_list[adj];


            if ((AdjRegion->Portals.size() == 2) && (Region->Portals.size() >= 3))
            {

                if ( Region->GetPortalToRegion(adj)->GetWidth() < bottlenecksize)
                {
                    Region->HighlightPortal(adj);
                    AdjRegion->HighlightPortal(r);
                    RegisterBottleneck(Bottleneck_list,adj,r);
                    RegisterBottleneck(Bottleneck_list,r,adj);
                    AdjRegion->GetPortalToRegion(r)->bottleneck = true;
                    Region->GetPortalToRegion(adj)->bottleneck = true;
                }
            }
            else if ((AdjRegion->Portals.size() >= 3) && (Region->Portals.size() == 2))
            {
                if ( AdjRegion->GetPortalToRegion(r)->GetWidth() < bottlenecksize)
                {
                    AdjRegion->HighlightPortal(r);
                    Region->HighlightPortal(adj);
                    RegisterBottleneck(Bottleneck_list,adj,r);
                    RegisterBottleneck(Bottleneck_list,r,adj);
                    AdjRegion->GetPortalToRegion(r)->bottleneck = true;
                    Region->GetPortalToRegion(adj)->bottleneck = true;
                }
            }

        }
    }//end for each region
}
