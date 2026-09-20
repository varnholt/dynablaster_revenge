#version 300 es

// GLES3 port of client/data/shaders/fontoutlines-vert.glsl.
// ftransform()/gl_MultiTexCoord0 (fixed-function built-ins) become explicit
// attributes/uniforms - GLDevice::push() uploads u_modelViewProjection per draw call. gl_Color
// (varied per glyph quad in the legacy code, but always set to one constant color for an entire
// buildVertices()+draw() call - see BitmapFont::setColor()) becomes a uniform in the fragment
// shader instead of a varying here.

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texcoord0;

uniform mat4 u_modelViewProjection;

out vec2 uv;

void main()
{
   uv = a_texcoord0;
   gl_Position = u_modelViewProjection * vec4(a_position, 0.0, 1.0);
}
