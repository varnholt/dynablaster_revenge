#pragma once

/// \brief the shared "texalpha" shader most of the menu system's screen-space quad draws use
/// (PSDLayer::render(), MenuPageBackgroundItem's scrolling quad, MenuPageComboBoxItem's
/// drawQuad(), MenuPageTextEditItem's cursor highlight, MenuMouseCursor). MenuDrawable binds
/// it once for the whole page-render pass rather than each of those re-loading/binding it - but
/// a few draws genuinely need to switch to a *different* shader mid-pass (BitmapFont's
/// signed-distance-field text, MenuPageListItem's listhighlight rows). Restoring to *this*
/// shader when they're done (instead of shader 0) is what keeps every draw call after theirs in
/// the same pass working - shader 0 means "no program bound" in GLES3, unlike desktop GL's
/// fixed-function fallback the original code relied on.
///
/// This multiplies the uniform "alpha" against the texture's own sampled alpha (matching the
/// original's real per-item fixed-function GL_MODULATE) - see getFramebufferBlitShader() for the
/// one place that instead wants to *replace* alpha, and texalpha-frag.glsl's comment for how this
/// was found (2026-09-20, comparing against a genuine reference build).
unsigned int getDefaultMenuShader();

//! "alpha" uniform location on getDefaultMenuShader() - loads the shader first if needed.
int getDefaultMenuShaderAlphaParam();

/// \brief the "texalphaignore" shader - used by exactly one real call site in the original,
/// MenuDrawable's page-composite framebuffer blit (client/src/menus/menudrawable.cpp), where the
/// FBO's own alpha channel is accumulated blend residue, not meaningful per-pixel coverage, so
/// replacing it with a controlled fade-alpha uniform is correct. Do not use this for per-item PSD
/// asset draws - see getDefaultMenuShader().
unsigned int getFramebufferBlitShader();

//! "alpha" uniform location on getFramebufferBlitShader() - loads the shader first if needed.
int getFramebufferBlitShaderAlphaParam();
