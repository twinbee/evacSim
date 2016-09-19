// convex path reduction table

#include "PRT.h"


//2 bits for the typecode, 6 bits for the portal located on the dst region


CPRT::CPRT( vector <CRegion * > & Region_list)
{
         int size = Region_list.size();
         PRT = new char[size * size];  //3 shouldn't be static I don't believe          
}

CPRT::~CPRT()
{
             if(size) delete [] PRT;
             
}

CPortal * GetTarget(vector< int > & path)
{
        
        
}
