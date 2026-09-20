#version 300 es

// GLES3 port of client/data/shaders/environment-vert.glsl.
// gl_Normal/gl_Vertex/gl_NormalMatrix/gl_ModelViewProjectionMatrix (fixed-function built-ins)
// become explicit attributes/uniforms - GLDevice::push() uploads u_modelViewProjection and
// u_normalMatrix per draw call (see GLDevice::uploadTransformUniforms()).

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

uniform mat4 u_modelViewProjection;
uniform mat4 u_normalMatrix;
uniform vec3 camera;

out vec3 eye;
out vec3 nrm;

void main()
{
   nrm = normalize(mat3(u_normalMatrix) * a_normal);
   eye = normalize(mat3(u_normalMatrix) * (a_position - camera));
   gl_Position = u_modelViewProjection * vec4(a_position, 1.0);
}
