//Light position in world space coordinates
uniform vec3 LightPos;

// Light position in tangent space coordinates
varying vec3 LightDir;
// Camera vector (used to calculate specular in fragment program)
varying vec3 EyeDir;

attribute vec4 Tangent;

void main()
{
    EyeDir = vec3(gl_ModelViewMatrix * gl_Vertex);

    gl_TexCoord[0] = gl_MultiTexCoord0;

    vec3 n = normalize(gl_Normal).xyz;
    vec3 t = normalize(Tangent.xyz).xyz;
    vec3 b = cross(n, t);
    mat3 tbnMatrix = mat3(t.x, b.x, n.x,
	                      t.y, b.y, n.y,
	                      t.z, b.z, n.z);
	                      
    LightDir = tbnMatrix * (LightPos);
    LightDir = normalize(LightDir);

    EyeDir = tbnMatrix * EyeDir;
    EyeDir = normalize(EyeDir);
    
    gl_Position    = ftransform();
}
