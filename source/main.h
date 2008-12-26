#pragma once

#include <assert.h>
#include <iostream>
#include <string.h>
#include "windows.h"
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glApplication.h"
#include "glutWindow.h"
#include "glinfo.h"
#include "glsl.h"
#include "Camera.h"
#include "Vector3D.h"
#include "ShadowMap.h"
#include "Scene.h"
#include "ShadowMap.h"
namespace scene
{
  // Camera - related
  const float STRAFE_SENSIVITY = 0.5;
  const float MOUSE_STRAFE_SENSIVITY = 100.0f;

  Camera m_camera;
  ShadowMap m_shadowMap;
}
using namespace std;