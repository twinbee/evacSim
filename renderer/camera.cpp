/**********************************************************
** Camera.cpp, camera.h
** Primary Author: Mike J
** Description: keeps track of a camera position and includes update support functions
** Used in sim: rendering, rendering videos
*********************************************************/


#include <iostream>

#include "camera.h"
#include "matrix.h"

CCamera::~CCamera()
{}

CCamera::CCamera()
{
    Position.x = 0;
    Position.y = 0;
    Position.z = 0;
    View.x = 0;
    View.y = 1;
    View.z = 0;
    Side.x = 1;
    Side.y = 0;
    Side.z = 0;
    Up.x = 0;
    Up.y = 0;
    Up.z = 1;
    sensitivity = 1;
    CurrentXRot = 0;
    speed = 200;

}

CCamera::CCamera(CVector &nPos, CVector &nView, CVector &nUp)
{
    SetOrientation(nPos,nView,nUp);
    sensitivity = 1;
    speed = 200;
}

void CCamera::SetOrientation(CVector &nPos, CVector &nView, CVector &nUp)
{
    Position.x = nPos.x;
    Position.y = nPos.y;
    Position.z = nPos.z;
    View.x = nView.x;
    View.y = nView.y;
    View.z = nView.z;

    Side = Up ^ View;
    Side.Normalize();

    Up.x = nUp.x;
    Up.y = nUp.y;
    Up.z = nUp.z;

    CurrentXRot = 0;
}

void CCamera::SetSensitivity(float value)
{
    sensitivity  = value;
}

float CCamera::GetSpeed()
{
    return speed;
}

void CCamera::SetSpeed(float value)
{
    speed =  value;
}

CVector CCamera::GetPos()
{
    return Position;
}

float CCamera::GetX()
{
    return Position.x;
}

float CCamera::GetY()
{
    return Position.y;
}

float CCamera::GetZ()
{
    return Position.z;
}

CVector CCamera::GetView()
{
    return View;
}

CVector CCamera::GetUp()
{
    return Up;
}

CVector CCamera::GetSide()
{
    return Side;
}



void CCamera::MouseFollow(float mx, float my,double time)
{
    mx = mx/sensitivity * time;
    my = my/sensitivity * time;
    float OldXRot = CurrentXRot;
    CurrentXRot+=my;

    RotateView(mx * -1, Up.x, Up.y, Up.z);


    if ( (CurrentXRot <= MaxXRot) && CurrentXRot >= (MaxXRot * -1))
        RotateView(my, Side.x,Side.y,Side.z);
    else
    {
        if (CurrentXRot < (MaxXRot * -1))//if(CurrentXRot < 0)
        {
            RotateView((MaxXRot * -1) - OldXRot, Side.x,Side.y,Side.z);
            CurrentXRot = MaxXRot * -1;
        }
        else
        {
            RotateView(MaxXRot - OldXRot, Side.x,Side.y,Side.z);
            CurrentXRot = MaxXRot;
        }
    }
}




void CCamera::SetPosition(float x, float y, float z)
{
    Position.x = x;
    Position.y = y;
    Position.z = z;
}

void CCamera::SetPosition(CVector newpos)
{
    Position = newpos;
}



void CCamera::RotateView(float angle, float x, float y, float z)
{

    CQuaternion NewRot;
    NewRot.CreateFromDegrees(angle,x,y,z);
    NewRot.Normalize();
    QuatRot.Normalize();
    NewRot.Multi(QuatRot); //NewRot now equals or new total rotation
    QuatRot = NewRot;

    //renormalize our quaternion
    QuatRot.Normalize();

    CMatrix QuatMatrix;
    QuatMatrix.SetQuatRotation(QuatRot.GetData());

    QuatMatrix.TransformVertex(&View);
    //keep our Side and Up aligned with our new View vector.
    View.Normalize();
    Side = Up ^ View;
    Side.Normalize();


}

void CCamera::StrafeLeft(double time)
{
    Position+= Side * time * speed;

}

void CCamera::Move(CVector vel)
{
    Position += vel;
}

void CCamera::StrafeRight(double time)
{
    Position+= Side * time * speed*-1;

}



void CCamera::MoveForward(double time)
{
    Position+= View * time * speed;
}

void CCamera::MoveBackward(double time)
{
    Position+= View * time * speed * -1;
}

CVector CCamera::GetVelocity()
{
    return View * speed;
}
