#pragma once

#include <string>
#include "GL/glew.h"
#include "Tangent.h"

using namespace std;

extern const GLuint TANGENT_GENERIC_ATTRIBUTE_INDEX;

namespace scene
{
  class Mesh
  {
    GLuint m_vertexVBO;
    GLuint m_indexVBO;
    GLuint m_normVBO;
    GLuint m_texVBO;
    GLuint m_tangentVBO;

  public:
    //��� �������
    string Name;

    // ������� ������������� �������
    float Matrix [16];

    // ���������� ������
    int VertexCount;
    // �������
    float *Vertex;

    // ���������� ��������
    int IndexCount;
    // �������
    unsigned int *Index;

    // ���������� ���������� ���������
    int TexCount;
    // ���������� ����������
    float *TexCord;
    // ������ �������� ���������� ���������
    unsigned int *IndexTexCoord;


    // �������
    float *Normals;

    // ������ ���������
    unsigned int MaterialId;

    Mesh()
    {
      glGenBuffersARB(1, &m_vertexVBO);
      glGenBuffersARB(1, &m_indexVBO);
      glGenBuffersARB(1, &m_normVBO);
      glGenBuffersARB(1, &m_texVBO);
      glGenBuffersARB(1, &m_tangentVBO);
    }

    ~Mesh()
    {
      glDeleteBuffersARB(1, &m_vertexVBO);
      glDeleteBuffersARB(1, &m_indexVBO);
      glDeleteBuffersARB(1, &m_normVBO);
      glDeleteBuffersARB(1, &m_texVBO);
      glDeleteBuffersARB(1, &m_tangentVBO);
    }

    // ������������� ������� � ������������ � �������� �������������
    // ������� ���������������� �� ����, ��� ��� ������������������
    void CalcNormal(float &f0,float &f1,float &f2)
    {
      float f_0=f0*Matrix[0]+f1*Matrix[4]+f2*Matrix[8];
      float f_1=f0*Matrix[1]+f1*Matrix[5]+f2*Matrix[9];
      float f_2=f0*Matrix[2]+f1*Matrix[6]+f2*Matrix[10];
      f0=f_0;
      f1=f_1;
      f2=f_2;
    };

    // �������������� ������ � ����� ������� �����
    void Build ();

    void Render();

    void RenderGeometry();
  };
}