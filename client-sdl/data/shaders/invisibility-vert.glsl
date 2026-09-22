#version 300 es

// GLES3 port of client/data/shaders/invisibility-vert.glsl - same skinning treatment as
// playermaterial-vert.glsl (see its header comment), plus a clip-space "pos" varying the
// fragment stage uses for a screen-space glass-refraction UV.

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord0;
layout(location = 3) in vec4 a_boneweights0;
layout(location = 4) in vec4 a_boneweights1;

uniform mat4 u_modelViewProjection;
uniform mat4 u_normalMatrix;
uniform vec3 camera;
uniform mat4 bones[8];

out vec3 eye;
out vec2 uv;
out vec3 nrm;
out vec4 v_pos;

void main()
{
   uv = a_texcoord0;

   // blend matrix
   mat4 mat = bones[0] * a_boneweights0.x;
   mat += bones[1] * a_boneweights0.y;
   mat += bones[2] * a_boneweights0.z;
   mat += bones[3] * a_boneweights0.w;
   mat += bones[4] * a_boneweights1.x;
   mat += bones[5] * a_boneweights1.y;
   mat += bones[6] * a_boneweights1.z;
   mat += bones[7] * a_boneweights1.w;

   vec4 vertex = vec4(a_position, 1.0);
   vec4 v;
   v.x = mat[0].x * vertex.x + mat[1].x * vertex.y + mat[2].x * vertex.z + mat[3].x * vertex.w;
   v.y = mat[0].y * vertex.x + mat[1].y * vertex.y + mat[2].y * vertex.z + mat[3].y * vertex.w;
   v.z = mat[0].z * vertex.x + mat[1].z * vertex.y + mat[2].z * vertex.z + mat[3].z * vertex.w;
   v.w = mat[0].w * vertex.x + mat[1].w * vertex.y + mat[2].w * vertex.z + mat[3].w * vertex.w;

   // todo: blend normal (matches the original's own unresolved todo, not fixed here)
   nrm = normalize(mat3(u_normalMatrix) * a_normal);

   eye = normalize(mat3(u_normalMatrix) * (v.xyz - camera));
   v_pos = u_modelViewProjection * v;

   gl_Position = v_pos;
}
