#version 300 es
precision mediump float;

// GLES3 port of client/data/shaders/texturemapping-frag.glsl (texture2D -> texture, gl_FragColor
// -> an explicit out).

uniform sampler2D texturemap;

in vec2 uv;
out vec4 o_color;

void main()
{
   o_color = texture(texturemap, uv);
}
