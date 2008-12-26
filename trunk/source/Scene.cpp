#include "Scene.h"
const int MAX_NAME_LENGTH = 50;

using namespace scene;

// this stuff cannot should be done only after 
// opengl context is initialized
void Scene::Init()
{ 
  // load scene
  LoadFromFile(SCENE_FILE_NAME.c_str());
  m_light = light;
  InitShaders();
}

void Scene::DrawObjectGeometry(int objID)
{
  meshes[objID].RenderGeometry();
}


void Scene::DrawObject(int objID)
{
  material[meshes[objID].MaterialId].Bind();

  meshes[objID].Render();
}

void Scene::DrawBumpmappedObject(int objID)
{
  material[meshes[objID].MaterialId].Bind();
  meshes[objID].Render();
  return;

  glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, m_indexVBOs[objID] );


  // transfer normals
  glEnableClientState(GL_NORMAL_ARRAY);
  glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_normVBOs[objID] );
  glNormalPointer(GL_FLOAT,0,(GLvoid*)0);

  // transfer texture coordinates
  glClientActiveTextureARB(GL_TEXTURE0_ARB);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_texVBOs[objID] );
  glTexCoordPointer(3,GL_FLOAT,0,(GLvoid*)0);

  // transfer vertices
  glEnableClientState(GL_VERTEX_ARRAY);
  glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_vertexVBOs[objID] );
  glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)0);

  // transfer additional attributes
#ifdef TANGENT
  glEnableVertexAttribArrayARB( TANGENT_GENERIC_ATTRIBUTE_INDEX );
  //void glVertexAttribPointer(GLuint index,
  //GLint size,
  //GLenum type,
  //GLboolean normalized,
  //GLsizei stride,
  //const GLvoid *pointer)

  glVertexAttribPointerARB( TANGENT_GENERIC_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0 );
#endif

  glDrawArrays( GL_TRIANGLES, 0, meshes[objID].IndexCount * 3 );

  // clean up
#ifdef TANGENT
  glDisableVertexAttribArrayARB( TANGENT_GENERIC_ATTRIBUTE_INDEX );
#endif
  glDisableClientState( GL_VERTEX_ARRAY );	
  glDisableClientState( GL_NORMAL_ARRAY );	
  glDisableClientState( GL_TEXTURE_COORD_ARRAY );	
}


void Scene::BuildMaterials()
{
  glActiveTextureARB(GL_TEXTURE0_ARB);
  glEnable(GL_TEXTURE_2D);

  for (int i = 0; i < MaterialCount; i++)
  {
    material[i].Build();
  }
}

void Scene::Render()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.2f);
  // start using shader. OpenGL calls go through vertex 
  // and then fragment program associated to the shader
  if (defaultShader) defaultShader->begin();

  OutMirrorAndReflection();

  //OutScene();
  m_shadowMap.StoreLightMatrices(m_light.Position);
  m_shadowMap.FlushShadowMap(&Scene::OutSceneGeometry, this);
  m_shadowMap.ApplyShadowMap(&Scene::OutScene, this);

  // stop using shader. OpenGL calls will go through regular pipeline
  if (defaultShader) defaultShader->end();

  m_camera.Update();
  m_camera.Look();

  glutSwapBuffers();
}

