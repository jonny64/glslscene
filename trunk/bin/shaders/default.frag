// correspond to texture units which contains decal and shadowmap textures
uniform sampler2D tex;
uniform sampler2DShadow shadowMap;

// light position (vector-to-light) and color
uniform vec3 vecLight;
uniform vec3 lightColor;

varying vec4 projCoord;
varying vec3 Normal;

// shadow 'blackness' coef; purpose: avoiding 'totally black' shadows effect
const float SHADOW_BRIGHTNESS = 0.3;
// used for texture scaling ('rate', 'frequency')
const float TEXTURE_SCALE_FACTOR = 3.0;

void main()
{
	vec4 color = texture2D(tex, gl_TexCoord[0].st * TEXTURE_SCALE_FACTOR);
	color.rgb *= 0.5 * lightColor *  max(dot(Normal, vecLight), 0.0);
	
	float rValue = shadow2DProj(shadowMap, projCoord).r + SHADOW_BRIGHTNESS;
	rValue = clamp(rValue, 0.0, 1.0);

	vec3 coordPos  = projCoord.xyz / projCoord.w;
	if( (0.0 <= coordPos.x) && (coordPos.x <= 1.0)&&
		(0.0 <= coordPos.y) && (coordPos.y <= 1.0))
   	{
		color *= rValue;
	}
	
	gl_FragColor = color;
	// ensure passing alpha test
	gl_FragColor.a = 1.0;
}
