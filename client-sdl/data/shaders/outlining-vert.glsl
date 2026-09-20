#version 300 es

// GLES3 port of client/data/shaders/outlining-vert.glsl. gl_Color (fixed-function per-vertex
// color, set via the old glColor4f) becomes an explicit u_color uniform read directly in the
// fragment shader instead, since it never varies per-vertex here.

layout(location = 0) in vec3 a_position;

uniform mat4 u_modelViewProjection;

void main()
{
   vec4 pos = u_modelViewProjection * vec4(a_position, 1.0);
   pos.w *= 1.00001; // nudge outline geometry behind the surface it traces, same as upstream
   gl_Position = pos;
}
