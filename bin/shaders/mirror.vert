// shadow map texture unit ID
uniform int shadowMapTexture;

varying vec4 projCoord;

void main()
{
	projCoord = gl_TextureMatrix[shadowMapTexture] * gl_Vertex;
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
} 
