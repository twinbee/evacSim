/*********************************************************
** new_color.h, new_color.cpp
** Primary author: Mike E
** Description: color class with hue constraint
** Used in sim:
*********************************************************/


#include "new_Color.h"
#include <GL/gl.h>
#include <cmath>

Color::Color()
        : red(0.5), green(0.5), blue(0.5), alpha(1.0)
{ }

Color::Color( float r, float g, float b, float a )
        : red(r), green(g), blue(b), alpha(a)
{ }

Color::Color( float hue )
        :   red( cos(( hue - 0.000 ) * 2 * M_PI ) / 2 + 0.5 ),
        green( cos(( hue - 0.333 ) * 2 * M_PI ) / 2 + 0.5 ),
        blue( cos(( hue - 0.667 ) * 2 * M_PI ) / 2 + 0.5 ),
        alpha( 1.0 )
{ }

void Color::set() const
{
    glColor4f( red, green, blue, alpha );
}

float Color::value() const
{
    return ( red + green + blue ) / 3;
}

Color Color::scaleValue( float v ) const
{
    return Color( red * v, green * v, blue * v, alpha );
}

Color Color::withAlpha( float a ) const
{
    return Color( red, green, blue, a );
}

Color convert( const CColor& c )
{
    return Color( c.r(), c.g(), c.b(), c.a());
}

CColor convert( const Color& c )
{
    return CColor( c.red, c.green, c.blue, c.alpha );
}
