/**********************************************************
** Camsutil.h
** Primary Author: Matthew B
** Description: Functions for the support of loading and storing camera position dumps
** Used in sim: rendering videos
*********************************************************/

#ifndef CAMSUTIL_H
#define CAMSUTIL_H

#include <string>
#include <vector>

#include "camera.h"

extern std::vector< CCamera > g_Camera_list;

extern CCamera g_camera;

const int maxcams = 1000;
extern int dirtyCams[maxcams];

///////////////////////////////////////////////////////////////////////////////

bool storcams(std::string filename);
bool rtrvcams(std::string filename);

void switchcam(int n);
#endif
