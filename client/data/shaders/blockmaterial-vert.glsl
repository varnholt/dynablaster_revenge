#version 120

// shader constants
uniform vec3 camera;
uniform vec3 uvOffset;
uniform mat4 shadowCamera;

// output
varying vec3 eye;
varying vec3 nrm;
varying vec2 uv1;
varying vec2 uv2;
varying vec2 uv3;
varying float faceShadow;

const float texWidth= 192.0;
const float texHeight= 256.0;
const float texPadding= 4.0;

void main()
{
   vec4 pos= gl_ModelViewMatrix * gl_Vertex;
   vec3 n= gl_NormalMatrix * gl_Normal;
   vec3 tmp= vec3( shadowCamera[0].z, shadowCamera[1].z, shadowCamera[2].z );

   // vertex-normal faces shadow camera?
   faceShadow= clamp(0.5-dot(tmp, n), 0.0, 1.0);

   nrm= n;
   eye= normalize(gl_NormalMatrix * (gl_Vertex.xyz - camera));
   vec4 shadowPos= shadowCamera * pos;

   uv1 = gl_MultiTexCoord0.xy;
   // compensate padding between individual uv sets:
   uv2 = vec2(  (gl_MultiTexCoord0.x * texWidth + uvOffset.x * (texWidth + texPadding) + 1) / 1024.0,
                (gl_MultiTexCoord0.y * texHeight + uvOffset.y * (texHeight + texPadding) + 1) / 2048.0 );

   uv3= vec2(shadowPos.x, -shadowPos.y)*0.5+0.5;
   gl_Position = gl_ProjectionMatrix * pos;
}
