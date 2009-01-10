uniform int shadowMapTexture;
uniform vec3 LightPos;

varying vec3 Normal;
varying vec4 passcolor;
varying vec3 LightDir;
varying vec4 projCoord;

vec3 VertexSpaceLightDir(vec3 objSpaceLightDir);

void main(void)
{

  vec4 realPos = gl_ModelViewMatrix * gl_Vertex;
  projCoord = gl_TextureMatrix[shadowMapTexture] * gl_Vertex;//realPos;
  Normal = gl_Normal;
  
  vec3 n = normalize(gl_NormalMatrix * gl_Normal);
  vec3 t = normalize(gl_NormalMatrix * gl_MultiTexCoord1.xyz);
  vec3 b = cross(n, t) * gl_MultiTexCoord1.w;
  mat3 tbnMatrix = mat3(t.x, b.x, n.x,
                          t.y, b.y, n.y,
                          t.z, b.z, n.z);
    
  LightDir = tbnMatrix * LightPos.xyz;
  
  // use the first set of texture coordinates in the fragment shader 
  gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
  // put the vertex in the position passed
  gl_Position = ftransform(); 
}
