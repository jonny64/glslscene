#pragma once
#include "GL/glew.h"
#include "TgaLoader.h"
#include "Tangent.h"

const int BUMP_TEXTURE_INDEX = 1;
const int OPACITY_TEXTURE_INDEX = 1;
namespace scene
{
  class Map
  {
  public:
    int CountMap;
    char Tex1[256];
    char Tex2[256];
    char Tex3[256];
    unsigned int tex1,tex2,tex3;

    Map(void)
    {
      CountMap=0;
    };
  };

  class Material
  {
  public:
    void Build();
    void Bind();

    Map DiffuseMap;
    Map OpacityMap;
    Map BumpMap;
  private:
  };

}