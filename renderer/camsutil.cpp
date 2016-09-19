/**********************************************************
** Camsutil.cpp
** Primary Author: Matthew B
** Description: Functions for the support of loading and storing camera position dumps
** Used in sim: rendering videos
*********************************************************/

#include "camsutil.h"

#include <fstream>

using namespace std;

vector< CCamera > g_Camera_list;

int dirtyCams[maxcams] = {0};


///////////////////////////////////////////////////////////////////////////////

bool storcams(std::string filename)
{
    ofstream file(filename.c_str());

    for (int i = 0; i < g_Camera_list.size(); i++)
    {
        file << g_Camera_list[i];
    }

    return true;
}

bool rtrvcams(std::string filename)
{

    fstream file(filename.c_str());

    g_Camera_list.clear();

    CCamera temp;

    file >> temp;
//cout << temp << flush;

    while (file)
    {
        g_Camera_list.push_back(temp);
        file >> temp;
    }

    return true;
}

void switchcam(int n)
{
    if (n < g_Camera_list.size())  g_camera = g_Camera_list[n];
}
