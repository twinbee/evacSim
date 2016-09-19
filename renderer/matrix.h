#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

//STORED IN ROW MAJOR ORDER

class CMatrix
{
public:
    CMatrix();
    ~CMatrix();

    //Data Setting Functions
    void SetIdentity();
    void SetMatrix(float *);
    void SetQuatRotation(float *);
    void FromVectors(CVector&,CVector&,CVector&);
    void SetTranslation(float,float,float);

    //Matrix-Matrix Operations
    void PostMultiply(float *);
    void Add(float *);
    void Subtract(float *);

    //State alterations
    void SetInverse();
    void Transpose();

    //Vector Transformations
    void InverseTransformVertex(float *);
    void InverseRotateVertex(float *);
    void RotateVertex(float *);
    void TransformVertex(float *);
    void TransformVertex(CVector *);

    //accessor functions
    float * GetData();

private:
    float Data[16];
};


#endif

