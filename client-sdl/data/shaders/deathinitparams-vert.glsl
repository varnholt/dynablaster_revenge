#version 300 es

// GLES3 port of client/data/shaders/deathinitparams-vert.glsl - same fullscreen-quad shape as
// deathinitpositions-vert.glsl.

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texCoord;

out vec2 pos;

void main()
{
   gl_Position = vec4(a_position, 0.0, 1.0);
   pos = a_texCoord;
}
