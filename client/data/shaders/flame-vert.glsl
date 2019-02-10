#version 120

varying vec3 rayPosWorld; // ray origin position in world space
varying vec3 raypos;      // ray origin position in bounding box space (which is just a 2d offset to worldPos)
varying vec3 raydir;      // ray direction in bounding box space
uniform vec3 campos;      // camera position in world space

void main()
{
   vec4 worldPos = gl_ModelViewMatrix * gl_Vertex;
   rayPosWorld= worldPos.xyz;
   raydir = worldPos.xyz - campos;
   raypos =  gl_Normal.xyz;
   gl_Position = gl_ProjectionMatrix * worldPos;
}
