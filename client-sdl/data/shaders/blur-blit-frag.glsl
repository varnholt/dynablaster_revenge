#version 300 es
precision mediump float;

// See blur-blit-vert.glsl - not from the original codebase.

in vec2 uv;

uniform sampler2D texture0;
uniform float alpha;

out vec4 o_color;

void main()
{
   o_color = texture(texture0, uv) * vec4(1.0, 1.0, 1.0, alpha);
}
