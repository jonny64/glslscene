#include "Material.h"

using namespace scene;

void Material::Bind()
{
  switch (DiffuseMap.CountMap)
  {
    // simple material
  case 1:
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glBindTexture(GL_TEXTURE_2D, DiffuseMap.tex1);
    break;

  //composite
  case 2:
    //...texture 1 (TU0)
    glActiveTextureARB(GL_TEXTURE0_ARB);
    //glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, DiffuseMap.tex1);
    //...mixes with texture 2 (TU1)
    glActiveTextureARB(GL_TEXTURE1_ARB);
    //glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, DiffuseMap.tex2);
    //... using as mask texture 3 from TU2
    break;
  
    // material mix based on mask
  case 3:
    //...texture 1 (TU0)
    glActiveTextureARB(GL_TEXTURE0_ARB);
    //glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, DiffuseMap.tex2);
    //...mixes with texture 2 (TU1)
    glActiveTextureARB(GL_TEXTURE1_ARB);
    //glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, DiffuseMap.tex1);
    //... using as mask texture 3 from TU2
    glActiveTextureARB(GL_TEXTURE2_ARB);
    //glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, DiffuseMap.tex3);
    break;
  };

  // used opacity map
  if (BumpMap.CountMap == 1)
  {
    glActiveTextureARB(GL_TEXTURE0_ARB + BUMP_TEXTURE_INDEX );
    //glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, BumpMap.tex1);
  };

  // used bump map
  if (OpacityMap.CountMap == 1)
  {
    glActiveTextureARB(GL_TEXTURE0_ARB + OPACITY_TEXTURE_INDEX);
    //glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, OpacityMap.tex1);
  };
}

void Material::Build()
{
  Texture tex;

  if (DiffuseMap.CountMap == 1)
  {
    if (tex.Load(DiffuseMap.Tex1))
    {
      DiffuseMap.tex1 = tex.getID();
    }
    else
    {
      assert(0);
    }
  };
  if (DiffuseMap.CountMap == 2)
  {
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    tex.Load(DiffuseMap.Tex1);		
    DiffuseMap.tex1 = tex.getID();

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
    tex.Load(DiffuseMap.Tex2);		
    DiffuseMap.tex2=tex.getID();
    return;
  };
  if (DiffuseMap.CountMap == 3)
  {
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    tex.Load(DiffuseMap.Tex1);		
    DiffuseMap.tex1 = tex.getID();

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
    tex.Load(DiffuseMap.Tex2);		
    DiffuseMap.tex2=tex.getID();

    glActiveTextureARB(GL_TEXTURE2_ARB);
    glEnable(GL_TEXTURE_2D);
    tex.Load(DiffuseMap.Tex3);		

    DiffuseMap.tex3 = tex.getID();
    return;
  };
  if (OpacityMap.CountMap == 1)
  {
    if (tex.Load(OpacityMap.Tex1))
    {
      OpacityMap.tex1 = tex.getID();
    }
    else
    {
      assert(0);
    }
  }
  if (BumpMap.CountMap == 1)
  {
    if (tex.Load(BumpMap.Tex1))
    {
      BumpMap.tex1 = tex.getID();
    }
    else
    {
      assert(0);
    }
  }
}