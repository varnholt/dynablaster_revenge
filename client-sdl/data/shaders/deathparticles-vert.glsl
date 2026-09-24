#version 300 es

// GLES3 port of client/data/shaders/deathparticles-vert.glsl. gl_Vertex/gl_MultiTexCoord0 become
// explicit attributes; a_position.w carries the particle's remaining life (1.0 -> 0.0, same as
// the original's gl_Vertex.w). gl_ProjectionMatrix becomes u_projection - per PlayerDeathEffect::
// render()'s own comment, the player's modelview transform is already baked into the particle's
// world-space position (captured once at death time), so only the projection is needed here.
//
// Real fix beyond the mechanical port: the original only used `alpha` to shrink gl_PointSize
// (sqrt(alpha)), never passed it down to the fragment stage. Since alpha decays linearly but size
// follows sqrt(alpha), a particle stays close to full size for most of its life and only visibly
// shrinks in the last stretch - reads as a sudden pop, not a fade. Passing alpha through and
// multiplying it into the fragment's output alpha (see deathparticles-frag.glsl) makes particles
// actually dissolve as they age, not just shrink then vanish.

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;

uniform float particleSize;
uniform mat4 u_projection;

out vec2 uv;
out float particleAlpha;

void main()
{
   vec4 p = u_projection * vec4(a_position.xyz, 1.0);

   uv = a_texCoord;

   float alpha = a_position.w;
   particleAlpha = alpha;
   gl_PointSize = particleSize * sqrt(max(alpha, 0.0)) / p.w;

   gl_Position = p;
}
