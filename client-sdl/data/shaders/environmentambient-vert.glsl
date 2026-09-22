#version 300 es

// GLES3 port of client/data/shaders/environmentambient-vert.glsl - identical to
// environmenttexture-vert.glsl (same original source, different fragment stage).

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord0;

uniform mat4 u_modelViewProjection;
uniform mat4 u_normalMatrix;
uniform vec3 camera;

out vec3 eye;
out vec2 uv;
out vec3 nrm;

void main()
{
   nrm = normalize(mat3(u_normalMatrix) * a_normal);
   eye = normalize(mat3(u_normalMatrix) * (a_position - camera));
   uv = a_texcoord0;
   gl_Position = u_modelViewProjection * vec4(a_position, 1.0);
}
