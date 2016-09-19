/*********************************************************
** new_color.h, new_color.cpp
** Primary author: Mike E
** Description: color class with hue constraint
** Used in sim:
*********************************************************/

#ifndef usm_color
#define usm_color

#include "color.h"

// Mike Erwin
// 29 Sept 2007
// transplanted from Humane Assembly Language Tools

struct Color
{
    float red, green, blue;
    float alpha;

    Color(); // boring default
    Color( float hue );
    Color( float r, float g, float b, float a = 1.0 );

    void set() const; // use for subsequent drawing

    float value() const;
    Color scaleValue( float v ) const;
    Color withAlpha( float a ) const;
};

Color convert( const CColor& );
CColor convert( const Color& );

#endif
