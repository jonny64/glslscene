varying vec3 Normal;

void main()
{
  gl_TexCoord[0] = gl_MultiTexCoord0;
  Normal = normalize(gl_Normal);
  gl_Position = ftransform();
} 
