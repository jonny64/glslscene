// texture unit # with shadowmap texture
uniform int shadowMapTexture;

varying vec3 Normal;
varying vec4 projCoord;
void main(void)
{
	vec4 realPos = gl_ModelViewMatrix * gl_Vertex; 
	projCoord = gl_TextureMatrix[shadowMapTexture] * gl_Vertex;
	
  	Normal = normalize(gl_Normal);
  	gl_TexCoord[0] = gl_MultiTexCoord0;
 	gl_Position = ftransform();
}
