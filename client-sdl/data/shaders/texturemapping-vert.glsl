#version 300 es

// GLES3 port of client/data/shaders/texturemapping-vert.glsl.
// gl_Vertex/gl_MultiTexCoord0/gl_ModelViewProjectionMatrix (fixed-function built-ins) become
// explicit attributes/uniforms - GLDevice::push() uploads u_modelViewProjection per draw call.

layout(location = 0) in vec3 a_position;
layout(location = 2) in vec2 a_texcoord0;

uniform mat4 u_modelViewProjection;

out vec2 uv;

void main()
{
   uv = a_texcoord0;
   gl_Position = u_modelViewProjection * vec4(a_position, 1.0);
}
