/*********************************************************
** texcoord.h, texcoord.cpp
** Primary author: Mike E
** Description: texture coordinates (u,v)
** Used in sim: detail map
*********************************************************/

#include "TexCoord.h"
#include "graphics.h"

void TexCoord::set() const
{
    glTexCoord2f( s, t );
}
