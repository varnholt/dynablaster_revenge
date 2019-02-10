#version 120

varying vec2 currentPos;

void main()
{
   gl_Position = gl_Vertex;
   currentPos = gl_Vertex.xy * 0.5 + 0.5;
}
