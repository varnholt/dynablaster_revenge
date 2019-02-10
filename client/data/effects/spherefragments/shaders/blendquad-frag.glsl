#version 120

uniform sampler2D texturemap;
uniform vec4 color;

varying vec2 uv;

void main()
{
   gl_FragColor= texture2D(texturemap, uv) * color;
}

