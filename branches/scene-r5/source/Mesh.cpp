#include "Mesh.h"

using namespace scene;

void Mesh::Build()
{
  float *vertex=new float[IndexCount*9];
  for (int i=0;i<IndexCount;i++)
  {
    int i1=Index[i*3+0];
    int i2=Index[i*3+1];
    int i3=Index[i*3+2];
    vertex[i*9+0]=Vertex[i1*3+0];
    vertex[i*9+1]=Vertex[i1*3+1];
    vertex[i*9+2]=Vertex[i1*3+2];
    vertex[i*9+3]=Vertex[i2*3+0];
    vertex[i*9+4]=Vertex[i2*3+1];
    vertex[i*9+5]=Vertex[i2*3+2];
    vertex[i*9+6]=Vertex[i3*3+0];
    vertex[i*9+7]=Vertex[i3*3+1];
    vertex[i*9+8]=Vertex[i3*3+2];
  }
  delete [] Vertex;
  Vertex=vertex;

  float *texcoord=new float[IndexCount*9];
  for (int i=0;i<IndexCount;i++)
  {
    int i1=IndexTexCoord[i*3+0];
    int i2=IndexTexCoord[i*3+1];
    int i3=IndexTexCoord[i*3+2];
    texcoord[i*9+0]=TexCord[i1*3+0];
    texcoord[i*9+1]=TexCord[i1*3+1];
    texcoord[i*9+2]=TexCord[i1*3+2];
    texcoord[i*9+3]=TexCord[i2*3+0];
    texcoord[i*9+4]=TexCord[i2*3+1];
    texcoord[i*9+5]=TexCord[i2*3+2];
    texcoord[i*9+6]=TexCord[i3*3+0];
    texcoord[i*9+7]=TexCord[i3*3+1];
    texcoord[i*9+8]=TexCord[i3*3+2];
  }
  delete [] TexCord;
  TexCord=texcoord;

  float* tangent = new float[ VertexCount * 4 ];
  CalculateTangentArray(
    VertexCount,
    Vertex, 
    Normals, 
    TexCord, 
    IndexCount, 
    Index, 
    tangent);

  GLsizeiptr vertexDataSize = sizeof(float)*(VertexCount)*18;//;18;
  GLsizeiptr indexDataSize = sizeof(unsigned int)*IndexCount*3;
  GLsizeiptr normDataSize = sizeof(float)*IndexCount*9;
  GLsizeiptr texDataSize = (sizeof(float)*TexCount)*16;//18;
  GLsizeiptr tangentDataSize = sizeof(tangent);

  // connect buffs id's to and write vertex data
  //... index data
  //... normals data
  //... texture data
  glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_vertexVBO );
  glBufferDataARB( GL_ARRAY_BUFFER_ARB, vertexDataSize, Vertex, GL_STATIC_DRAW_ARB );

  glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, m_indexVBO );
  glBufferDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, indexDataSize, Index, GL_STATIC_DRAW_ARB );

  glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_normVBO );
  glBufferDataARB( GL_ARRAY_BUFFER_ARB, normDataSize, Normals, GL_STATIC_DRAW_ARB );

  glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_texVBO );
  glBufferDataARB( GL_ARRAY_BUFFER_ARB, texDataSize, TexCord, GL_STATIC_DRAW_ARB );


  glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_tangentVBO );
  glBufferDataARB( GL_ARRAY_BUFFER_ARB, tangentDataSize, tangent, GL_STATIC_DRAW_ARB );

  delete[] tangent;

  //cleanup
  glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );
  glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, 0 );
};

void Mesh::Render()
{
  //material[MaterialId].Bind();

  glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, m_indexVBO );

  // transfer normals
  glEnableClientState(GL_NORMAL_ARRAY);
  glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_normVBO );
  glNormalPointer(GL_FLOAT,0,(GLvoid*)0);

  // transfer texture coordinates
  glClientActiveTextureARB(GL_TEXTURE0_ARB);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_texVBO );
  glTexCoordPointer(3,GL_FLOAT,0,(GLvoid*)0);

  glEnableClientState(GL_VERTEX_ARRAY);
  glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_vertexVBO );
  glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)0);

  // transfer additional attributes
  glEnableVertexAttribArrayARB( TANGENT_GENERIC_ATTRIBUTE_INDEX );
  //void glVertexAttribPointer(GLuint index,
  //GLint size,
  //GLenum type,
  //GLboolean normalized,
  //GLsizei stride,
  //const GLvoid *pointer)
  glVertexAttribPointerARB( TANGENT_GENERIC_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0 );

  glDrawArrays(GL_TRIANGLES, 0, IndexCount*3);

  // cleanupl
  glDisableVertexAttribArrayARB( TANGENT_GENERIC_ATTRIBUTE_INDEX );
  glDisableClientState( GL_VERTEX_ARRAY ); 
  glDisableClientState( GL_TEXTURE_COORD_ARRAY );
  glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );
  glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, 0 );	
}

void Mesh::RenderGeometry()
{
    glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, m_indexVBO);

  glEnableClientState(GL_VERTEX_ARRAY);
  glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_vertexVBO);
  glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)0);

  glDrawArrays(GL_TRIANGLES, 0, IndexCount*3);

  // cleanup
  glDisableClientState( GL_VERTEX_ARRAY ); 
  glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}