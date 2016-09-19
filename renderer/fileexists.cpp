
#include "fileexists.h"

#include <fstream>

bool textFileExists( std::string filename )
{
    return (bool) std::ifstream( const_cast<char *> (filename.c_str()) );
}
