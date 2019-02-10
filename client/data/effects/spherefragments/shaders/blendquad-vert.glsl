#version 120

varying vec2 uv;

uniform float scale;
uniform vec3 offset;

void main()
{
   vec3 pos = gl_Vertex.xyz * scale;
   pos += offset;

   uv = gl_Vertex.xy * 0.5 + 0.5;
   gl_Position = vec4(pos, 1.0);
}
