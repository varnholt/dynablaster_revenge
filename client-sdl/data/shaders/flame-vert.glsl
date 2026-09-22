#version 300 es

// GLES3 port of client/data/shaders/flame-vert.glsl. gl_Vertex/gl_Normal/gl_ModelViewMatrix/
// gl_ProjectionMatrix (fixed-function built-ins) become explicit attributes/uniforms - see
// blockmaterial-vert.glsl's header comment for why u_modelView/u_projection are used the way they
// are here (u_modelView is the plain per-box world transform, u_projection already carries
// view*projection combined per this engine's own GLDevice::setCamera() convention).

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

uniform mat4 u_modelView;
uniform mat4 u_projection;
uniform vec3 campos;

out vec3 rayPosWorld;
out vec3 raypos;
out vec3 raydir;

void main()
{
   vec4 worldPos = u_modelView * vec4(a_position, 1.0);
   rayPosWorld = worldPos.xyz;
   raydir = worldPos.xyz - campos;
   raypos = a_normal;
   gl_Position = u_projection * worldPos;
}
