#version 300 es
precision mediump float;

// GLES3 port of client/data/effects/spherefragments/shaders/duplicatealpha-frag.glsl
// (texture2D -> texture, varying -> in, gl_FragColor -> an explicit out).

in vec2 uv;

uniform vec4 color;
uniform sampler2D texturemap;

out vec4 o_color;

void main()
{
   float alpha = texture(texturemap, uv).a;
   o_color = color * alpha;
}
