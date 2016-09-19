
#include "matrix.h"

// 4 x 4 matrix class

CMatrix::CMatrix()
{
    SetIdentity();
}

CMatrix::~CMatrix()
{
}

void CMatrix::SetMatrix(float * Matrix)
{
    for (int i=0;i<16;i++)
        Data[i]=Matrix[i];
}

float * CMatrix::GetData()
{
    return Data;
}

void CMatrix::SetIdentity() //ok
{
    Data[0]=1;
    Data[1]=0;
    Data[2]=0;
    Data[3]=0;
    Data[4]=0;
    Data[5]=1;
    Data[6]=0;
    Data[7]=0;
    Data[8]=0;
    Data[9]=0;
    Data[10]=1;
    Data[11]=0;
    Data[12]=0;
    Data[13]=0;
    Data[14]=0;
    Data[15]=1;
}

void CMatrix::Add(float * Matrix)
{
    for (int i=0;i<16;i++)
        Data[i]+=Matrix[i];
}

void CMatrix::Subtract(float * Matrix)
{
    for (int i=0;i<16;i++)
        Data[i]-=Matrix[i];
}

void CMatrix::PostMultiply(float * Matrix)
{
    float * m2 = Matrix;
    float NewMatrix[16];

    NewMatrix[0] = Data[0]*m2[0] + Data[1]*m2[4] + Data[2]*m2[8];
    NewMatrix[4] = Data[4]*m2[0] + Data[5]*m2[4] + Data[6]*m2[8];
    NewMatrix[8] = Data[8]*m2[0] + Data[9]*m2[4] + Data[10]*m2[8];
    NewMatrix[12] = 0;

    NewMatrix[1] = Data[0]*m2[1] + Data[1]*m2[5] + Data[2]*m2[9];
    NewMatrix[5] = Data[4]*m2[1] + Data[5]*m2[5] + Data[6]*m2[9];
    NewMatrix[9] = Data[8]*m2[1] + Data[9]*m2[5] + Data[10]*m2[9];
    NewMatrix[13] = 0;

    NewMatrix[2] = Data[0]*m2[2] + Data[1]*m2[6] + Data[2]*m2[10];
    NewMatrix[6] = Data[4]*m2[2] + Data[5]*m2[6] + Data[6]*m2[10];
    NewMatrix[10] = Data[8]*m2[2] + Data[9]*m2[6] + Data[10]*m2[10];
    NewMatrix[14] = 0;

    NewMatrix[3] = Data[0]*m2[3] + Data[1]*m2[7] + Data[2]*m2[11] + Data[3];
    NewMatrix[7] = Data[4]*m2[3] + Data[5]*m2[7] + Data[6]*m2[11] + Data[7];
    NewMatrix[11] = Data[8]*m2[3] + Data[9]*m2[7] + Data[10]*m2[11] + Data[11];
    NewMatrix[15] = 1;

    SetMatrix(NewMatrix);

}







void CMatrix::SetQuatRotation(float * Quat)
{
    float x=Quat[0];
    float y=Quat[1];
    float z=Quat[2];
    float w=Quat[3];


    Data[0] = ( float )( 1.0 - 2.0*y*y - 2.0*z*z );
    Data[4] = ( float )( 2.0*x*y + 2.0*w*z );
    Data[8] = ( float )( 2.0*x*z - 2.0*w*y );

    Data[1] = ( float )( 2.0*x*y - 2.0*w*z );
    Data[5] = ( float )( 1.0 - 2.0*x*x - 2.0*z*z );
    Data[9] = ( float )( 2.0*y*z + 2.0*w*x );

    Data[2] = ( float )( 2.0*x*z + 2.0*w*y );
    Data[6] = ( float )( 2.0*y*z - 2.0*w*x );
    Data[10] = ( float )( 1.0 - 2.0*x*x - 2.0*y*y );

}

void CMatrix::SetTranslation(float dx, float dy, float dz)
{
    Data[12]=dx;
    Data[13]=dy;
    Data[14]=dz;


}

void CMatrix::SetInverse()
{
    float NewMatrix[16];
    NewMatrix[0]=Data[0];
    NewMatrix[1]=Data[4];
    NewMatrix[2]=Data[8];
    NewMatrix[3]=-Data[3];
    NewMatrix[4]=Data[1];
    NewMatrix[5]=Data[5];
    NewMatrix[6]=Data[9];
    NewMatrix[7]=-Data[7];
    NewMatrix[8]=Data[2];
    NewMatrix[9]=Data[6];
    NewMatrix[10]=Data[10];
    NewMatrix[11]=-Data[11];
    NewMatrix[12]=-Data[12];
    NewMatrix[13]=-Data[13];
    NewMatrix[14]=-Data[14];
    NewMatrix[15]=-Data[15];
    SetMatrix(NewMatrix);
}



