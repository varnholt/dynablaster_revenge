#version 300 es

// Sibling of texalphaignore-vert.glsl - see texalpha-frag.glsl for why this is a separate shader
// rather than reusing that one.

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texcoord0;

uniform mat4 u_modelViewProjection;

out vec2 uv;

void main()
{
   uv = a_texcoord0;
   gl_Position = u_modelViewProjection * vec4(a_position, 1.0);
}
