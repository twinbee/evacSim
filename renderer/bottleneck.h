
#ifndef BOTTLENECK_H
#define BOTTLENECK_H

#include "region.h"
using namespace std;

struct t_Bottleneck
{
    int src;
    int dst;
    t_Bottleneck(int s = 0, int d = 0) : src(s), dst(d){}
};

void SelectBottlenecks(vector< CRegion *> & Region_list, vector< t_Bottleneck > & Bottleneck_list);

#endif

