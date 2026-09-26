#pragma once

#include "math/matrix.h"

#include <memory>
#include <vector>

class Detonation;

// GLES3 port of client/src/game/detonationmanager.cpp's real volumetric flame effect (see
// project memory for the "verify scope before deferring" correction that led to this - it turned
// out to need a shader port + geometry port, not to be blocked by anything GLES3 architecturally
// lacks). GL_TEXTURE_1D (gradient palette) becomes a 2D texture with height 1; GL_TEXTURE_3D
// (procedural noise volume) is native in GLES3, ported as-is with an R8 instead of ALPHA internal
// format (ALPHA isn't part of GLES3's texImage3D format table). Immediate-mode box geometry
// becomes a dynamic attribute-array buffer, rebuilt per box per frame. glGetFloatv(GL_PROJECTION_
// MATRIX, ...) becomes GLDevice::getProjectionMatrix() (an accessor added for exactly this legacy
// pattern - see its own doc comment).
class DetonationManager
{
public:
   DetonationManager();
   ~DetonationManager();

   void init();
   void clear();
   void addDetonation(int x, int y, int top, int bottom, int left, int right);

   void update(float time);
   void render();

private:
   void drawExplosion(Detonation* det, float time);
   void drawBox(float x, float y, float z, float left, float right, float bottom, float top, int sides);

   float mTime;
   unsigned int mShader;

   unsigned int mNoiseMap;
   unsigned int mGradientMap;

   int mParamTime;
   int mParamCamPos;
   int mParamTop;
   int mParamBottom;
   int mParamLeft;
   int mParamRight;
   int mParamBoundMin;
   int mParamBoundMax;
   int mParamNoiseMap;
   int mParamGradientMap;

   unsigned int mBoxVertexBuffer;

   std::vector<std::unique_ptr<Detonation>> mDetonations;
};
