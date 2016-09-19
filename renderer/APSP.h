/*********************************************************
** APSP.cpp, APSP.h
** Primary author: Mike J, Matthew B
** Description: algorithms directly related to graph manipulation
**   including graph search, Floyd-Warshall All-Pairs Shortest-Paths,
**   reward/penalty edge-weighting based on steepness changes,
**   weighting based on region-type, and minimum spanning tree
** Used in sim: region-to-region and pathing decision support
*********************************************************/

#ifndef APSP_H
#define APSP_H

#include <vector>
#include "region.h"
#include "adjlist.h"

void FloydAPSP (int N,	float* C, std::vector<std::vector<float> > &D, std::vector<std::vector<int> > &P);
void GetPath(int u, int v, std::vector < std::vector <int> > &P, std::vector<int> & Path);
void GetPathInclusive(int u, int v, std::vector < std::vector <int> > &P, std::vector<int> & Path);

float * ComputeAdjacencyArray(std::vector<CRegion *> & Region_list);
void PrintAdjacencyList(float * array, int size);

#endif
