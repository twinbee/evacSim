/*********************************************************
** vec3.cpp, vec3.h
** Primary author: Mike E
** Description: 3-vector
** Used in sim:
*********************************************************/

//TODO: use the vector.cpp Cvector class

#include "Vec3.h"
#include <cmath>

using namespace std;

// This version of the Vec3 class is written for clarity,
// not performance. Just so you know.

// [update] actually, gcc emits some great code from this!

const Vec3 Vec3::zero;
const float epsilon = 0.0001f;


Vec3::Vec3()
        : x( 0.f ), y( 0.f ), z( 0.f )
{ }

Vec3::Vec3( float arg_x, float arg_y, float arg_z )
        : x( arg_x ), y( arg_y ), z( arg_z )
{ }

float Vec3::magnitude() const
{
    return sqrt( mag2());
}

float Vec3::mag2() const
{
    return x*x + y*y + z*z;
}

Vec3 Vec3::normalized() const
{
    return 1.f / magnitude() * *this;
}

void Vec3::normalize()
{
    *this = normalized();
}

void Vec3::operator += ( const Vec3& u )
{
    x += u.x;
    y += u.y;
    z += u.z;
}

bool operator == ( const Vec3& u, const Vec3& v )
// combined
{
    return (u-v).mag2() < epsilon * epsilon;
}

// axis by axis
//  { return fabsf( u.x - v.x ) < epsilon
//    and fabsf( u.y - v.y ) < epsilon
//    and fabsf( u.z - v.z ) < epsilon; }

bool operator != ( const Vec3& u, const Vec3& v )
{
    return not ( u == v );
}

Vec3 operator + ( const Vec3& u, const Vec3& v )
{
    return Vec3( u.x + v.x, u.y + v.y, u.z + v.z );
}

Vec3 operator - ( const Vec3& u, const Vec3& v )
{
    return Vec3( u.x - v.x, u.y - v.y, u.z - v.z );
}

Vec3 operator * ( float s, const Vec3& u )
{
    return Vec3( s * u.x, s * u.y, s * u.z );
}

Vec3 operator - ( const Vec3& u )
{
    return Vec3( -u.x, -u.y, -u.z );
}

float dot( const Vec3& u, const Vec3& v )
{
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

Vec3 proj( const Vec3& u, const Vec3& v )
{
    return dot( u, v ) / dot( v, v ) * v;
}

Vec3 cross( const Vec3& u, const Vec3& v )
{
    return Vec3( u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x );
}
