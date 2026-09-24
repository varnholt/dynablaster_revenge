#version 300 es
precision highp float;

// GLES3 port of client/data/shaders/deathinitpositions-frag.glsl - unprojects the dying player's
// captured depth buffer back to world space to seed each particle's initial position. The unused
// "boundingRect" uniform from the original (declared, never read) is dropped.

uniform sampler2D depthmap;
uniform mat4 invProj;

in vec2 pos;

out vec4 o_color;

void main()
{
   float z = texture(depthmap, (pos + 1.0) * 0.5).r;

   // unproject (x,y,z) to world space
   vec4 v = invProj * vec4(pos.x, pos.y, z * 2.0 - 1.0, 1.0);

   float t = 1.0 / v.w;

   o_color = vec4(v.xyz * t, 1.0);
}
