#version 300 es

// GLES3 port of client/data/shaders/destruction-vert.glsl - see blockmaterial-vert.glsl's header
// comment for why u_modelView/u_projection are used the way they are here. Faithfully keeps the
// original's unusual "eye = normalize(clip-space pos)" (computed AFTER the projection multiply,
// not a real eye-space vector) - not a porting bug, matches the original source exactly.

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord0;

uniform mat4 shadowCamera;
uniform mat4 u_modelView;
uniform mat4 u_projection;
uniform mat4 u_normalMatrix;

out vec3 eye;
out vec2 uv;
out vec3 nrm;
out vec2 uv3;
out float faceShadow;

void main()
{
   // vertex position in world space (see blockmaterial-vert.glsl)
   vec4 pos = u_modelView * vec4(a_position, 1.0);

   // normal in world space
   nrm = mat3(u_normalMatrix) * a_normal;

   // normalized z-axis of camera
   vec3 tmp = vec3(shadowCamera[0].z, shadowCamera[1].z, shadowCamera[2].z);

   // vertex-normal facing shadow camera? (1.0 = yes, 0.0 = no)
   faceShadow = clamp(0.5 - dot(tmp, nrm), 0.0, 1.0);

   vec4 shadowPos = shadowCamera * pos;

   pos = u_projection * pos;

   eye = normalize(pos.xyz);
   uv = a_texcoord0;
   uv3 = vec2(shadowPos.x, -shadowPos.y) * 0.5 + 0.5;
   gl_Position = pos;
}
