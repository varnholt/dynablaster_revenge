#pragma once

// GLES3 port of client/src/effects/spherefragments/geometryvbo.h. The legacy drawVbos() member
// and the mVertices/mIndices QVector members are dropped - drawVbos() was never actually called by
// any of the three subclasses (each hand-rolls its own glVertexAttribPointer/glDrawElements
// sequence, matching the pattern already established by the ported engine materials), and the
// QVectors were populated nowhere. Genuinely dead code, not ported.

// spherefragments
#include "vertex3d.h"

// forward declarations
class Geometry;

class GeometryVbo
{

public:

   GeometryVbo();
   virtual ~GeometryVbo();

   virtual void initialize();


protected:

   unsigned int mVertexBuffer;
   unsigned int mIndexBuffer;

   Geometry* mGeometry;
};
