#version 300 es
precision mediump float;

// GLES3 port of client/data/shaders/deathparticles-frag.glsl. gl_PointCoord is a real GLES3
// built-in (unchanged). texture2D -> texture, gl_FragColor -> o_color.
//
// Real fix beyond the mechanical port: multiplies in particleAlpha (the particle's remaining
// life, 1.0 -> 0.0) so particles actually fade out via blending as they age, instead of only
// shrinking via gl_PointSize - see deathparticles-vert.glsl's comment for why the shrink alone
// reads as a sudden pop rather than a fade.

uniform sampler2D texturemap;
uniform sampler2D colormap;

in vec2 uv;
in float particleAlpha;

out vec4 o_color;

void main()
{
   vec4 maskColor = texture(texturemap, gl_PointCoord.xy);
   vec4 color = texture(colormap, uv);

   o_color = maskColor * color;
   o_color.a *= particleAlpha;
}
