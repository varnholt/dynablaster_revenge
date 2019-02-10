#version 120

//varying vec2 uv;
varying vec4 pos;

void main()
{
   pos= gl_ModelViewProjectionMatrix * gl_Vertex;
   gl_Position = pos;
//   uv= gl_MultiTexCoord0.xy;
}
