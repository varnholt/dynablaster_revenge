#version 300 es
precision mediump float;

// GLES3 port of client/data/effects/spherefragments/shaders/blendquad-frag.glsl
// (texture2D -> texture, varying -> in, gl_FragColor -> an explicit out).

uniform sampler2D texturemap;
uniform vec4 color;

in vec2 uv;

out vec4 o_color;

void main()
{
   o_color = texture(texturemap, uv) * color;
}
