#version 120

uniform mat4 shadowCamera;

// output
varying vec3 eye;
varying vec2 uv;
varying vec3 nrm;
varying vec2 uv3;
varying float faceShadow;

void main()
{
   // vertex position in camera space
   vec4 pos= gl_ModelViewMatrix * gl_Vertex;

   // normal in world space
   nrm= gl_NormalMatrix * gl_Normal;

   // normalized z-axis of camera
   vec3 tmp= vec3( shadowCamera[0].z, shadowCamera[1].z, shadowCamera[2].z );

   // vertex-normal facing shadow camera? (1.0 = yes, 0.0 = no)
   faceShadow= clamp(0.5-dot(tmp, nrm), 0.0, 1.0);

   vec4 shadowPos= shadowCamera * pos;

   pos= gl_ProjectionMatrix * pos;

   eye= normalize(pos.xyz);
   uv = gl_MultiTexCoord0.xy;
   uv3= vec2(shadowPos.x, -shadowPos.y)*0.5+0.5;
   gl_Position = pos;
}
