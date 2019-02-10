#version 120

varying vec4 color;

void main()
{
   // pass uv to fragment shader
   color= gl_MultiTexCoord0;

   // transform vertex to camera space
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
