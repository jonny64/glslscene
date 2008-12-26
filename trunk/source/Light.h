#pragma once
#include "Vector3D.h"

namespace scene
{
  class Light
  {
  public:
    Vector3D Position;
    Vector3D Color;
    // Vector3D Specular;
    // Vector3D Ambient;
    Light()
    {
      Vector3D defaultPos(0.0f, 0.0f, 0.0f);
      Vector3D defaultColor(0.0f, 0.0f, 0.0f);

      Position = defaultPos;
      Color = defaultColor;
    };

    // mark light position 
    void Render()
    {
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glTranslatef(Position.x, Position.y, Position.z);
      glColor3f( 0.5f, 0.5f, 0.5f );
      glutWireCube(0.2);
      glPopMatrix();
    }
  };

}