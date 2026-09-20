#version 300 es

// GLES3 port of client/data/effects/spherefragments/shaders/duplicatealpha-vert.glsl. Like
// blendquad-vert.glsl, the original writes gl_Position straight from the vertex position with no
// camera/projection transform at all.

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texcoord0;

out vec2 uv;

void main()
{
   uv = a_texcoord0;
   gl_Position = vec4(a_position, 1.0);
}
