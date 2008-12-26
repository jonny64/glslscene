#pragma once
#include "GL/glew.h"
#include "glutWindow.h"
#include "Vector3D.h"
#include "Camera.h"
#include "Scene.h"


namespace scene
{
class Scene;
typedef void (Scene::*PFNRENDER)();
typedef void (Scene::*PFNRENDERGEOMETRY)();

// These store our width and height for the shadow texture
const GLuint SHADOW_WIDTH  = 512;
const GLuint SHADOW_HEIGHT = 512;
const GLuint SHADOW_MAP_TEXTURE_INDEX = 4;
extern Camera m_camera;

  class ShadowMap
  {
    // We set the light's view position at the origin
    GLfloat m_LightView[3];
    // These are the light's matrices that need to be stored
    GLfloat m_projection[16];
    GLfloat m_modelView[16];
    GLuint m_shadowMapID;

    // copy z buf contents to texture at predefined TU
    void RenderZtoTexture();
  public:

    ShadowMap();
    virtual ~ShadowMap(void);

    void InitShadowTexture();
    void StoreLightMatrices(Vector3D lightPos);
    void ApplyShadowMap(PFNRENDER OutScene, Scene* scene);
    void FlushShadowMap(PFNRENDERGEOMETRY OutSceneGeometry, Scene* scene);
  };

}