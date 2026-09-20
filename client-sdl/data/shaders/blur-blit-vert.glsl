#version 300 es

// Not from the original codebase. BlurFilter::draw() (the final upscale blit) used a
// shader-less fixed-function textured quad in the original (activeDevice->setShader(0), the
// texture modulated by whatever glColor4f() alpha was current) - GLES3 has no fixed-function
// fallback, so this is a minimal dedicated replacement shader instead.

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texcoord0;

uniform mat4 u_modelViewProjection;

out vec2 uv;

void main()
{
   uv = a_texcoord0;
   gl_Position = u_modelViewProjection * vec4(a_position, 1.0);
}
