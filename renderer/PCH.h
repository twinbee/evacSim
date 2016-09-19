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

#ifndef usm_pch
#define usm_pch

#include <queue>
#include "adjlist.h"

struct Report
{
    int src;
    int dest;
    int time;
    Report(int s, int d, int t) : src(s), dest(d), time(t){}
};

inline bool operator < (const Report& t1, const Report& t2)
{
    return t1.time < t2.time;
}

class CPortalClearingHouse
{
public :
    CPortalClearingHouse(float * pAdjArray, int dim);
    ~CPortalClearingHouse();
    void ReportClosure(int src, int dst, int time);
    bool PortalClosed(int src, int dst);
    bool PathClosed(std::vector<int> & Path);
    void Purge(int time);
     std::vector<int> FindNearest(int src,  std::vector<int> & targets);
    bool GetClosureReport(Report & rep);

    static const float minfrustration = 50.0;
    static const int threshhold = 6;
    static const int ttl = 15; //message time to live

private :
    int * AdjArray;
    int dim;
    std::priority_queue <Report> ReportQ;
    CAdjlist * Adj_List;
     std::vector<Report> Closures;



};

#endif
