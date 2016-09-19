/*********************************************************
** quaternion.h, quaternion.cpp
** Primary author: ?? web . Jeff molofe maybe. dave astell ?
** Description: quaternion
** Used in sim:
*********************************************************/

#ifndef QUATERNION_H
#define QUATERNION_H

#include <cmath>
#include "vector.h" //for PI

class CQuaternion
{
public:
    CQuaternion();
    CQuaternion(float *, int);                          //0 for Radians, 1 for Degrees
    CQuaternion(float *,float *,float);

    CQuaternion(CVector,int);
    float * GetData();
    void CreateFromRadians(float *);
    void CreateFromDegrees(float *);
    void CreateFromDegrees(float,float,float,float);
    void CreateFromSLERP(float *,float *,float);
    CQuaternion operator*(CQuaternion);
    CQuaternion Multi(CQuaternion);
    float operator[](int);
    void Set(float,float,float,float);
    void operator=(CQuaternion);
    float Length();
    void Normalize();
    CQuaternion GetUnitQuat();
    void Negate();

private:
    float Data[4];
};


#endif
