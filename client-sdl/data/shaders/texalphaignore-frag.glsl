#version 300 es
precision mediump float;

// GLES3 port of client/data/shaders/texalphaignore-frag.glsl (texture2D -> texture, varying ->
// in, gl_FragColor -> an explicit out). The "alpha" uniform used to be multiplied against the
// per-vertex color's alpha (vertexColor.a) - always 1.0 at every real call site (see the vertex
// shader) - so it's used directly.
//
// Investigated 2026-09-20 while comparing against a genuine reference build: confirmed via the
// original source that this exact shader's *only* real usage is MenuDrawable's page-composite
// framebuffer blit (client/src/menus/menudrawable.cpp) - i.e. "ignore" is intentional and correct
// there, because the FBO's own alpha channel is accumulated blend residue (this port's shared
// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) blends the alpha channel too, same as the
// original - confirmed the original doesn't use glBlendFuncSeparate either), not meaningful
// per-pixel coverage - replacing it with a controlled fade-alpha uniform is exactly right.
// See texalpha-frag.glsl for the *other* (multiplying) shader real per-item PSDLayer::render()
// calls need - do not point per-item draws (buttons/backgrounds/comboboxes/etc) at this one.
uniform float alpha;
uniform sampler2D tex;

in vec2 uv;
out vec4 o_color;

void main()
{
   vec4 color = texture(tex, uv);
   o_color = vec4(color.rgb, alpha);
}