void Scene::SetCommonShaderUniforms(cwc::glShader* shader)
{
  shader->setUniform1i("tex", 0);
  shader->setUniform3f("vecLight", m_light.Position.x, m_light.Position.y, m_light.Position.z);
  shader->setUniform3f("lightColor", m_light.Color.x, m_light.Color.y, m_light.Color.z);
  // this TU holds shadow map
  shader->setUniform1i("shadowMap", SHADOW_MAP_TEXTURE_INDEX);
  shader->setUniform1i("shadowMapTexture", SHADOW_MAP_TEXTURE_INDEX);
}
void Scene::InitShaders()
{
  cwc::glInfo   oGLInfo;
  oGLInfo.Init();

  bool bOk = cwc::HasGLSLSupport();

  // if we have shader support
  if (bOk)
  {
    defaultShader = shader_manager.loadfromFile( "shaders/default.vert",
      "shaders/default.frag" );
#ifdef OLD_CUP_SHADER
    cupShader = shader_manager.loadfromFile( "shaders/cupOld.vert", "shaders/cupOld.frag" );
#else
    cupShader = shader_manager.loadfromFile( "shaders/cupBump.vert", "shaders/cupBump.frag" );
#endif
    mirrorShader = shader_manager.loadfromFile( "shaders/mirror.vert", "shaders/mirror.frag" );
    wireGauzeShader = shader_manager.loadfromFile( "shaders/wireGauze.vert", "shaders/wireGauze.frag" );
    ceramicShader = shader_manager.loadfromFile( "shaders/ceramic.vert", "shaders/ceramic.frag" );
  }

  // setup uniform variables
  cupShader->begin();
  cupShader->setUniform3f("LightPos", m_light.Position.x, m_light.Position.y, m_light.Position.z);
  cupShader->setUniform1i("DecalTex", 0 );
  cupShader->setUniform1i("BumpTex", BUMP_TEXTURE_INDEX);
#ifdef TANGENT
  cupShader->BindAttribLocation( TANGENT_GENERIC_ATTRIBUTE_INDEX, "Tangent" );
#endif

  mirrorShader->begin();
  mirrorShader->setUniform1i("tex", 0);
  mirrorShader->setUniform1i("shadowMap", SHADOW_MAP_TEXTURE_INDEX ); 
  mirrorShader->setUniform1i("shadowMapTexture", SHADOW_MAP_TEXTURE_INDEX);

  wireGauzeShader->begin();
  wireGauzeShader->setUniform1i("tex", 0);
  wireGauzeShader->setUniform1i("opacityTex", OPACITY_TEXTURE_INDEX);
  wireGauzeShader->setUniform3f("vecLight", m_light.Position.x, m_light.Position.y, m_light.Position.z);

  defaultShader->begin();
  defaultShader->setUniform1i("tex", 0);
  defaultShader->setUniform3f("vecLight", m_light.Position.x, m_light.Position.y, m_light.Position.z);
  defaultShader->setUniform3f("lightColor", m_light.Color.x, m_light.Color.y, m_light.Color.z);
  // this TU holds shadow map
  defaultShader->setUniform1i("shadowMap", SHADOW_MAP_TEXTURE_INDEX);
  defaultShader->setUniform1i("shadowMapTexture", SHADOW_MAP_TEXTURE_INDEX);

  ceramicShader->begin();
  ceramicShader->setUniform1i("tex1", 0);
  ceramicShader->setUniform1i("tex2", 1);
  ceramicShader->setUniform3f("vecLight", m_light.Position.x, m_light.Position.y, m_light.Position.z);
  ceramicShader->setUniform3f("lightColor", m_light.Color.x, m_light.Color.y, m_light.Color.z);
  ceramicShader->setUniform1i("shadowMap", SHADOW_MAP_TEXTURE_INDEX ); 
  ceramicShader->setUniform1i("shadowMapTexture", SHADOW_MAP_TEXTURE_INDEX);

  
  glUseProgram(0);
}


void Scene::OutMirrorGeometry()
{
  // search for and out mirror
  for (int objIndex = 0; objIndex < MeshCount; objIndex++)
  {
    bool currObjIsMirror = ( meshes[objIndex].Name == MIRROR_OBJ_NAME);
    if (currObjIsMirror) 
    {
      defaultShader->end();
      // no texturing 

      glClientActiveTextureARB(GL_TEXTURE0_ARB);
      glDisable(GL_TEXTURE_2D);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);

      DrawObjectGeometry(objIndex);

      defaultShader->begin();
      return;
    }
  };
}

void Scene::OutMirrorTextured()
{
  // search for and out mirror
  for (int objIndex = 0; objIndex < MeshCount; objIndex++)
  {
    bool currObjIsMirror = ( meshes[objIndex].Name == MIRROR_OBJ_NAME);
    if (currObjIsMirror) 
    {
      DrawObject(objIndex);
      return;
    }
  };
}

