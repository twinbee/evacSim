// convex path reduction table
// Mike johnson

#ifndef PRT_H
#define PRT_H
        
#include "region.h"
#include <vector>
using namespace std;


class CPRT
{
 public:
        CPRT( vector <CRegion * > & Region_list);
        ~CPRT();
        CPortal * GetTarget(vector< int > & path);
        
 private:      
      char * PRT;
      int size;   //number of regions, size * size = array dimensions
};

#endif
