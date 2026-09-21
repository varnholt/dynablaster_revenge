#pragma once

#include <QList>

// Deferred real GL port (see project memory - Phase 5): the real DetonationManager
// (client/src/game/detonationmanager.cpp) is a volumetric flame-shader effect using GL_TEXTURE_1D
// (which doesn't exist in GLES3 at all), raw glBegin/glVertex/glNormal immediate-mode box
// geometry, and a GL_PROJECTION_MATRIX fixed-function query - a real GLES3 port (1D texture
// emulated as 1xN 2D, attribute-array box geometry, ported flame-vert/flame-frag shaders) rather
// than a mechanical copy. Not done in this pass. No-op stand-in with the exact real interface -
// bomb detonations still destroy blocks/players for real (that flows through BombermanClient's
// packet handling regardless of what renders here), there's just no flame visual yet.
class DetonationManager
{
public:
   DetonationManager() = default;

   void init() {}
   void clear() {}
   void addDetonation(int, int, int, int, int, int) {}

   void update(float) {}
   void render() {}
};