void Scene::OutMirrorAndReflection()
{
  // enable writing to all buffers
  glDepthMask(TRUE);
  glDepthFunc(GL_LESS);
  glStencilMask( (GLuint)~0 );
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

  m_camera.SetupMirroredModelview();

  // mark mirror area
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 0x80, 0x80);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  glDepthMask(GL_FALSE);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDisable(GL_LIGHTING);
  glColor4f(0,0,0,1);
  OutMirrorGeometry();


  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glColor4f(1.0,1.0,1.0,1.0);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  // out scene in mirror area(marked with seven bit in stencil)
  glEnable(GL_STENCIL_TEST);
  glDepthMask(GL_TRUE);
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
  glStencilFunc(GL_EQUAL, 0x80, 0x80);
  glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  OutScene();

  glDisable(GL_CULL_FACE);
  glDisable(GL_STENCIL_TEST);
  glStencilMask((GLuint)~0);
  glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 
  // return to staright camera
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  m_camera.Update();
  m_camera.Look();

  // draw mirror
  glDisable(GL_STENCIL_TEST);
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
  glEnable(GL_BLEND);
  // ... resColor = straightPixelColor * srcRGBfactor + mirroredPixelColor * dstRGBfactor
  // ... glBlendFuncSeparate( srcRGBfactor, dstRGBfactor, sAlpha, dAlpha )
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
  glBlendEquation(GL_FUNC_ADD);
  glColor4fv(MIRROR_COLOR);
  glDisable(GL_LIGHTING);

  mirrorShader->begin();
  OutMirrorTextured();
  defaultShader->begin();

  glDisable(GL_BLEND);
}


void Scene::OutObject(int objIndex, bool geometryOnly)
{
  if (meshes[objIndex].Name == MIRROR_OBJ_NAME)
    return;

  // setup necessary textures
  if (!geometryOnly)
  {
    defaultShader->begin();
    DrawObject(objIndex);
  }
  else
  {
    DrawObjectGeometry(objIndex);
  }
}

void Scene::OutCup( int objIndex )
{
  assert(cupShader != NULL);
  cupShader->begin();  

  cupShader->setUniform3f("LightPos", m_light.Position.x, m_light.Position.y, m_light.Position.z );
  DrawBumpmappedObject(objIndex);

  cupShader->end();
  defaultShader->begin();
}

void Scene::OutWireGauze( int objIndex )
{
  assert(wireGauzeShader!=NULL);
  wireGauzeShader->begin();

  DrawObject(objIndex);

  defaultShader->begin();
}

void Scene::OutBread( int objIndex )
{
  assert(ceramicShader!=NULL);
  ceramicShader->begin();

  DrawObject(objIndex);

  defaultShader->begin();
}

void Scene::OutSceneGeometry()
{
  // for each scene object
  for (int objIndex = 0; objIndex < MeshCount; objIndex++)
  {		
    OutObject(objIndex, true);		  // not only geometry: texturing is ON
  };
}
void Scene::OutScene()
{
  for (int objIndex = 0; objIndex < MeshCount; objIndex++)
  {		
    if (meshes[objIndex].Name == CUP_OBJ_NAME)
      OutCup(objIndex);
    else if (meshes[objIndex].Name == WIRE_OBJ_NAME)
      OutWireGauze(objIndex);
    else if (meshes[objIndex].Name == CERAMIC_OBJ_NAME)
      OutBread(objIndex);
    else
      OutObject(objIndex, false);		  // not only geometry: texturing is ON
  };

  // finally, mark light source position with cube
  m_light.Render();
}


