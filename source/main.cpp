#include "main.h"

//-----------------------------------------------------------------------------
using namespace scene;

class myApplication : public cwc::glApplication
{
public:
  virtual void OnInit() {cout << "Hello World!\n"; }
  virtual void OnExit() {cout << "Bye World\n"; }
};

class myWindow : public cwc::glutWindow
{
  // scene
  Scene m_scene;

  void GetViewportSize(GLint *centerX, GLint *centerY)
  {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    *centerX = viewport[2];
    *centerY = viewport[3];
  }
  
public:
  myWindow()
  {
  }


  virtual void OnRender(void)
  {
    m_scene.Render();
  }


  virtual void OnIdle()
  {
    //cwc::psleep(1);
  }


  virtual void OnInit()
  {
    //SetFullscreen(true);

    m_scene.Init();

    glClearColor(0.5f, 0.5f, 1.0f, 0.0f);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    GLint width, height;
    GetViewportSize(&width, &height);
    
    // Position cursor to capture latter moves
    int middleX = width >> 1;				// This is a binary shift to get half the width
    int middleY = height >> 1;				// This is a binary shift to get half the height

    // Set the mouse position to the middle of our window
    glutWarpPointer(middleX, middleY);	
    // Hide cursor 
    glutSetCursor(GLUT_CURSOR_NONE);

    // finally, setup camera
    m_camera.PositionCamera(
      10, 10, 10,
      0, 0, 0,
      0, 1, 0
      );
    m_shadowMap.InitShadowTexture();
  }

  virtual void OnResize(int w, int h)
  {
    // set view port to complete window area
    glViewport(0,0,(GLsizei)w, (GLsizei)h);

    // setup projection matrix (with correct aspect w/h)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0,(float)w/h, 0.2, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  virtual void OnClose(void)
  {
  }

  virtual void OnMouseWheel(int nWheelNumber, int nDirection, int x, int y)
  { 
    switch(nDirection)
    {
    case 1:
      // up
      m_camera.MoveCamera(STRAFE_SENSIVITY);
      break;
    case -1:
      // down
      m_camera.MoveCamera(-STRAFE_SENSIVITY);
      break;
    }
    glutPostRedisplay();
  }

  virtual void OnMouseMove(int x, int y)
  {
    GLint width = 0;
    GLint height = 0;
    GetViewportSize(&width, &height);

    int middleX = width  >> 1;				// This is a binary shift to get half the width
    int middleY = height >> 1;				// This is a binary shift to get half the height

    // If our cursor is still in the middle, we never moved... so don't update the screen
    if( (x == middleX) && (y == middleY) ) return;

    // Set the mouse position to the middle of our window
    glutWarpPointer(middleX, middleY);							

    m_camera.SetViewByMouse(
      (middleX - x)/MOUSE_STRAFE_SENSIVITY,
      (middleY - y)/MOUSE_STRAFE_SENSIVITY
      );

    m_camera.Look();
    glutPostRedisplay();
  }

  virtual void OnKeyDown(int nKey, char cAscii)
  {       
    static bool bFullscreen = false;

    switch (cAscii)
    {
    case 27:
      //esc
      Close();
      return;

    case 13:
      // enter
      bFullscreen = !bFullscreen;
      SetFullscreen(bFullscreen);
      break;

    case 'D': 
    case 'd': 
      m_camera.StrafeCamera(+STRAFE_SENSIVITY); 
      break;

    case 'A': 
    case 'a': 
      m_camera.StrafeCamera(-STRAFE_SENSIVITY);
      break;

    case 'W':
    case 'w':
    case '=':
    case '+':
      m_camera.MoveCamera(+STRAFE_SENSIVITY);
      break;

    case 'S': 
    case 's': 
    case '-': 
      m_camera.MoveCamera(-STRAFE_SENSIVITY);
      break;

    default:;
    };

    const float LIGHT_MOVE_PITCH = 0.2f;
    switch (nKey)
    {
    case 104: // PgUp
      //m_light.Position.y += LIGHT_MOVE_PITCH;
      break;

    case 105: // PgDn
      //m_light.Position.y -= LIGHT_MOVE_PITCH;
      break;
    default:
      break;
    }

    glutPostRedisplay();
  };

  virtual void OnKeyUp(int nKey, char cAscii)
  {
  };
};

//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
#ifdef FREEIMAGE_LIB
  FreeImage_Initialise(); // only call if freeimage is linked statically
#endif

  myApplication*  pApp = new myApplication;
  myWindow* myWin = new myWindow();

  pApp->run();
  delete pApp;

#ifdef FREEIMAGE_LIB 
  FreeImage_DeInitialise(); // only call if freeimage is linked statically
#endif   
  return 0; 
}

//-----------------------------------------------------------------------------

