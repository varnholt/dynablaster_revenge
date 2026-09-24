#version 300 es

// GLES3 port of client/data/shaders/deathinitpositions-vert.glsl. Draws a fullscreen quad whose
// per-vertex texcoord carries the bounding-rect-relative UV corners passed in by
// DeathFlowFieldAnimation::initializePositions() (gl_MultiTexCoord0 in the original).

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texCoord;

out vec2 pos;

void main()
{
   gl_Position = vec4(a_position, 0.0, 1.0);
   pos = a_texCoord;
}
