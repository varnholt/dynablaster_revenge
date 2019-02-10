#version 120

varying vec2 pos2d;

void main()
{
   gl_Position = gl_Vertex;
   pos2d= (gl_Vertex.xy + 1.0) * 0.5;
}
