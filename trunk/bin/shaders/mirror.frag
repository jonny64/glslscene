// correspond to texture units which contains decal and shadowmap textures
uniform sampler2D tex;
uniform sampler2DShadow shadowMap;

varying vec4 projCoord;

const float MIRROR_TURBIDITY = 0.8;
const float SHADOW_BRIGHTNESS = 0.3;
const vec3 MIRROR_COLOR = vec3(0.2, 0.8, 1.0);

// used for texture coordinate scaling ('rate', 'frequency')
const float TEXTURE_SCALE_FACTOR  = 2.25;

void ApplyShadow();

void main()
{
  vec4 decalColor = texture2D(tex, gl_TexCoord[0].st * TEXTURE_SCALE_FACTOR );
  gl_FragColor.rgb = decalColor.rgb * MIRROR_COLOR;

  ApplyShadow();
  
  // to combine mirror color and mirrored scene pixel color i use blending mechanizm 
  // (applies at last conveyor stage and involves mirror alpha) 
  // so i need setup it here
  gl_FragColor.a = MIRROR_TURBIDITY;
}

void ApplyShadow()
{
	float rValue = shadow2DProj(shadowMap, projCoord).r + SHADOW_BRIGHTNESS;
	rValue = clamp(rValue, 0.0, 1.0);
  
	vec3 coordPos  = projCoord.xyz / projCoord.w;
	if( (0.0 <= coordPos.x) && (coordPos.x <= 1.0)&&
		(0.0 <= coordPos.y) && (coordPos.y <= 1.0))
    {
		gl_FragColor.rgb *= rValue;
	}
}