void CMatrix::TransformVertex(float * Vertex)
{
    float NewVertex[4];
//Vertex[3] = 1;
    NewVertex[3] = 1; //homogenous coord
    NewVertex[0]=(Data[0]*Vertex[0]) + (Data[1] * Vertex[1]) + (Data[2] * Vertex[2]) + (Data[3] * 1);//Vertex[3]);
    NewVertex[1]=(Data[4]*Vertex[0]) + (Data[5] * Vertex[1])+ (Data[6] * Vertex[2]) + (Data[7] * 1);//Vertex[3]);
    NewVertex[2]=(Data[8]*Vertex[0]) + (Data[9] * Vertex[1]) + (Data[10] * Vertex[2]) + (Data[11] * 1);//Vertex[3]);
//NewVertex[3]=(Data[12]*Vertex[0]) + (Data[13] * Vertex[1]) + (Data[14] * Vertex[2]) + (Data[15] * 1);//Vertex[3]);
    Vertex[0] = NewVertex[0];
    Vertex[1] = NewVertex[1];
    Vertex[2] = NewVertex[2];
//Vertex[3] = NewVertex[3];
}


void CMatrix::TransformVertex(CVector * Vertex)
{
    float NewVertex[3];

    NewVertex[0]=(Data[0]*Vertex->x) + (Data[1] * Vertex->y) + (Data[2] * Vertex->z) + (Data[3] * 1);
    NewVertex[1]=(Data[4]*Vertex->x) + (Data[5] * Vertex->y)+ (Data[6] * Vertex->z) + (Data[7] * 1);
    NewVertex[2]=(Data[8]*Vertex->x) + (Data[9] * Vertex->y) + (Data[10] * Vertex->z) + (Data[11] * 1);

    Vertex->x = NewVertex[0];
    Vertex->y = NewVertex[1];
    Vertex->z = NewVertex[2];

}


void CMatrix::InverseTransformVertex(float * Vertex)
{
    float NewVertex[4];
//Vertex[3] = 1;

//translate first, THEN rotate, because initially translation was done second, so gotta reverse it
    Vertex[0]-=Data[3];
    Vertex[1]-=Data[7];
    Vertex[2]-=Data[11];
//Vertex[3] = 1;  //Vertex
    NewVertex[0]=(Data[0]*Vertex[0]) + (Data[4] * Vertex[1]) + (Data[8] * Vertex[2]);
    NewVertex[1]=(Data[1]*Vertex[0]) + (Data[5] * Vertex[1])+ (Data[9] * Vertex[2]);
    NewVertex[2]=(Data[2]*Vertex[0]) + (Data[6] * Vertex[1]) + (Data[10] * Vertex[2]);
//NewVertex[3]=1;//(Data[12]*Vertex[0]) + (Data[13] * Vertex[1]) + (Data[14] * Vertex[2]);

    Vertex[0] = NewVertex[0];
    Vertex[1] = NewVertex[1];
    Vertex[2] = NewVertex[2];
//Vertex[3] = NewVertex[3];

}

void CMatrix::InverseRotateVertex(float * Vertex)
{
    float NewVertex[4];

    NewVertex[0]=(Data[0]*Vertex[0]) + (Data[4] * Vertex[1]) + (Data[8] * Vertex[2]);
    NewVertex[1]=(Data[1]*Vertex[0]) + (Data[5] * Vertex[1])+ (Data[9] * Vertex[2]);
    NewVertex[2]=(Data[2]*Vertex[0]) + (Data[6] * Vertex[1]) + (Data[10] * Vertex[2]);


    Vertex[0] = NewVertex[0];
    Vertex[1] = NewVertex[1];
    Vertex[2] = NewVertex[2];
}

void CMatrix::RotateVertex(float * Vertex)
{
    float NewVertex[4];

    NewVertex[0]=(Data[0]*Vertex[0]) + (Data[1] * Vertex[1]) + (Data[2] * Vertex[2]);
    NewVertex[1]=(Data[4]*Vertex[0]) + (Data[5] * Vertex[1])+ (Data[6] * Vertex[2]);
    NewVertex[2]=(Data[8]*Vertex[0]) + (Data[9] * Vertex[1]) + (Data[10] * Vertex[2]);


    Vertex[0] = NewVertex[0];
    Vertex[1] = NewVertex[1];
    Vertex[2] = NewVertex[2];
}

void CMatrix::FromVectors(CVector &xaxis, CVector &yaxis, CVector &zaxis)
{

    Data[0] = xaxis.x;
    Data[4] = xaxis.y;
    Data[8] = xaxis.z;
    Data[3] = 0;

    Data[1] = yaxis.x;
    Data[5] = yaxis.y;
    Data[9] = yaxis.z;
    Data[7] = 0;

    Data[2] = zaxis.x;
    Data[6] = zaxis.y;
    Data[10] = zaxis.z;
    Data[11] = 0;

    Data[12] = 0;
    Data[13] = 0;
    Data[14] = 0;
    Data[15] = 1;

}

void CMatrix::Transpose()
{

    float NewMatrix[16];
    NewMatrix[0]=Data[0];
    NewMatrix[1]=Data[4];
    NewMatrix[2]=Data[8];
    NewMatrix[3]=Data[12];
    NewMatrix[4]=Data[1];
    NewMatrix[5]=Data[5];
    NewMatrix[6]=Data[9];
    NewMatrix[7]=Data[13];
    NewMatrix[8]=Data[2];
    NewMatrix[9]=Data[6];
    NewMatrix[10]=Data[10];
    NewMatrix[11]=Data[14];
    NewMatrix[12]=Data[3];
    NewMatrix[13]=Data[7];
    NewMatrix[14]=Data[11];
    NewMatrix[15]=Data[15];
    SetMatrix(NewMatrix);



}

