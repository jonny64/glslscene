#pragma once

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library
#include <string>										  // Used for our STL string objects
#include <fstream>										// Used for our ifstream object to load text files

#include "Vector3D.h"
#include "GL/freeglut.h"

using namespace std;

//#define SCREEN_WIDTH 800								// We want our screen width 800 pixels
//#define SCREEN_HEIGHT 600								// We want our screen height 600 pixels
//#define SCREEN_DEPTH 16	

namespace scene
{
  // This is our camera class
  class Camera 
  {

  public:

    // Our camera constructor
    Camera();	

    // These are are data access functions for our camera's private data
    Vector3D Position() {	return m_vPosition;		}
    Vector3D View()		{	return m_vView;			}
    Vector3D UpVector() {	return m_vUpVector;		}
    Vector3D Strafe()	{	return m_vStrafe;		}

    // This changes the position, view, and up vector of the camera.
    // This is primarily used for initialization
    void PositionCamera(float positionX, float positionY, float positionZ,
      float viewX,     float viewY,     float viewZ,
      float upVectorX, float upVectorY, float upVectorZ);

    // This rotates the camera's view around the position depending on the values passed in.
    void RotateView(float angle, float X, float Y, float Z);

    // This moves the camera's view by the mouse movements (First person view)
    void SetViewByMouse(float dx, float dy); 

    // This rotates the camera around a point (I.E. your character).
    void RotateAroundPoint(Vector3D vCenter, float X, float Y, float Z);

    // This strafes the camera left or right depending on the speed (+/-) 
    void StrafeCamera(float speed);

    // This will move the camera forward or backward depending on the speed
    void MoveCamera(float speed);

    // This checks for keyboard movement
    void CheckForMovement();

    // This updates the camera's view and other data (Should be called each frame)
    void Update();

    // This uses gluLookAt() to tell OpenGL where to look
    void Look();

    Vector3D GetPosition();

    Vector3D GetTarget();

    void SetupMirroredModelview();

    // This returns the inverse to the current modelview matrix in OpenGL
    void GetInverseMatrix(float mCameraInverse[16]);


  private:

    // The camera's position(eye)
    Vector3D m_vPosition;					

    // The camera's view (target, center)
    Vector3D m_vView;						

    // The camera's up vector
    Vector3D m_vUpVector;		

    // The camera's strafe vector
    Vector3D m_vStrafe;						
  };


  // This makes sure we only draw at a specified frame rate
  bool AnimateNextFrame(int desiredFrameRate);
}