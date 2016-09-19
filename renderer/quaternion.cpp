/*********************************************************
** quaternion.h, quaternion.cpp
** Primary author: ?? web . Jeff molofe maybe. dave astell ?
** Description: quaternion
** Used in sim:
*********************************************************/

#include "quaternion.h"


CQuaternion::CQuaternion()
{
    Data[0] = 0;
    Data[1] = 0;
    Data[2] = 0;
    Data[3] = 1;
}


CQuaternion::CQuaternion(float * Parameter, int Format)
{
    if (Format == 0)
        CreateFromRadians(Parameter);
    if (Format == 1)
        CreateFromDegrees(Parameter);
}

CQuaternion::CQuaternion(CVector Parameter, int Format)
{
    float Parameterf[3] = {Parameter.x,Parameter.y,Parameter.z};

    if (Format == 0)
        CreateFromRadians(Parameterf);
    if (Format == 1)
        CreateFromDegrees(Parameterf);
}

CQuaternion::CQuaternion(float* Quat1,float* Quat2,float Interpolation)
{
    CreateFromSLERP(Quat1,Quat2,Interpolation);
}


void CQuaternion::CreateFromRadians(float * Parameter)
{
    float X = Parameter[0];
    float Y = Parameter[1];
    float Z = Parameter[2];

    float angle;
    double sx, sy, sz, cx, cy, cz;

    angle = Z*0.5f;
    sz = sin( angle );
    cz = cos( angle );
    angle = Y*0.5f;
    sy = sin( angle );
    cy = cos( angle );
    angle = X*0.5f;
    sx = sin( angle );
    cx = cos( angle );

    double cxcy = cx*cy;
    double sxsy = sx*sy;

    Data[0] = ( float )( sx*cy*cz-cx*sy*sz );
    Data[1] = ( float )( cx*sy*cz+sx*cy*sz );
    Data[2] = ( float )( cxcy*sz-sxsy*cz );
    Data[3] = ( float )( cxcy*cz+sxsy*sz );

}

void CQuaternion::CreateFromDegrees(float * Parameter)
{
    float X = Parameter[0];
    float Y = Parameter[1];
    float Z = Parameter[2];

    float angle;
    double sx, sy, sz, cx, cy, cz;

    angle = (float)((Z/180 * PI)*0.5f);
    sz = sin( angle );
    cz = cos( angle );
    angle = (float)((Y/180 * PI)*0.5f);
    sy = sin( angle );
    cy = cos( angle );
    angle = (float)((X/180 * PI)*0.5f);
    sx = sin( angle );
    cx = cos( angle );

    double cxcy = cx*cy;
    double sxsy = sx*sy;

    Data[0] = ( float )( sx*cy*cz-cx*sy*sz );
    Data[1] = ( float )( cx*sy*cz+sx*cy*sz );
    Data[2] = ( float )( cxcy*sz-sxsy*cz );
    Data[3] = ( float )( cxcy*cz+sxsy*sz );

}

void CQuaternion::CreateFromDegrees(float angle,float x,float y,float z)
{
    //axis is a unit vector
    Data[3]  = cosf(angle/2);
    Data[0] = x * sinf(angle/2 );
    Data[1] = y * sinf(angle/2 );
    Data[2] = z * sinf(angle/2 );
}

float * CQuaternion::GetData()
{
    return Data;
}

void CQuaternion::CreateFromSLERP(float * q1,float * q2,float interp)
{

    int i;
    float a = 0, b = 0;
    for ( i = 0; i < 4; i++ )
    {
        a += ( q1[i]-q2[i] )*( q1[i]-q2[i] );
        b += ( q1[i]+q2[i] )*( q1[i]+q2[i] );
    }
    if ( a > b )
    {
        q2[0]=-q2[0];
        q2[1]=-q2[1];
        q2[2]=-q2[2];
        q2[3]=-q2[3];
    }

    float cosom = q1[0]*q2[0]+q1[1]*q2[1]+q1[2]*q2[2]+q1[3]*q2[3];
    double sclq1, sclq2;

    if (( 1.0+cosom ) > 0.00000001 )
    {
        if (( 1.0-cosom ) > 0.00000001 )
        {
            double omega = acos( cosom );
            double sinom = sin( omega );
            sclq1 = sin(( 1.0-interp )*omega )/sinom;
            sclq2 = sin( interp*omega )/sinom;
        }
        else
        {
            sclq1 = 1.0-interp;
            sclq2 = interp;
        }
        for ( i = 0; i < 4; i++ )
            Data[i] = ( float )( sclq1*q1[i]+sclq2*q2[i] );
    }
    else
    {
        Data[0] = -q1[1];
        Data[1] = q1[0];
        Data[2] = -q1[3];
        Data[3] = q1[2];

        sclq1 = sin(( 1.0-interp )*0.5*PI );
        sclq2 = sin( interp*0.5*PI );
        for ( i = 0; i < 3; i++ )
            Data[i] = ( float )( sclq1*q1[i]+sclq2*Data[i] );
    }

}

float CQuaternion::Length()
{
    return (float)sqrt(Data[0] * Data[0] + Data[1] * Data[1]
                       + Data[2] * Data[2] + Data[3] * Data[3]);
}

void CQuaternion::Normalize()
{
    float scaler = Length();
    Data[0] /= scaler;
    Data[1] /= scaler;
    Data[2] /= scaler;
    Data[3] /= scaler;
}

CQuaternion CQuaternion::GetUnitQuat()
{
    CQuaternion temp = *this;
    temp.Normalize();
    return temp;
}

void CQuaternion::Negate()
{
    Data[0] *= -1;
    Data[1] *= -1;
    Data[2] *= -1;
}


float CQuaternion::operator[](int idx)
{
    return Data[idx];
}

void CQuaternion::Set(float x,float y,float z,float w)
{
    Data[0] = x;
    Data[1] = y;
    Data[2] = z;
    Data[3] = w;
}

CQuaternion CQuaternion::operator*(CQuaternion quat)
{
    float C[4];

    C[0] = Data[3]*quat[0] + Data[0]*quat[3] + Data[1]*quat[2] - Data[2]*quat[1];
    C[1] = Data[3]*quat[1] - Data[0]*quat[2] + Data[1]*quat[3] + Data[2]*quat[0];
    C[2] = Data[3]*quat[2] + Data[0]*quat[1] - Data[1]*quat[0] + Data[2]*quat[3];
    C[3] = Data[3]*quat[3] - Data[0]*quat[0] - Data[1]*quat[1] - Data[2]*quat[2];
    return CQuaternion(C,0);
}

CQuaternion CQuaternion::Multi(CQuaternion quat)
{
    float C[4];

    C[0] = Data[3]*quat[0] + Data[0]*quat[3] + Data[1]*quat[2] - Data[2]*quat[1];
    C[1] = Data[3]*quat[1] - Data[0]*quat[2] + Data[1]*quat[3] + Data[2]*quat[0];
    C[2] = Data[3]*quat[2] + Data[0]*quat[1] - Data[1]*quat[0] + Data[2]*quat[3];
    C[3] = Data[3]*quat[3] - Data[0]*quat[0] - Data[1]*quat[1] - Data[2]*quat[2];
    return CQuaternion(C,0);
}

void CQuaternion::operator=(CQuaternion temp)
{
    float * data = temp.GetData();
    Data[0] = data[0];
    Data[1] = data[1];
    Data[2] = data[2];
    Data[3] = data[3];
}
