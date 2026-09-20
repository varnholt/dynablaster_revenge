#version 300 es
precision mediump float;

// See pointsprite-vert.glsl - not from the original codebase.

in vec2 uv;

uniform sampler2D texturemap;

out vec4 o_color;

void main()
{
   o_color = texture(texturemap, uv);
}
