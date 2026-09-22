#version 300 es

// GLES3 port of client/data/shaders/skullmaterial-vert.glsl - see blockmaterial-vert.glsl's
// header comment for why u_modelView/u_projection are used the way they are here.

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord0;

uniform vec3 camera;
uniform vec3 uvOffset;
uniform mat4 shadowCamera;
uniform mat4 u_modelView;
uniform mat4 u_projection;
uniform mat4 u_normalMatrix;

out vec3 eye;
out vec3 nrm;
out vec2 uv1;
out vec2 uv2;
out float faceShadow;

void main()
{
   vec4 pos = u_modelView * vec4(a_position, 1.0);
   vec3 n = mat3(u_normalMatrix) * a_normal;
   vec3 tmp = vec3(shadowCamera[0].z, shadowCamera[1].z, shadowCamera[2].z);

   // vertex-normal faces shadow camera?
   faceShadow = clamp(0.5 - dot(tmp, n), 0.0, 1.0);

   nrm = n;
   eye = normalize(mat3(u_normalMatrix) * (a_position - camera));
   vec4 shadowPos = shadowCamera * pos;

   uv1 = a_texcoord0;

   uv2 = vec2(shadowPos.x, -shadowPos.y) * 0.5 + 0.5;
   gl_Position = u_projection * pos;
}
