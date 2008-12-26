// texture unit # with shadowmap texture
uniform int shadowMapTexture;

varying vec3 Normal;
varying vec4 projCoord;
void main(void)
{
	projCoord = gl_TextureMatrix[shadowMapTexture] * gl_Vertex;
	
  	Normal = normalize(gl_Normal);
  	gl_TexCoord[0] = gl_MultiTexCoord0;
 	gl_Position = ftransform();
}
