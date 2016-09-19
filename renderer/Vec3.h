/*********************************************************
** vec3.cpp, vec3.h
** Primary author: Mike E
** Description: 3-vector
** Used in sim:
*********************************************************/

#ifndef mce_vector
#define mce_vector

struct Vec3
{
    float x, y, z;

    // constructors
    Vec3(); // zero by default
    Vec3( float arg_x, float arg_y, float arg_z ); // arbitrary vector

    // queries
    float magnitude() const;
    float mag2() const;
    Vec3 normalized() const;

    // mutators
    void normalize();
    void operator += ( const Vec3& u );

    // a useful constant
    static const Vec3 zero;
};

// operators
bool operator == ( const Vec3& u, const Vec3& v );
bool operator != ( const Vec3& u, const Vec3& v );
Vec3 operator + ( const Vec3& u, const Vec3& v );
Vec3 operator - ( const Vec3& u, const Vec3& v );
Vec3 operator * ( float s, const Vec3& u );
Vec3 operator - ( const Vec3& u );
float dot( const Vec3& u, const Vec3& v );
Vec3 proj( const Vec3& u, const Vec3& v );
Vec3 cross( const Vec3& u, const Vec3& v );

// a useful synonym
typedef Vec3 Point;

#endif
