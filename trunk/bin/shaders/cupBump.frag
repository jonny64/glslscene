// decal and normalmap textures
uniform sampler2D DecalTex;
uniform sampler2D BumpTex;

uniform vec3 LightColor;

varying vec3 LightDir;
varying vec3 EyeDir;

// used for texture coordinate scaling ('rate', 'frequency')
const float TEXTURE_SCALE_FACTOR  = 3.0;

#define SPECULAR

void main()
{
    vec3 decal = texture2D(DecalTex, gl_TexCoord[0].st * TEXTURE_SCALE_FACTOR).xyz;
    vec3 normal = texture2D(BumpTex, gl_TexCoord[0].st * TEXTURE_SCALE_FACTOR).xyz;
    // extract: [0..1]-> [-1 1]
    normal = normalize(normal * 2.0 - 1.0); 
    
    vec3 litColor = decal * max(dot(normal, LightDir), 0.0);
#ifdef SPECULAR
    const float SpecularFactor = 0.4;
    vec3 reflectDir = reflect(LightDir, normal);
    float spec = max(dot(EyeDir, reflectDir), 0.0);
    spec = pow(spec, 6.0);
    spec *= SpecularFactor;
    litColor = min(litColor + spec, vec3(1.0));
#else
    litColor = min(litColor, vec3(1.0));
#endif
	gl_FragColor = vec4(litColor, 1.0);
}
