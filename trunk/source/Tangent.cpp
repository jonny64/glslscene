#include "Tangent.h"

const float EPSILON = 1e-6f;
static bool CloseEnough(float f1, float f2)
{
  // Determines whether the two floating-point values f1 and f2 are
  // close enough together that they can be considered equal.

  return fabsf((f1 - f2) / ((f2 == 0.0f) ? 1.0f : f2)) < EPSILON;
}


void CalculateTangentArray(long vertexCount, const float *vertices, const float *normals,
                           const float *texcoords, long triangleCount, const unsigned int *triangles, float *tangents)
{
  Vector3D *tan1 = new Vector3D[vertexCount * 2];
  Vector3D *tan2 = tan1 + vertexCount;
  memset(tan1, 0, vertexCount * 3 * sizeof(float) * 2);

  Triangle* triangle = (Triangle*)triangles;
  Point3D* vertex = (Point3D*)vertices;
  Point2D* texcoord = (Point2D*)texcoords;
  Vector3D* normal = (Vector3D*)normals;
  Vector4D* tangent = new Vector4D[vertexCount];

  for (long i = 0; i < triangleCount; i++)
  {
    long i1 = triangle->index[0];
    long i2 = triangle->index[1];
    long i3 = triangle->index[2];

    const Point3D& v1 = vertex[i1];
    const Point3D& v2 = vertex[i2];
    const Point3D& v3 = vertex[i3];

    const Point2D& w1 = texcoord[i1];
    const Point2D& w2 = texcoord[i2];
    const Point2D& w3 = texcoord[i3];

    // Create 2 vectors in object space (lays on triangle edges)
    // edge1(x1, y1, z1) and edge2(x2, y2, z2)
    Vector3D edge1(v2 - v1);
    Vector3D edge2(v3 - v1);

    // Create 2 vectors in tangent (texture) space that point in the same
    // direction as edge1 and edge2 (in object space)
    // texEdge1(s1, t1) and texEdge2(s2, t2)
    Vector2D texEdge1(w2 - w1);
    Vector2D texEdge2(w3 - w1);

    // These 2 sets of vectors form the following system of equations:
    //
    //  edge1 = (texEdge1.x * tangent) + (texEdge1.y * bitangent)
    //  edge2 = (texEdge2.x * tangent) + (texEdge2.y * bitangent)
    //
    // Using matrix notation this system looks like:
    //
    //  [ edge1 ]     [ texEdge1.x  texEdge1.y ]  [ tangent   ]
    //  [       ]  =  [                        ]  [           ]
    //  [ edge2 ]     [ texEdge2.x  texEdge2.y ]  [ bitangent ]
    //
    // The solution is:
    //
    //  [ tangent   ]        1     [ texEdge2.y  -texEdge1.y ]  [ edge1 ]
    //  [           ]  =  -------  [                         ]  [       ]
    //  [ bitangent ]      det A   [-texEdge2.x   texEdge1.x ]  [ edge2 ]
    //
    //  where:
    //        [ texEdge1.x  texEdge1.y ]
    //    A = [                        ]
    //        [ texEdge2.x  texEdge2.y ]
    //
    //    det A = (texEdge1.x * texEdge2.y) - (texEdge1.y * texEdge2.x)
    //
    // From this solution the tangent space basis vectors are:
    //
    //    tangent = (1 / det A) * ( texEdge2.y * edge1 - texEdge1.y * edge2)
    //  bitangent = (1 / det A) * (-texEdge2.x * edge1 + texEdge1.x * edge2)
    //     normal = cross(tangent, bitangent)

    float k = 1.0f / ( (texEdge1.x * texEdge2.y) - (texEdge1.y * texEdge2.x) );
    Vector3D sdir(k * ( texEdge2.y * edge1 - texEdge1.y * edge2) );
    Vector3D tdir(k * (-texEdge2.x * edge1 + texEdge1.x * edge2) );

    tan1[i1] = sdir;
    tan1[i2] = sdir;
    tan1[i3] = sdir;

    tan2[i1] = tdir;
    tan2[i2] = tdir;
    tan2[i3] = tdir;

    triangle++;
  }

  for (long i = 0; i < vertexCount; i++)
  {
    const Vector3D& n = normal[i];
    const Vector3D& t = tan1[i];

    // Gram-Schmidt orthogonalize
    //tangent[i] = (t - n * Dot(n, t)).Normalize();
    tangent[i].Set(t, 0);

    // Calculate handedness
    tangent[i].w = (Dot(Cross(n, t), tan2[i]) < 0.0F) ? -1.0F : 1.0F;
  }

  delete[] tan1;

  for (long i = 0; i < vertexCount; i++)
  {
    tangents[i*4 + 0] = tangent->x;
    tangents[i*4 + 1] = tangent->y;
    tangents[i*4 + 2] = tangent->z;
    tangents[i*4 + 3] = tangent->w;
  }
  delete[] tangent;
}