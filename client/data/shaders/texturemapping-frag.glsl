#version 120

uniform sampler2D   texturemap;

// input
varying vec2 uv;

void main()
{
   vec4 col= texture2D(texturemap, uv);

   gl_FragColor = col;
}
