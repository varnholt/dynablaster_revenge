#pragma once

// Shared GLES3 replacement for the various glBegin(GL_QUADS) quads the sphere-fragments
// postprocessing chain (BlendQuad, DuplicateAlpha, BlurFilter) each hand-rolled independently in
// the original codebase. Not part of the original - GLES3 has no immediate mode, so this factors
// the one recurring pattern (a position+texcoord attribute quad, drawn as two triangles) into a
// single reusable helper instead of triplicating VBO setup, matching the same spirit as the
// existing shared getDefaultMenuShader() helper (menus/defaultshader.h).
class FullScreenQuad
{
public:
   FullScreenQuad();

   //! draws the static unit quad spanning (-1,-1)-(1,1) at z=-1 with uv 0..1 - used by
   //! BlendQuad/DuplicateAlpha, whose vertex shaders write gl_Position straight from the position
   //! attribute (no camera/projection transform involved at all - confirmed from the original
   //! blendquad-vert.glsl/duplicatealpha-vert.glsl sources, which never multiply by a matrix).
   void drawUnit();

   //! draws an axis-aligned rect from (x0,y0) to (x1,y1) with texcoords (u0,v0)-(u1,v1) - used by
   //! BlurFilter's downsample/gauss passes, which run under an explicit pixel-space ortho
   //! projection and vary the rect size every call (own dynamic vertex buffer, matching the
   //! established MenuPageBackgroundItem precedent for animated-quad geometry).
   void drawRect(float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1);

private:
   unsigned int mUnitBuffer;
   unsigned int mDynamicBuffer;
};
