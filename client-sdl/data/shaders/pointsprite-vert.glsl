#version 300 es

// Not from the original codebase. GameLogoPointSprite::draw() used a plain glBegin(GL_QUADS)
// immediate-mode textured quad per spark in the original (no shader at all) - GLES3 has no
// fixed-function fallback, so this is a minimal replacement.

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texcoord0;

uniform mat4 u_modelViewProjection;

out vec2 uv;

void main()
{
   uv = a_texcoord0;
   gl_Position = u_modelViewProjection * vec4(a_position, 1.0);
}
