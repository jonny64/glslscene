#pragma once
#include "Vector4D.h"
#include <memory.h>
#include "GL/glew.h"

const GLuint TANGENT_GENERIC_ATTRIBUTE_INDEX = 1;

struct Triangle
{
    unsigned int index[3];
};

void CalculateTangentArray(long vertexCount, const float *vertices, const float *normals,
        const float *texcoords, long triangleCount, const unsigned int *triangles, float *tangent);