int Scene::LoadMaterial (FILE *f)
{
  // ищем первую открывающую скобку
  fscanf(f,"%s",str);
  while (strcmp(str,"{")!=0) 
  {
    fscanf(f,"%s",str);
  };

  // количество открывающих скобок
  int LevelCount = 1;
  // пока не встретили такого же количества закрывающих скобок
  while (LevelCount > 0)
  {
    fscanf(f,"%s",str);
    if (strcmp(str,"{")==0) LevelCount++;
    if (strcmp(str,"}")==0) LevelCount--;

    // загрузить дифузную карту
    if (strcmp(str,"*MAP_DIFFUSE")==0)
    {
      fscanf(f,"%s",str);
      while (strcmp(str,"*MAP_CLASS")!=0) 
      {
        fscanf(f,"%s",str);
      };
      fscanf(f,"%s",str);
      // обычная дифузная карта
      if (strcmp(str,"\"Bitmap\"")==0)
      {
        fscanf(f,"%s",str);
        while (strcmp(str,"*BITMAP")!=0) 
        {
          fscanf(f,"%s",str);
        };				
        // имя текстуры
        fscanf(f,"%s",str);
        str[strlen(str)-1]=0;
        material[MaterialCount].DiffuseMap.CountMap=1;
        strcpy(material[MaterialCount].DiffuseMap.Tex1,str+1);
      }
      // миксованный материал
      if (strcmp(str,"\"Mix\"")==0)
      {
        material[MaterialCount].DiffuseMap.CountMap=3;

        fscanf(f,"%s",str);
        while (strcmp(str,"*BITMAP")!=0) 
        {
          fscanf(f,"%s",str);
        };				
        // имя текстуры
        fscanf(f,"%s",str);
        str[strlen(str)-1]=0;
        strcpy(material[MaterialCount].DiffuseMap.Tex1,str+1);

        fscanf(f,"%s",str);
        while (strcmp(str,"*BITMAP")!=0) 
        {
          fscanf(f,"%s",str);
        };				
        // имя текстуры
        fscanf(f,"%s",str);
        str[strlen(str)-1]=0;
        strcpy(material[MaterialCount].DiffuseMap.Tex2,str+1);

        fscanf(f,"%s",str);
        while (strcmp(str,"*BITMAP")!=0) 
        {
          fscanf(f,"%s",str);
        };				
        // имя текстуры
        fscanf(f,"%s",str);
        str[strlen(str)-1]=0;
        strcpy(material[MaterialCount].DiffuseMap.Tex3,str+1);

      }//if (strcmp(str,"\"Mix\"")==0)
          // миксованный материал
      if (strcmp(str,"\"Composite\"")==0)
      {
        material[MaterialCount].DiffuseMap.CountMap=2;

        fscanf(f,"%s",str);
        while (strcmp(str,"*BITMAP")!=0) 
        {
          fscanf(f,"%s",str);
        };				
        // имя текстуры
        fscanf(f,"%s",str);
        str[strlen(str)-1]=0;
        strcpy(material[MaterialCount].DiffuseMap.Tex1,str+1);

        fscanf(f,"%s",str);
        while (strcmp(str,"*BITMAP")!=0) 
        {
          fscanf(f,"%s",str);
        };				
        // имя текстуры
        fscanf(f,"%s",str);
        str[strlen(str)-1]=0;
        strcpy(material[MaterialCount].DiffuseMap.Tex2,str+1);

      }//if (strcmp(str,"\"Composite\"")==0)
    }//if (strcmp(str,"*MAP_DIFFUSE")==0)
  };//while

  fscanf(f,"%s",str);
  if (strcmp(str,"*MAP_BUMP")==0)
  {
    fscanf(f,"%s",str);
    while (strcmp(str,"*MAP_CLASS")!=0) 
    {
      fscanf(f,"%s",str);
    };
    fscanf(f,"%s",str);

    // бамп карта
    if (strcmp(str,"\"Bitmap\"")==0)
    {
      fscanf(f,"%s",str);
      while (strcmp(str,"*BITMAP")!=0) 
      {
        fscanf(f,"%s",str);
      };				
      // имя текстуры
      fscanf(f,"%s",str);
      str[strlen(str)-1]=0;
      material[MaterialCount].BumpMap.CountMap=1;
      strcpy(material[MaterialCount].BumpMap.Tex1,str+1);
    }
  }
  // загрузить карту прозрачности
  else if (strcmp(str,"*MAP_OPACITY")==0)
  {
    fscanf(f,"%s",str);
    while (strcmp(str,"*MAP_CLASS")!=0) 
    {
      fscanf(f,"%s",str);
    };
    fscanf(f,"%s",str);

    // обычная карта прозрачности
    if (strcmp(str,"\"Bitmap\"")==0)
    {
      fscanf(f,"%s",str);
      while (strcmp(str,"*BITMAP")!=0) 
      {
        fscanf(f,"%s",str);
      };				
      // имя текстуры
      fscanf(f,"%s",str);
      str[strlen(str)-1]=0;
      material[MaterialCount].OpacityMap.CountMap=1;
      strcpy(material[MaterialCount].OpacityMap.Tex1,str+1);
    }
  }
  MaterialCount++;

  return SUCCESS;
}

