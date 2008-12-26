// corresponed to texture units which contains decal and opacity textures
uniform sampler2D tex;
uniform sampler2D opacityTex;

uniform vec3 vecLight;

varying vec3 Normal;

// used for texture coordinate scaling ('rate', 'frequency')
const float TEXTURE_SCALE_FACTOR  = 3.0;

void main()
{
    vec4 color = texture2D(tex, gl_TexCoord[0].st * TEXTURE_SCALE_FACTOR );

	// calculate diffuse
    color *= 0.25 * max(dot(Normal, vecLight), 0.0);
    gl_FragColor = color;
    
    // use alpha value from opacity texture 'red' component
    vec4 opacityColor = texture2D(opacityTex, gl_TexCoord[0].st * TEXTURE_SCALE_FACTOR );
    gl_FragColor.a = opacityColor.r;
}