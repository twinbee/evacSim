/*********************************************************
** texcoord.h, texcoord.cpp
** Primary author: Mike E
** Description: texture coordinates (u,v)
** Used in sim: detail map
*********************************************************/

#ifndef usm_tex_coord
#define usm_tex_coord

struct TexCoord
{
    float s, t;
    void set() const;
};

#endif
