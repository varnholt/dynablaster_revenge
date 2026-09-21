#include "fullscreenquad.h"
#include "gldevice.h"

FullScreenQuad::FullScreenQuad() : mUnitBuffer(0), mDynamicBuffer(0)
{
   static const float unitQuad[] = {
      -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f,  1.0f, -1.0f, 1.0f, 1.0f,
      -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 1.0f,
   };

   glGenBuffers(1, &mUnitBuffer);
   glBindBuffer(GL_ARRAY_BUFFER, mUnitBuffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(unitQuad), unitQuad, GL_STATIC_DRAW);

   glGenBuffers(1, &mDynamicBuffer);
}

void FullScreenQuad::drawUnit()
{
   glBindBuffer(GL_ARRAY_BUFFER, mUnitBuffer);
   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)0);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

   glDrawArrays(GL_TRIANGLES, 0, 6);

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
}

void FullScreenQuad::drawRect(float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1)
{
   const float verts[] = {
      x0, y0, 0.0f, u0, v0, x1, y0, 0.0f, u1, v0, x1, y1, 0.0f, u1, v1, x0, y0, 0.0f, u0, v0, x1, y1, 0.0f, u1, v1, x0, y1, 0.0f, u0, v1,
   };

   glBindBuffer(GL_ARRAY_BUFFER, mDynamicBuffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)0);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

   glDrawArrays(GL_TRIANGLES, 0, 6);

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
}
