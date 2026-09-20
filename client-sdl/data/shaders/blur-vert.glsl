#version 300 es

// GLES3 port of client/data/shaders/blur-vert.glsl. ftransform() (fixed-function P*MV using
// whatever the current matrix stack held) becomes u_modelViewProjection - BlurFilter always draws
// under an explicit pixel-space ortho projection with an identity world transform (see
// BlurFilter::downSamplePass()/gaussPass()), matching what ftransform() gave the original.

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texcoord0;

uniform mat4 u_modelViewProjection;

out vec2 uv;

void main()
{
   uv = a_texcoord0;
   gl_Position = u_modelViewProjection * vec4(a_position, 1.0);
}
