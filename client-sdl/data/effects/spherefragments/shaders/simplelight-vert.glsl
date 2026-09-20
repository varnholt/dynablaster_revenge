#version 300 es

// GLES3 port of client/data/effects/spherefragments/shaders/simplelight-vert.glsl.
// gl_ModelViewMatrix / gl_ProjectionMatrix / gl_NormalMatrix / gl_Vertex / gl_Normal /
// gl_MultiTexCoord0 (fixed-function built-ins) become explicit uniforms/attributes; gl_Color
// becomes an explicit u_color uniform set per-draw - the original always rendered this geometry
// right after another draw call had left glColor4f() at the desired constant, never a real
// per-vertex color array.

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord0;

uniform mat4 u_modelView;
uniform mat4 u_projection;
uniform mat4 u_normalMatrix;
uniform vec4 u_color;

out vec3 normal;
out vec4 color;
out vec2 uv;
out vec3 lightDir;
out vec3 position;

void main()
{
   vec3 lightPosition = vec3(30.0, 15.0, -100.0);

   position = vec3(u_modelView * vec4(a_position, 1.0));
   uv = a_texcoord0;

   gl_Position = u_projection * vec4(position, 1.0);
   lightDir = normalize(position - lightPosition);

   normal = normalize(mat3(u_normalMatrix) * a_normal);
   color = u_color;
}
