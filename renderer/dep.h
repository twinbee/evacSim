/**********************************************************
** dep.h
** Primary Author: Matthew B
** Description: deprecation handling
** Used in sim:
*********************************************************/


#ifndef DEP_H
#define DEP_H

#include <iostream>

inline void deprecated(char * a)
{
    std::cerr << "!!! " << a << " is deprecated, but still in use\n";
    return;
}

#endif
