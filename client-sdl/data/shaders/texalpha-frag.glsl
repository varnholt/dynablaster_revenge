#version 300 es
precision mediump float;

// Real per-item PSD-asset drawing (PSDLayer::render() and everything that shares its ambient
// bound shader: buttons, labels, backgrounds, comboboxes, textedit, list rows, cursor) needs the
// *real*, sampled per-pixel texture alpha multiplied by the uniform "alpha" - matching the
// original's fixed-function GL_MODULATE (glColor4f(r,g,b, mOpacity*alpha) times the texture's own
// sampled alpha, see PSDLayer::render() in client/src/menus/psdlayer.cpp). Found 2026-09-20 by
// comparing against a genuine reference build: the main-menu background's bomb-pattern watermark
// (and any other PSD asset with real per-pixel transparency) was rendering fully opaque because
// this port originally shared texalphaignore-frag.glsl (which *replaces* alpha, discarding the
// texture's own) for both this per-item case and MenuDrawable's page-composite framebuffer blit -
// only the latter actually wants "ignore" semantics (see that shader's own comment). This is the
// multiplying sibling that everything else needs - do not point the framebuffer blit at this one.
uniform float alpha;
uniform sampler2D tex;

in vec2 uv;
out vec4 o_color;

void main()
{
   vec4 color = texture(tex, uv);
   o_color = vec4(color.rgb, color.a * alpha);
}
