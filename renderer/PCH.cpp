/******************************************************************************
** PCH.cpp, PCH.h
** Primary author: Mike J, Matthew B
** Description: portal clearing house
** Used in sim: represents information about how many entities are trying to get
**   to a particular portal location and failing. Messages of entity frustration
**   are reported to a PCH, and they eventually time out. The PCH decides if a
**   portal is considered to conjested to be closed by the number of entities
**   reports it receives for a given portal in a given amount of time.
******************************************************************************/

#include "PCH.h"
#include <iostream>
using namespace std;

CPortalClearingHouse::CPortalClearingHouse(float * pAdjArray, int pdim)
{

    dim = pdim;
    AdjArray = new int[dim * dim];

    for (int x=0; x< dim * dim; x++)
    {
        if (pAdjArray[x])
            AdjArray[x] = 0;
        else
            AdjArray[x] = -1;
    }

    Adj_List = new CAdjlist(pAdjArray,pdim);

}

CPortalClearingHouse::~CPortalClearingHouse()
{
    if (AdjArray) delete [] AdjArray;
    if (Adj_List) delete Adj_List;
}

void CPortalClearingHouse::ReportClosure(int src, int dst, int time)
{

    if ((src < 0) || (src >= dim)) return;
    if ((dst < 0) || (dst >= dim)) return;
    Report R(src,dst,time);
    ReportQ.push(R);

    AdjArray[src * dim + dst] ++;

    if (AdjArray[src * dim + dst] == (CPortalClearingHouse::threshhold + 1))
    {
        Adj_List->CloseLink(src,dst);
        Adj_List->CloseLink(dst,src);
        Closures.push_back(R);
    }
    //cout<<"GOT A REPORT at "<<src<<" "<<dst<<" "<<AdjArray[src * dim + dst]<<endl;
}

bool CPortalClearingHouse::PortalClosed(int src, int dst)
{
    if (AdjArray[src * dim + dst] > CPortalClearingHouse::threshhold)
    {
        // cout<<"CLOSED BITCH! "<<src<<" "<<dst<<" "<<AdjArray[src * dim + dst]<<endl;
        return true;
    }
    return false;
}

bool CPortalClearingHouse::PathClosed(std::vector<int> & Path)
{
    for (int i=0; i< (Path.size() - 1); i++)
        if (PortalClosed(Path[i],Path[i+1])) return true;
    return false;
}

void CPortalClearingHouse::Purge(int time)
{

//purge the expired records
    while (ReportQ.size())
    {
        Report oldest = ReportQ.top();
        if (time - oldest.time > CPortalClearingHouse::ttl)
        {
            AdjArray[oldest.src * dim + oldest.dest] -= 1; //message popped
            ReportQ.pop();

            if (!PortalClosed(oldest.src,oldest.dest))
            {
                Adj_List->OpenLink(oldest.src,oldest.dest);
                Adj_List->OpenLink(oldest.dest,oldest.src);
            }
        }
        else break; //quit the while loop since this is the oldest anyways
    }
}

vector<int> CPortalClearingHouse::FindNearest(int src, vector<int> & targets)
{
    return Adj_List->FindNearest(src,targets);
}

bool CPortalClearingHouse::GetClosureReport(Report & rep)
{
    if (!Closures.size()) return false;

    rep = Closures.back();
    Closures.pop_back();
    return true;
}
