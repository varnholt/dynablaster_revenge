#version 300 es

// GLES3 port of client/data/shaders/texalphaignore-vert.glsl.
// ftransform()/gl_MultiTexCoord0/gl_Color (fixed-function built-ins) become explicit
// attributes/uniforms - GLDevice::push() uploads u_modelViewProjection per draw call, same as
// every other shader. The per-vertex color this used to carry (gl_Color, always
// glColor4f(1,1,1,1) at every real call site) contributed nothing and is dropped rather than
// wired through as a vertex attribute nobody varies.

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texcoord0;

uniform mat4 u_modelViewProjection;

out vec2 uv;

void main()
{
   uv = a_texcoord0;
   gl_Position = u_modelViewProjection * vec4(a_position, 1.0);
}
