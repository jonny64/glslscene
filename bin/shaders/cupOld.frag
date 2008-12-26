uniform sampler2D tex;
uniform sampler2D bumpTex;

uniform vec3 LightPos;
uniform vec4 ambientColor;
uniform sampler2DShadow shadowMap;

varying vec3 LightDir;
varying vec4 projCoord;
varying vec3 Normal;

const vec2 texCordScale = vec2(10.0, 10.0);
const float kTransparency = 0.3;

void ApplyShadow();

void main()
{
  //Get the color of the bump-map
  vec3 BumpNorm = texture2D(bumpTex, gl_TexCoord[0].st * texCordScale).xyz;
  //Expand the bump-map into a normalized signed vector
  BumpNorm = normalize(BumpNorm * 2.0 - 1.0);
  LightDir = normalize(LightDir);
  //Get the color of the texture
  vec3 DecalCol = texture2D(tex, gl_TexCoord[0].st * texCordScale).rgb;
  //Find the dot product between the light direction and the normal
  float NdotL = max(dot(BumpNorm, LightDir), 0.0);
  //float NdotL = dot(BumpNorm, LightDir);
  //Calculate the final color
  vec3 diffuse =  NdotL * DecalCol;
  //Set the color of the fragment...  If you want specular lighting or other types add it here
  //vec3 testBumpNorm = texture2D(bumpTex, gl_TexCoord[0].st * texCordScale).rgb;
  gl_FragColor = vec4(diffuse, 1);

  // multiply color by shadow map value
  //ApplyShadow();
	
  // ensure passing alpha test
  gl_FragColor.a = 1.0;
}

void ApplyShadow()
{
  float rValue = shadow2DProj(shadowMap, projCoord).r + kTransparency;
  rValue = clamp(rValue, 0.0, 1.0);
  vec3 coordPos  = projCoord.xyz / projCoord.w;
  if( (0.0 <= coordPos.x) && (coordPos.x <= 1.0)&&
  	(0.0 <= coordPos.y) && (coordPos.y <= 1.0))
   {
     gl_FragColor *= rValue;
   }
}
