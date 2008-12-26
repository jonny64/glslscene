#include "ShadowMap.h"

using namespace scene;

ShadowMap::ShadowMap()
{
  m_shadowMapID = -1;

  for (int i = 0; i < 3; i++)
    m_LightView[i] = 0.0f;

  //InitShadowTexture();
}

ShadowMap::~ShadowMap(void)
{
}

void ShadowMap::ApplyShadowMap(PFNRENDER OutScene, Scene* scene)
{
  glActiveTextureARB(GL_TEXTURE0_ARB + SHADOW_MAP_TEXTURE_INDEX);
  glEnable(GL_TEXTURE_2D); 
  glBindTexture(GL_TEXTURE_2D, m_shadowMapID);


  // Here is where we set the mode and function for shadow mapping with shadow2DProj().
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);

  glActiveTextureARB ( SHADOW_MAP_TEXTURE_INDEX + GL_TEXTURE0_ARB );
  // настраиваем текстурные координаты
  glMatrixMode  ( GL_TEXTURE );

  // Create our bias matrix to have a 0 to 1 ratio after clip space
  const float mBias[] = {0.5, 0.0, 0.0, 0.0, 
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0};

  glMatrixMode(GL_TEXTURE);
  glLoadMatrixf(mBias);			// The bias matrix to convert to a 0 to 1 ratio
  glMultMatrixf(m_projection);	// The light's projection matrix
  glMultMatrixf(m_modelView);	// The light's modelview matrix

  // Instead of using texture generation functions that handle the inverse
  // camera matrix for us, we calculated this ourselves and need to multiply
  // it by the preceding matrices.  Remember the equation for projection:
  // 
  // mProjectedTexture = mLightProjection * mLightModelview * mCameraInverse;
  //
  // We need to use this because we are projecting the shadow map onto the
  // world from the light's position.  A bit tricky to wrap your head around?
  // Keep in mind, the camera's inverse matrix is really the modelview matrix
  // inverted with the camera already applied to it.

  // Following the above equation, multiply our camera's inverse by the matrices.
  //glMultMatrixf(m_camera.GetInverse());

  glMatrixMode(GL_MODELVIEW);			// Switch back to normal modelview mode

  scene->OutScene();						// Render the world that needs to be shadowed

  // Now that the world is shadowed and we are done with the texture generation,
  // let's set everything back to normal by resetting the texture matrix.
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  // Turn the first multi-texture pass off
  glActiveTextureARB(GL_TEXTURE0_ARB + SHADOW_MAP_TEXTURE_INDEX);		
  glDisable(GL_TEXTURE_2D);
}



void ShadowMap::FlushShadowMap(PFNRENDERGEOMETRY OutSceneGeometry, Scene* scene)
{
  // Clear the screen and depth buffer so we can render from the light's view
  glClear(GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  // Set the current viewport to our texture size
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  GLint oldWidth = viewport[2];
  GLint oldHeight = viewport[3];
  glViewport(0, 0, (int)SHADOW_WIDTH, (int)SHADOW_HEIGHT);

  // Now we just need to set the matrices for the light before we render
  glMatrixMode(GL_PROJECTION);

  // Push on a matrix to make sure we can restore to the old matrix easily
  glPushMatrix();

  // Set the current projection matrix to our light's projection matrix
  glLoadMatrixf(m_projection);

  // Load modelview mode to set our light's modelview matrix
  glMatrixMode(GL_MODELVIEW);

  // Load the light's modelview matrix before we render to a texture
  glLoadMatrixf(m_modelView);

  // Since we don't care about color when rendering the depth values to
  // the shadow-map texture, we disable color writing to increase speed.
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); 

  // Eliminate artifacts caused by shadow mapping
  glPolygonOffset(8.0f, 4.0f);
  //glPolygonOffset(2.0f, 2.0f);


  // This turns of the polygon offset functionality to fix artifacts.
  // Comment this out and run the program to see what artifacts I mean.
  glEnable(GL_POLYGON_OFFSET_FILL);

  // Render the world according to the light's view
  scene->OutSceneGeometry();
  //OutScene();


  glActiveTextureARB ( SHADOW_MAP_TEXTURE_INDEX + GL_TEXTURE0_ARB);
  // Now that the world is rendered, save the depth values to a texture
  glBindTexture(GL_TEXTURE_2D, m_shadowMapID);
  glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, (int)SHADOW_WIDTH, (int)SHADOW_HEIGHT);

  // We can turn color writing back on since we already stored the depth values
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); 

  // Turn off polygon offsetting
  glDisable(GL_POLYGON_OFFSET_FILL);

  // Restore our normal viewport size to our screen width and height
  glViewport(0, 0, (int)oldWidth, (int)oldHeight);

  // Go back to the projection mode and restore the original matrix
  glMatrixMode(GL_PROJECTION);

  // Restore the original projection matrix
  glPopMatrix();

  // Go back to modelview model to start drawing like normal
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();			

  // Clear the color and depth bits and start over from the camera's view
  glClear(GL_DEPTH_BUFFER_BIT);							
  m_camera.Look();	
}


void ShadowMap::InitShadowTexture()
{
  // create and setup shadowmap texture
  glGenTextures   ( 1, &m_shadowMapID );
  glActiveTextureARB ( SHADOW_MAP_TEXTURE_INDEX + GL_TEXTURE0_ARB );
  glEnable(GL_TEXTURE_2D);
  glBindTexture   ( GL_TEXTURE_2D, m_shadowMapID );
  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

  glTexParameteri  ( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB );
  glTexParameteri  ( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL );
  glTexParameteri  ( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE );
  glTexGeni        ( GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
  glTexGeni        ( GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
  glTexGeni        ( GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
  glTexGeni        ( GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
  glTexImage2D     ( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0,
    GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL );
}


// copy z buf contents to texture at predefined TU
void ShadowMap::RenderZtoTexture()
{
  // копируем в текстуру содержимое Z-буфера
  glActiveTextureARB(SHADOW_MAP_TEXTURE_INDEX + GL_TEXTURE0_ARB);
  glEnable(GL_TEXTURE_2D);
  glBindTexture    ( GL_TEXTURE_2D, m_shadowMapID  );
  glCopyTexImage2D ( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, 
    SHADOW_WIDTH, SHADOW_HEIGHT, 0 );
}


//	This function positions our view from the light for shadow mapping
void ShadowMap::StoreLightMatrices(Vector3D lightPos)
{	
  // Reset our current light matrices
  memset(m_modelView, 0, sizeof(float)*16);
  memset(m_projection, 0, sizeof(float)*16);

  // Let's push on a new matrix so we don't change the rest of the world
  glPushMatrix();

  // Push on a new matrix to keep our view changes isolated
  glPushMatrix();

  // Reset the current modelview matrix
  glLoadIdentity();		

  // This is where we set the light's position and view.
  gluLookAt(
    lightPos.x,  
    lightPos.y,  
    lightPos.z, 
    m_LightView[0],	
    m_LightView[1],	  
    m_LightView[2],	
    0, 1, 0
    );

  // Now that we have the light's view, let's save the current modelview matrix.
  glGetFloatv(GL_MODELVIEW_MATRIX, m_modelView);

  // Now pop off the current light view's matrix
  glPopMatrix();

  // Reset the current matrix
  glLoadIdentity();

  // Set our FOV, aspect ratio, then near and far planes for the light's view
  gluPerspective  ( 120, 1, 0.1, 300 );

  // Grab the current matrix that will be used for the light's projection matrix
  glGetFloatv(GL_MODELVIEW_MATRIX, m_projection);

  // Go back to the original matrix
  glPopMatrix();
}