int Scene::LoadObject(FILE *f)
{
  // СЧИТЫВАНИЕ ИМЕНИ ОБЪЕКТА
  fscanf(f,"%s",str);
  while (strcmp(str,"*NODE_NAME")!=0) 
  {
    fscanf(f,"%s",str);
  };
  //meshes[MeshCount].Name=new
  char name[MAX_NAME_LENGTH];
  fscanf(f,"%s",&name);
  meshes[MeshCount].Name = dequote(string(name));

  // СЧИТЫВАНИЕ ПЕРВОЙ СТРОКИ МАТРИЦЫ
  fscanf(f,"%s",str);
  while (strcmp(str,"*TM_ROW0")!=0) 
  {
    fscanf(f,"%s",str);
  };
  fscanf(f,"%f %f %f",&meshes[MeshCount].Matrix[0],&meshes[MeshCount].Matrix[1],&meshes[MeshCount].Matrix[2]);

  // СЧИТЫВАНИЕ ВТОРОЙ СТРОКИ МАТРИЦЫ
  fscanf(f,"%s",str);
  while (strcmp(str,"*TM_ROW1")!=0) 
  {
    fscanf(f,"%s",str);
  };
  fscanf(f,"%f %f %f",&meshes[MeshCount].Matrix[4],&meshes[MeshCount].Matrix[5],&meshes[MeshCount].Matrix[6]);

  // СЧИТЫВАНИЕ ТРЕТЬЕЙ СТРОКИ МАТРИЦЫ
  fscanf(f,"%s",str);
  while (strcmp(str,"*TM_ROW2")!=0) 
  {
    fscanf(f,"%s",str);
  };
  fscanf(f,"%f %f %f",&meshes[MeshCount].Matrix[8],&meshes[MeshCount].Matrix[9],&meshes[MeshCount].Matrix[10]);

  // СЧИТЫВАНИЕ ЧЕТВЕРТОЙ СТРОКИ МАТРИЦЫ
  fscanf(f,"%s",str);
  while (strcmp(str,"*TM_ROW3")!=0) 
  {
    fscanf(f,"%s",str);
  };
  fscanf(f,"%f %f %f",&meshes[MeshCount].Matrix[12],&meshes[MeshCount].Matrix[13],&meshes[MeshCount].Matrix[14]);

  // СЧИТЫВАНИЯ КОЛИЧЕСТВО ВЕРТЕКСОВ
  fscanf(f,"%s",str);
  while (strcmp(str,"*MESH_NUMVERTEX")!=0) 
  {
    fscanf(f,"%s",str);
  };
  fscanf(f,"%i",&meshes[MeshCount].VertexCount);
  meshes[MeshCount].Vertex=new float[3*meshes[MeshCount].VertexCount];

  // CЧИТЫВАНИЕ КОЛИЧЕСТВА ИНДЕКСОВ
  fscanf(f,"%s",str);
  while (strcmp(str,"*MESH_NUMFACES")!=0) 
  {
    fscanf(f,"%s",str);
  };
  fscanf(f,"%i",&meshes[MeshCount].IndexCount);
  meshes[MeshCount].Index=new unsigned int[3*meshes[MeshCount].IndexCount];
  meshes[MeshCount].Normals=new float [9*meshes[MeshCount].IndexCount];

  // СЧИТЫВАНИЕ ВЕРШИН
  for (int i=0;i<meshes[MeshCount].VertexCount;i++)
  {
    fscanf(f,"%s",str);
    while (strcmp(str,"*MESH_VERTEX")!=0) 
    {
      fscanf(f,"%s",str);
    };
    int k;
    fscanf(f,"%i%f%f%f",&k,&meshes[MeshCount].Vertex[i*3+0],&meshes[MeshCount].Vertex[i*3+1],&meshes[MeshCount].Vertex[i*3+2]);
  }

  // СЧИТЫВАНИЕ ИНДЕКСОВ
  for (int i=0;i<meshes[MeshCount].IndexCount;i++)
  {
    fscanf(f,"%s",str);
    while (strcmp(str,"*MESH_FACE")!=0) 
    {
      fscanf(f,"%s",str);
    };
    int k;
    fscanf(f,"    %i:    A:    %i B:    %i C:    %i",&k,&meshes[MeshCount].Index[i*3+0],&meshes[MeshCount].Index[i*3+1],&meshes[MeshCount].Index[i*3+2]);
  };

  // СЧИТЫВАНИЕ КОЛИЧЕСТВА ТЕКСТУРНЫХ КООРДИНАТ
  fscanf(f,"%s",str);
  while (strcmp(str,"*MESH_NUMTVERTEX")!=0) 
  {
    fscanf(f,"%s",str);
  };
  fscanf(f,"%i",&meshes[MeshCount].TexCount);
  meshes[MeshCount].TexCord=new float[3*meshes[MeshCount].TexCount];

  // СЧИТЫВАНИЕ ТЕКСТУРНЫХ КООРДИНАТ
  for (int i=0;i<meshes[MeshCount].TexCount;i++)
  {
    fscanf(f,"%s",str);
    while (strcmp(str,"*MESH_TVERT")!=0) 
    {
      fscanf(f,"%s",str);
    };
    int k;
    fscanf(f,"%i%f%f%f",&k,&meshes[MeshCount].TexCord[i*3+0],&meshes[MeshCount].TexCord[i*3+1],&meshes[MeshCount].TexCord[i*3+2]);
  }

  // СЧИТЫВАНИЕ ИНДЕКСОВ ТЕКСТУРНЫХ КООРДИНАТ
  meshes[MeshCount].IndexTexCoord=new unsigned int[3*meshes[MeshCount].IndexCount];
  for (int i=0;i<meshes[MeshCount].IndexCount;i++)
  {
    fscanf(f,"%s",str);
    while (strcmp(str,"*MESH_TFACE")!=0) 
    {
      fscanf(f,"%s",str);
    };
    int k;
    fscanf(f,"%i%i%i%i",&k,&meshes[MeshCount].IndexTexCoord[i*3+0],&meshes[MeshCount].IndexTexCoord[i*3+1],&meshes[MeshCount].IndexTexCoord[i*3+2]);
  };


  // ЗАГРУЗКА НОРМАЛЕЙ
  for (int i=0;i<meshes[MeshCount].IndexCount*3;i++)
  {
    fscanf(f,"%s",str);
    while (strcmp(str,"*MESH_VERTEXNORMAL")!=0) 
    {
      fscanf(f,"%s",str);
    };
    int k;
    fscanf(f,"%i%f%f%f",&k,&meshes[MeshCount].Normals[i*3+0],&meshes[MeshCount].Normals[i*3+1],&meshes[MeshCount].Normals[i*3+2]);
    meshes[MeshCount].CalcNormal(meshes[MeshCount].Normals[i*3+0],meshes[MeshCount].Normals[i*3+1],meshes[MeshCount].Normals[i*3+2]);
  };

  // ЗАГРУЗКА ИНДЕКСА МАТЕРИАЛА
  fscanf(f,"%s",str);
  while (strcmp(str,"*MATERIAL_REF")!=0) 
  {
    fscanf(f,"%s",str);
  };
  fscanf(f,"%i",&meshes[MeshCount].MaterialId);

  // ПЕРЕСТРОЕНИЕ МОДЕЛИ
  meshes[MeshCount].Build();

  MeshCount++;
  return SUCCESS;
}

