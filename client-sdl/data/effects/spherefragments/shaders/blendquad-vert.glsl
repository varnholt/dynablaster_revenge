#version 300 es

// GLES3 port of client/data/effects/spherefragments/shaders/blendquad-vert.glsl. The original
// writes gl_Position directly from the vertex position (scaled/offset) with no camera/projection
// matrix involved at all - preserved as-is, just with an explicit texcoord attribute instead of
// recomputing uv from position (FullScreenQuad's unit quad already stores matching 0..1 uvs).

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texcoord0;

uniform float scale;
uniform vec3 offset;

out vec2 uv;

void main()
{
   vec3 pos = a_position * scale;
   pos += offset;

   uv = a_texcoord0;
   gl_Position = vec4(pos, 1.0);
}
