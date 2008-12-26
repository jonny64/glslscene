#pragma once

#include <stdio.h>
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glApplication.h"
#include "glutWindow.h"
#include "glinfo.h"
#include "glsl.h"

#include <string.h>
#include <assert.h>

#include "Material.h"
#include "Mesh.h"
#include "Light.h"
#include "TgaLoader.h"
#include "Tangent.h"
#include "Vector3D.h"
#include "StatusCodes.h"
#include "StringUtils.h"

#include "glsl.h"
#include "Camera.h"
#include "Vector3D.h"
#include "ShadowMap.h"

#define TANGENT
//#define OLD_CUP_SHADER

namespace scene
{
static const string SCENE_FILE_NAME = "scene.ase";
static const string MIRROR_OBJ_NAME  = "Mirror";
static const string CUP_OBJ_NAME = "Cup";
static const string WIRE_OBJ_NAME  = "SandClockGlass";
static const string CERAMIC_OBJ_NAME  = "Teapot";

static GLfloat MIRROR_COLOR[4] = {0.2f, 0.8f, 1.0f, 0.3f};
class ShadowMap;
extern ShadowMap m_shadowMap;

  class Scene
  {
    friend class ShadowMap;

    char str[128];

    Light m_light;

    GLuint* m_vertexVBOs;
    GLuint* m_indexVBOs;
    GLuint* m_normVBOs;
    GLuint* m_texVBOs;
    GLuint* m_tangentVBOs;


    cwc::glShaderManager shader_manager;
    cwc::glShader* defaultShader;
    cwc::glShader* cupShader;
    cwc::glShader* mirrorShader;
    cwc::glShader* wireGauzeShader;
    cwc::glShader* ceramicShader;

    void BuildMaterials();

    void SetCommonShaderUniforms(cwc::glShader* shader);
    void InitShaders();
    void SetupMirroredModelview();
    void OutMirrorGeometry();
    void OutMirrorTextured();
    void OutMirrorAndReflection();
    void OutObject(int objIndex, bool geometryOnly);
    void OutCup( int objIndex );
    void OutWireGauze( int objIndex );
    void OutBread(int objIndex);
    void OutSceneGeometry();
    void OutScene();

    int LoadObject(FILE *f);
    int LoadLightObject (FILE *f);
    int LoadMaterial (FILE *f);
  public:
    // objects
    int MeshCount;
    Mesh meshes[100];

    // light source
    Light light;

    // materials
    int MaterialCount;
    Material material[100];

    Scene ()
    {
      MeshCount = 0;
      MaterialCount = 0;
    }

    ~Scene()
    {
    }

    // loading scene from ase file
    int LoadFromFile(const char*);

    // no texturing
  public:
    void Init();
    void DrawObjectGeometry(int);
    void DrawObject(int);
    void DrawBumpmappedObject(int);
    void Render();
  };

}