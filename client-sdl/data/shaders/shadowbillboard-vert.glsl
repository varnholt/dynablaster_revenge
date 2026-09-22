#version 300 es

// GLES3 port of client/data/shaders/shadowbillboard-vert.glsl. Position+texcoord only - this
// mesh has no normal attribute (location 1 is deliberately unused, matching ShadowBillboard's
// own custom vertex format).

layout(location = 0) in vec3 a_position;
layout(location = 2) in vec2 a_texcoord0;

uniform mat4 u_modelViewProjection;

out vec2 uv;

void main()
{
   uv = a_texcoord0;
   gl_Position = u_modelViewProjection * vec4(a_position, 1.0);
}
