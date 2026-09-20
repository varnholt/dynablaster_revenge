#version 300 es

// GLES3 port of client/data/shaders/listhighlight-vert.glsl.
// gl_MultiTexCoord0/1 (fixed-function multitexturing) become two explicit texcoord attributes;
// gl_Color is dropped (only its alpha channel was ever used downstream, and that is now the
// "rowAlpha" uniform set once per row by MenuPageListItem::selectAlpha - see the frag shader).
// ftransform() becomes the same u_modelViewProjection upload every other menu shader uses.

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texcoord0;
layout(location = 2) in vec2 a_texcoord1;

uniform mat4 u_modelViewProjection;

out vec2 uvClamp;
out vec2 uvHighlight;

void main()
{
   uvClamp = a_texcoord0;
   uvHighlight = a_texcoord1;
   gl_Position = u_modelViewProjection * vec4(a_position, 1.0);
}
