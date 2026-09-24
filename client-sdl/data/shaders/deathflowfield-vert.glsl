#version 300 es

// GLES3 port of client/data/shaders/deathflowfield-vert.glsl - a fullscreen-quad pass (position
// already in NDC), gl_Vertex becomes an explicit attribute.

layout(location = 0) in vec2 a_position;

out vec2 currentPos;

void main()
{
   gl_Position = vec4(a_position, 0.0, 1.0);
   currentPos = a_position * 0.5 + 0.5;
}
