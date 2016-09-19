/**********************************************************
** Camera.cpp, camera.h
** Primary Author: Mike J
** Description: keeps track of a camera position and includes update support functions
** Used in sim: rendering, rendering videos
*********************************************************/


#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"
#include "quaternion.h"

#include <iostream>

#define MaxXRot 1.5 //radians, 2 = 90 degrees  MAX ROTATION ON THE X AXIS(UP AND DOWN VIEW)

class CCamera
{

private:
    CVector View;             //camera view vector
    CVector Up;               //camera up vector
    CVector Side;             //camera side vector
    CVector Position;         //camera position

    CQuaternion QuatRot;      //camera's alignment

    int oldmousex,oldmousey;
    float sensitivity;        //camera rotation sensitivity
    float CurrentXRot;        //used to test against MaxXRot
    float speed;              //camera fly-speed

    friend std::ostream& operator<< (std::ostream& os, const CCamera& cam);
    friend std::istream& operator>> (std::istream& os, CCamera& cam);

    void RotateView(float,float,float,float);

public:

    ~CCamera();
    CCamera();
    CCamera(CVector &,CVector &,CVector &);

    //Data access functions
    CVector GetPos();
    float GetX();
    float GetY();
    float GetZ();
    CVector GetUp();
    CVector GetSide();
    CVector GetView();
    float GetSpeed();
    CVector GetVelocity();

    //attribute setting functions
    void SetPosition(float, float, float);
    void SetPosition(CVector);
    void SetSensitivity(float);              //set rotation sensitivity
    void SetSpeed(float);                    //set camera speed

    void SetOrientation(CVector &nPos, CVector &nView, CVector &nUp);

    //update functions
    void Move(CVector);
    void MouseFollow(float,float,double);  //follow mousex,mousey
    void StrafeLeft(double);
    void StrafeRight(double);
    void MoveForward(double);
    void MoveBackward(double);

};

inline std::ostream& operator<< (std::ostream& os, const CCamera& cam)
{
    return os << cam.Position << std::endl
           << cam.View << std::endl
           << cam.Up  << std::endl;
}

inline std::istream& operator>> (std::istream& os, CCamera& cam)
{
    CVector temp;
    os >> cam.Position >> cam.View >>  cam.Up;

    return os;
}


#endif