int Scene::LoadLightObject (FILE *f)
{
  // СЧИТЫВАНИЕ ЧЕТВЕРТОЙ СТРОКИ СТРОКИ МАТРИЦЫ - ПОЗИЦИЯ ИСТОЧНИКА СВЕТА
  fscanf(f,"%s",str);
  while (strcmp(str,"*TM_ROW3")!=0) 
  {
    fscanf(f,"%s",str);
  };
  //fscanf(f,"%f %f %f",&light.position[0],&light.position[1],&light.position[2]);
  fscanf(f,"%f %f %f", &light.Position[0],&light.Position[1],&light.Position[2]);

  // дифузный и зеркальный цвет источника света
  fscanf(f,"%s",str);
  while (strcmp(str,"*LIGHT_COLOR")!=0) 
  {
    fscanf(f,"%s",str);
  };
  //fscanf(f,"%f %f %f", &light.color[0], &light.color[1], &light.color[2]);
  fscanf(f,"%f %f %f", &light.Color[0], &light.Color[1], &light.Color[2]);

  return SUCCESS;
};

int Scene::LoadFromFile(const char *name)
{
  FILE *sceneFile = fopen(name, "rt");
  assert(sceneFile != NULL);
  if (sceneFile == NULL) 
    return FAILURE;

  while (fscanf(sceneFile,"%s",str)!=EOF)
  {
    if (strcmp(str,"*GEOMOBJECT")==0)
    {
      LoadObject(sceneFile);
    }
    if (strcmp(str,"*LIGHTOBJECT")==0)
    {
      LoadLightObject(sceneFile);
    }
    if (strcmp(str,"*MATERIAL")==0)
    {
      LoadMaterial(sceneFile);
    }
  }


  BuildMaterials();

  return SUCCESS;
}